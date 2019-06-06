#include "stdafx.h"
#include "PPU.h"


PPU::PPU(Memory& m) : mem(m) { }

void PPU::emulateAferCPU() {
	assert(!(this->registerRead && this->registerWritten));
	if(this->registerRead) this->handleRegisterReads();
	else if(this->registerWritten) this->handleRegisterWrites();

	// Checks if an NMI should occur
	if((this->STATUS >> 7) & (this->CTRL >> 7)) {
		mem.NMICalled = this->oldNMI ^ true; // Edge-sensitivity
		if(mem.inNMI) this->oldNMI = true; // Don't change until CPU actually responds
	} else {
		this->oldNMI = false;
	}
	if(mem.inDMA && mem.DMAdoneDummy && (this->mem.mapper->CPUcycleCount % 2)) {
		this->mem.OAM[(this->OAMDMAStartAddr + ((mem.DMAAddr - 1) & 0xff)) % 256] = this->mem.DMAVal;
	}
}

void PPU::handleRegisterReads() {
	switch(this->registerRead) {
	case 0x2002: // STATUS
		this->STATUS &= ~(1 << 7); // Clear VBlank Flag
		this->onSecondWrite = false; // Reset address latch
		break;
	case 0x2007: // DATA
		if(this->currentVramAddr <= 0x3EFF) {
			mem.ppuDATAReadBuffer = mem.getVRAM8(this->currentVramAddr);
		} else {
			mem.ppuDATAReadBuffer = mem.getVRAM8(this->currentVramAddr & ~0x1000);
		}
		this->currentVramAddr += 1 + ((this->CTRL >> 2) & 0x1) * 31;
		break;
	}
	this->registerRead = 0;
}

void PPU::handleRegisterWrites() {
	switch(this->registerWritten) {
	case 0x2000: // CTRL
		// t: ...BA.. ........ = d: ......BA
		this->tempVramAddr = (this->tempVramAddr & ~0xC00) | (this->CTRL & 0x3) << 10;
		break;
	case 0x2004: // OAM DATA
		this->mem.OAM[this->OAMADDR++] = this->OAMDATA;
		this->OAMADDR %= 0x100;
		break;
	case 0x2005: // SCROLL
		if(this->onSecondWrite) {
			// t: CBA..HG FED..... = d: HGFEDCBA
			// Gets low 3 bits and [3-7]
			this->tempVramAddr = (this->tempVramAddr & ~0x73E0) | ((this->SCROLL & 0x7) << 12) |
								 ((this->SCROLL & (0x1F << 3)) << 2);
		} else {
			// Does 15 - fineXScroll for later use
			this->fineXScroll = 15 - (this->SCROLL & 0x7); // x: CBA = d: .....CBA
			// t: ....... ...HGFED = d: HGFED...
			this->tempVramAddr = (this->tempVramAddr & ~0x1F) | this->SCROLL >> 3;
		}
		this->onSecondWrite = !this->onSecondWrite;
		break;
	case 0x2006: // ADDR
		if(this->onSecondWrite) {
			// Sets low 8 bits to the bits in ADDR
			this->tempVramAddr = (this->tempVramAddr & ~0xFF) | this->ADDR;
			this->currentVramAddr = this->tempVramAddr; // Gets all 8 bits
		} else {
			// Sets Bits [8-13] to the low 6 bits in ADDR and clears bit 14
			this->tempVramAddr = (this->tempVramAddr & ~(0x7F << 8)) | ((this->ADDR & 0x3F) << 8);
		}
		this->onSecondWrite = !this->onSecondWrite;
		break;
	case 0x2007: // DATA
		mem.setVRAM8(this->currentVramAddr++ & 0x3FFF, this->DATA);
		this->currentVramAddr += ((this->CTRL >> 2) & 0x1) * 31; // Ads extra 31 if bit 2 is set
		break;
	case 0x4014: // OAM DMA
		mem.DMAPage = this->OAMDMA;
		mem.DMAAddr = this->OAMDMA << 8; // OAMDMA has the high byte
		mem.inDMA = true;
		this->OAMDMAStartAddr = this->OAMADDR;
		break;
	}
	this->registerWritten = 0;
}

void PPU::emulateDot() {
	if(this->cycleNum == 0) { // Idle
		if(this->scanlineNum == -1) this->oddFrame = !this->oddFrame;
		this->cycleNum++;
		return;
	} else if(this->scanlineNum == -1) { // Pre-render Scanline
		if(this->cycleNum == 1) {
			this->STATUS &= ~(0b111 << 5); // Clear VBlank, Sprite 0, and Sprite Overflow
			this->sprite0IsInSOAM = this->sprite0Hit = false;
		} else if(this->isRendering() && this->cycleNum >= 280 && this->cycleNum <= 304) {
			// vert(v) = vert(t)
			this->currentVramAddr = (this->currentVramAddr & ~0x7BE0) | (this->tempVramAddr & 0x7BE0);
		}
	}


	if(this->scanlineNum < 240 && this->isRendering()) { // Pre-render Scanline and Visible Scanlines
		if(this->scanlineNum != -1 && this->cycleNum <= 64) { // Secondary OAM Clear
			this->secondaryOAM[this->cycleNum % 32] = 0xFF;
			this->secondaryOAMBuffer = 0xFF;
			this->spritesFound = 0;
			this->spriteEvaluationDone = false;
		}

		if(this->cycleNum <= 256 || this->cycleNum > 320) {
			this->fetchBGData();
			if(this->scanlineNum != -1 && this->cycleNum > 64 && this->cycleNum <= 256) this->evaluateSprites();
			if(this->scanlineNum != -1 && this->cycleNum <= 256) this->renderDot();
			if(this->cycleNum >= 2 && this->cycleNum <= 257 || this->cycleNum >= 322 && this->cycleNum <= 337) {
				this->lowBGShiftReg <<= 1;
				this->highBGShiftReg <<= 1;
				this->lowBGAttrShiftReg <<= 1;
				this->highBGAttrShiftReg <<= 1;
			}
			if(this->cycleNum % 8 == 0) this->incrementScrollX(); // Inc.hori(v)
			if(this->cycleNum == 256) this->incrementScrollY(); // Inc. vert(v)
		}
		else {
			if(this->cycleNum == 257) {
				this->prevSpritesFound = this->spritesFound;
				this->prevSprite0IsInSOAM = this->sprite0IsInSOAM;
				// hori(v) = hori(t)
				this->currentVramAddr = (this->currentVramAddr & ~0x41F) | (this->tempVramAddr & 0x41F);
			}
			this->fetchSpriteData();
		}
	} else if(this->scanlineNum == 241 && this->cycleNum == 1) {
		this->STATUS |= (1 << 7); // Set VBlank flag
		this->window.renderScreen();
	} else if(this->scanlineNum != -1 && this->scanlineNum < 240 && this->cycleNum <= 256)
		this->setDot(this->getBGColor(0, 0));

	// Skip one cycle if odd frame and is rendering
	if(this->scanlineNum == -1 && this->cycleNum == 338 && this->isRenderingBG()) this->cycleNum += this->oddFrame;
	if(this->cycleNum == 340) {
		this->scanlineNum = ((this->scanlineNum + 1 + 1) % (261 + 1)) - 1;
		this->cycleNum = 0;
	} else this->cycleNum++;
}


// Useful Functions
bool PPU::isRenderingBG() {
	return (this->MASK >> 3) & 0x1;
}

bool PPU::isRenderingSprites() {
	return (this->MASK >> 4) & 0x1;
}

bool PPU::isRendering() {
	return (this->MASK >> 3) & 0x3;
}

void PPU::renderDot() {
	this->chosenSpritePixelIndex = -1;
	this->chosenSpritePixelColor = 0;
	this->chosenSpritePixelIsBehindBG = 1;
	if(this->isRenderingSprites() && (((this->MASK >> 2) & 0x1) || this->cycleNum > 8)) {
		this->selectSpritePixel();
	}
	// Background Data - fineXScroll is already 15 - fineXScroll
	uint8_t bgPaletteNum;
	uint8_t bgColorNum;
	if(!this->isRenderingBG() || !(((this->MASK >> 1) & 0x1) || this->cycleNum > 8)) bgPaletteNum = bgColorNum = 0;
	else {
		uint8_t lowAttrBit = (this->lowBGAttrShiftReg >> this->fineXScroll) & 0x1;
		uint8_t highAttrBit = (this->highBGAttrShiftReg >> this->fineXScroll) & 0x1;
		uint8_t lowBGBit = (this->lowBGShiftReg >> this->fineXScroll) & 0x1;
		uint8_t highBGBit = (this->highBGShiftReg >> this->fineXScroll) & 0x1;
		bgPaletteNum = (highAttrBit << 1) | lowAttrBit;
		bgColorNum = (highBGBit << 1) | lowBGBit;
	}
	// Priority Multiplexer Decision
	uint8_t color;
	if(this->chosenSpritePixelColor == 0) {
		color = this->getBGColor(bgPaletteNum, bgColorNum);
	} else  {
		uint8_t spriteColorNum = this->chosenSpritePixelColor;
		if(!this->chosenSpritePixelIsBehindBG || bgColorNum == 0) {
			uint8_t spritePaletteNum = this->spriteAttrs[this->chosenSpritePixelIndex] & 0x3;
			color = this->getSpriteColor(spritePaletteNum, spriteColorNum);
		} else {
			color = this->getBGColor(bgPaletteNum, bgColorNum);
		}
		// Set Sprite 0 Hit based on condition
		if(this->chosenSpritePixelIndex == 0 && this->prevSprite0IsInSOAM && spriteColorNum != 0 && 
			bgColorNum != 0 && !this->sprite0Hit && this->cycleNum != 255 + 1) {
			this->sprite0Hit = true;
			this->STATUS |= (1 << 6); // Sets bit 6 or Sprite 0 Hit
		}
	}

	this->setDot(color);
}

void PPU::setDot(uint8_t color) {
	// TODO: Add emphasis based on PPU MASK
	if(this->MASK & 0x1) color &= 0x30;
	Color rgbColor = this->paletteTable[color];

	int index = ((Window::height - 1 - this->scanlineNum) * Window::width) + this->cycleNum - 1;
	this->window.screenTexPixels[index] = rgbColor;
}

void PPU::fetchBGData() {
	uint8_t fineY = (this->currentVramAddr >> 12) & 0x7;
	uint16_t bgPage = (this->CTRL << 8) & 0x1000;
	uint16_t v = this->currentVramAddr;
	switch(this->cycleNum % 8) {
	case 1: 
		if(this->cycleNum >= 9 && this->cycleNum <= 257 || this->cycleNum >= 329 && this->cycleNum <= 337) {
			this->highBGShiftReg |= this->highTileByte;
			this->lowBGShiftReg |= this->lowTileByte;

			this->highBGAttrShiftReg |= (this->paletteNum >> 1) * 0xFF;
			this->lowBGAttrShiftReg |= (this->paletteNum & 0x1) * 0xFF;
		}
		break;
	case 2: // NT Byte
		this->tileNum = mem.getNametableLoc(v & 0x0FFF);
		break;
	case 4: // AT Byte
		{
		uint8_t atByte = mem.getNametableLoc(0x03C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07));
		uint8_t yBit = (v & 0x40) >> 5; // Bit 1 of coarse y in pos 1
		uint8_t xBit = (v & 0x2) >> 1; // Bit 1 of carse x
		// yx is used to select the corresponding 2 bits from the attribute byte
		this->paletteNum = (atByte >> ((yBit | xBit) * 2)) & 0x3;
		}
		break;
	case 6: // Low BG Tile Byte
		this->lowTileByte = mem.getCHRLoc(bgPage | (this->tileNum << 4) | fineY);
		break;
	case 0: // High BG Tile Byte
		this->highTileByte = mem.getCHRLoc(bgPage | (this->tileNum << 4) | 0x8 | fineY);
		break;
	}
}

void PPU::evaluateSprites() { // Sprite Evaluation
	if(this->spritesFound < 8 && !this->spriteEvaluationDone) {
		if(this->cycleNum % 2 == 1) this->secondaryOAMBuffer = this->mem.OAM[this->OAMADDR];
		else {
			uint8_t m = this->OAMADDR % 4; // TODO: Treat first OAMADDR as y coordinate, 
			this->secondaryOAM[4 * this->spritesFound + m] = secondaryOAMBuffer;
			// Increment OAMADDR when sprite is in range otherwise skip sprite
			int diff = this->scanlineNum - secondaryOAMBuffer;  // Checks with Y only when m = 0
			if(diff >= 0 && diff <= 7 + ((this->CTRL & 0x20) >> 2) || m != 0) {
				if(OAMADDR == 0) this->sprite0IsInSOAM = true;
				if(++this->OAMADDR % 4 == 0) this->spritesFound++; // Finished loading sprite data
			}
			else this->OAMADDR += 4; // Go to next sprite
			if(this->OAMADDR == 0) this->spriteEvaluationDone = true; // OAMADDR overflowed
		}
	} else {
		// TODO: Implement Buggy Behavior of Sprite Overflow
		// this->OAMADDR++;
	}
}

void PPU::fetchSpriteData() { // Sprite Fetches
	this->OAMADDR = 0;
	int spriteNum = (this->cycleNum - 1) / 8 - 32;
	if(spriteNum >= this->prevSpritesFound) return;
	bool is8x16 = (this->CTRL >> 5) & 0x1;
	// isVerticallyFlipped is used before spriteAttrs is set, so get data from Secondary OAM
	bool isVerticallyFlipped = (this->secondaryOAM[spriteNum * 4 + 2] >> 7) & 0x1;
	uint8_t fineY = (this->scanlineNum - this->spriteY) & ~0x8;
	if(isVerticallyFlipped) fineY = ~fineY & 0x7;
	uint16_t spritePage;
	// Use value from Secondary OAM instead of tileNum as bit 0 changes for top and bottom half
	if(is8x16) spritePage = (this->secondaryOAM[spriteNum * 4 + 1] & 0x1) << 12;
	else spritePage = (this->CTRL << 9) & 0x1000;

	switch(this->cycleNum % 8) {
	case 1: // Sprite Y
		this->spriteY = this->secondaryOAM[spriteNum * 4 + 0];
		break;
	case 2: // Sprite Tile Num
		if(is8x16) {
			// Top Half
			if((this->scanlineNum - this->spriteY < 8) != isVerticallyFlipped)
				this->tileNum = this->secondaryOAM[spriteNum * 4 + 1] & 0xFE;
			else // Bottom Half
				this->tileNum = this->secondaryOAM[spriteNum * 4 + 1] | 0x1;
		} else this->tileNum = this->secondaryOAM[spriteNum * 4 + 1];
		break;
	case 3: // Sprite Attribute
		this->spriteAttrs[spriteNum] = this->secondaryOAM[spriteNum * 4 + 2];
		break;
	case 4: // Sprite X
		this->spriteXs[spriteNum] = this->secondaryOAM[spriteNum * 4 + 3];
		break;
	case 6: // Low Sprite Tile Byte
		this->lowTileByte = mem.getCHRLoc(spritePage | (this->tileNum << 4) | fineY);
		if(((this->spriteAttrs[spriteNum] >> 6) & 0x1) == 1) 
			this->lowTileByte = this->BitReverseTable[this->lowTileByte];
		this->lowSpriteShiftRegs[spriteNum] = this->lowTileByte;
		break;
	case 0: // Hgih Sprite Tile Byte
		this->highTileByte = mem.getCHRLoc(spritePage | (this->tileNum << 4) | 0x8 | fineY);
		if(((this->spriteAttrs[spriteNum] >> 6) & 0x1) == 1)
			this->highTileByte = this->BitReverseTable[this->highTileByte];
		this->highSpriteShiftRegs[spriteNum] = this->highTileByte;
		break;
	}
}

void PPU::selectSpritePixel() {
	for(int i = 0; i < this->prevSpritesFound; i++) {
		if(this->cycleNum - 1 >= this->spriteXs[i] && this->cycleNum - 1 <= this->spriteXs[i] + 7) {
			uint8_t low = this->lowSpriteShiftRegs[i] << (this->cycleNum - 1 - this->spriteXs[i]);
			uint8_t high = this->highSpriteShiftRegs[i] << (this->cycleNum - 1 - this->spriteXs[i]);

			uint8_t lowSpriteBit = (low >> 7) & 0x1;
			uint8_t highSpriteBit = (high >> 6) & 0x2;
			uint8_t color = highSpriteBit | lowSpriteBit;
			if(color != 0) {
				this->chosenSpritePixelIndex = i;
				this->chosenSpritePixelColor = color;
				this->chosenSpritePixelIsBehindBG = (this->spriteAttrs[i] >> 5) & 0x1;
				break;
			}
			/*int priority = (this->spriteAttrs[i] >> 5) & 0x1;
			int chosenPriority = 1; // 0 is in front of BG, 1 is behind BG
			if(this->chosenSpritePixelIndex != -1)
				chosenPriority = (this->spriteAttrs[this->chosenSpritePixelIndex] >> 5) & 0x1;
			if((this->chosenSpritePixelIndex == -1 || priority < chosenPriority) ) {
				this->chosenSpritePixelIndex = i;
				this->chosenSpritePixelColor = color;
			}*/
		}
	}
}

// From https://wiki.nesdev.com/w/index.php/PPU_scrolling#Wrapping_around
void PPU::incrementScrollX() {
	if((this->currentVramAddr & 0x001F) == 31) {
		// Switch horizontal nametable
		this->currentVramAddr = (this->currentVramAddr & ~0x001F) ^ 0x0400;
	} else {
		this->currentVramAddr++;
	}
}

// From https://wiki.nesdev.com/w/index.php/PPU_scrolling#Wrapping_around
void PPU::incrementScrollY() {
	if((this->currentVramAddr & 0x7000) != 0x7000) { // Fine y < 7
		this->currentVramAddr += 0x1000; // Increment fine y
	} else {
		this->currentVramAddr &= ~0x7000; // Fine Y = 0
		int coarseY = (this->currentVramAddr & 0x03E0) >> 5;
		if(coarseY == 29) {
			coarseY = 0;
			this->currentVramAddr ^= 0x0800; // Switch vertical nametable
		} else if(coarseY == 31) {
			coarseY = 0;
		} else {
			coarseY += 1;
		}
		// Put coarse y back
		this->currentVramAddr = (this->currentVramAddr & ~0x03E0) | (coarseY << 5);
	}
}

uint8_t PPU::getBGColor(uint8_t paletteNum, uint8_t colorNum) {
	uint8_t color;
	if(this->isRenderingBG()) {
		if(colorNum == 0) color = mem.getPaletteLoc(0);
		else color = mem.getPaletteLoc((paletteNum << 2) | colorNum);
	} else {
		if(this->currentVramAddr >= 0x3F00 && this->currentVramAddr <= 0x3FFF) 
			color = mem.getPaletteLoc(this->currentVramAddr - 0x3F00);
		color = mem.getPaletteLoc(0);
	}
	return color;
}

uint8_t PPU::getSpriteColor(uint8_t paletteNum, uint8_t colorNum) {
	assert(colorNum != 0);
	uint8_t color = mem.getPaletteLoc(0x0010 | (paletteNum << 2) | colorNum);
	return color;
}

void PPU::setPaletteTable(Color* paletteData) {
	std::copy(paletteData, paletteData + this->paletteTable.size(), this->paletteTable.data());
}
