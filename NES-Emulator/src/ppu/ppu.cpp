#include "ppu.h"

#include <iostream>
#include <fstream>

#include <io.h>
#include <fcntl.h>

using std::cout;
using std::endl;

PPU::PPU(Memory::PPU& m) : mem(m) { this->initPaletteTable("../res/Nintendulator-NTSC.pal"); }

void PPU::emulateCycle(bool afterCPU) {
	if(afterCPU) {
		this->handleRegisterReads();
		this->handleRegisterWrites();

		// Checks if an NMI should occur
		if((this->STATUS >> 7) & (this->CTRL >> 7)) {
			mem.NMICalled = this->oldNMI ^ true; // Edge-sensitivity
			if(mem.inNMI) this->oldNMI = true; // Don't change until CPU actually responds
		} else {
			this->oldNMI = false;
		}
		if(mem.inDMA && mem.inOddCycle && mem.DMAdoneDummy) {
			this->mem.OAM[(this->OAMDMAStartAddr + ((mem.DMAAddr - 1) & 0xff)) % 256] = this->mem.DMAVal;
		}
	}

	this->emulateDot();
}

void PPU::handleRegisterReads() {
	switch(this->registerRead) {
	case 0x2002: // STATUS
		this->STATUS &= ~(1 << 7); // Clear VBlank Flag
		this->onSecondWrite = false; // Reset address latch
		break;
	case 0x2007: // DATA
		if(this->currentVramAddr <= 0x3EFF) {
			mem.DATAReadBuffer = mem.get8(this->currentVramAddr);
		} else {
			mem.DATAReadBuffer = mem.get8(this->currentVramAddr & ~0x1000);
		}
		this->currentVramAddr += 1 + ((this->CTRL >> 2) & 0x1) * 31;
		break;
	default:
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
		break;
	case 0x2005: // SCROLL
		if(this->onSecondWrite) {
			// t: CBA..HG FED..... = d: HGFEDCBA
			// Gets low 3 bits and [3-7]
			this->tempVramAddr = (this->tempVramAddr & ~0x73E0) | ((this->SCROLL & 0x7) << 12) |
								 ((this->SCROLL & (0x1F << 3)) << 2);
		} else {
			this->fineXScroll = this->SCROLL & 0x7; // x: CBA = d: .....CBA
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
		mem.set8(this->currentVramAddr++, this->DATA);
		this->currentVramAddr += ((this->CTRL >> 2) & 0x1) * 31; // Ads extra 31 if bit 2 is set
		break;
	case 0x4014: // OAM DMA
		mem.DMAPage = this->OAMDMA;
		mem.DMAAddr = this->OAMDMA << 8; // OAMDMA has the high byte
		mem.inDMA = true;
		this->OAMDMAStartAddr = this->OAMADDR;
		break;
	default:
		break;
	}
	this->registerWritten = 0;
}

void PPU::emulateDot() {
	if(this->scanlineNum == 0 && this->cycleNum == 0 && !this->oddFrame && this->isRendering()) {
		this->cycleNum++;
	} else if(this->cycleNum == 0) {
		this->cycleNum++;
		return;
	} else if(this->scanlineNum == -1) { // Pre-render Scanline
		if(this->cycleNum == 1) {
			this->STATUS &= ~(0b111 << 5); // Clear VBlank, Sprite 0, and Sprite Overflow
			this->sprite0IsInSOAM = this->sprite0Hit = false;
		}
		else if(this->isRendering() && this->cycleNum >= 280 && this->cycleNum <= 304) {
			// vert(v) = vert(t)
			this->currentVramAddr = (this->currentVramAddr & ~0x7BE0) | (this->tempVramAddr & 0x7BE0);
		}
	}


	if(this->scanlineNum < 240 && this->isRendering()) { // Pre-render Scanline and Visible Scanlines
		if(this->cycleNum <= 64 && this->scanlineNum != -1) { // Secondary OAM Clear
			this->secondaryOAM[this->cycleNum % 32] = 0xFF;
			this->secondaryOAMBuffer = 0xFF;
			this->spritesFound = 0;
			this->spriteEvaluationDone = false;
		}

		if(this->cycleNum <= 256 || this->cycleNum > 320) {
			this->fetchBGData();
			if(this->cycleNum >= 65 && this->cycleNum <= 256 && this->scanlineNum != -1) this->evaluateSprites();
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
		else if(this->cycleNum > 256 && this->cycleNum <= 320)
			this->fetchSpriteData();
		if(this->cycleNum == 257) // hori(v) = hori(t)
			this->currentVramAddr = (this->currentVramAddr & ~0x41F) | (this->tempVramAddr & 0x41F);


	} else if(this->scanlineNum == 241 && this->cycleNum == 1) {
		this->STATUS |= (1 << 7); // Set VBlank flag
		Graphics::renderScreen();
	}

	// Skip one cycle if odd frame and is rendering
	if(this->oddFrame && this->cycleNum == 339 && this->scanlineNum == -1 && this->isRendering()) this->cycleNum++;
	if(this->cycleNum == 340) this->scanlineNum = ((this->scanlineNum + 1+1) % (261+1))-1;
	this->cycleNum = (this->cycleNum + 1) % 341;
	if(this->scanlineNum == -1 && this->cycleNum == 0) this->oddFrame = !this->oddFrame;
}


// Useful Functions
bool PPU::isRenderingBG() {
	return (this->MASK >> 3) & 0x1;
}

bool PPU::isRenderingSprites() {
	return (this->MASK >> 4) & 0x1;
}

bool PPU::isRendering() {
	return this->isRenderingBG() || this->isRenderingSprites();
}

void PPU::renderDot() {
	this->chosenSpritePixelIndex = -1;
	if(this->isRenderingSprites() && (((this->MASK >> 2) & 0x1) || this->cycleNum > 8)) {
		this->selectSpritePixel();
	}
	// Background Data
	uint8_t lowAttrBit = (this->lowBGAttrShiftReg >> (15 - this->fineXScroll)) & 0x1;
	uint8_t highAttrBit = (this->highBGAttrShiftReg >> (15 - this->fineXScroll)) & 0x1;
	uint8_t lowBGBit  = (this->lowBGShiftReg  >> (15 - this->fineXScroll)) & 0x1;
	uint8_t highBGBit = (this->highBGShiftReg >> (15 - this->fineXScroll)) & 0x1;
	uint8_t bgPaletteNum = (highAttrBit << 1) | lowAttrBit;
	uint8_t bgColorNum = (highBGBit << 1) | lowBGBit;
	if(!(this->isRenderingBG() && (((this->MASK >> 1) & 0x1) || this->cycleNum > 8))) bgColorNum = 0;
	// Priority Multiplexer Decision
	Color color;
	if(this->chosenSpritePixelIndex == -1) {
		color = this->getBGColor(bgPaletteNum, bgColorNum);
	} else {
		// Sprite Data
		uint8_t spritePaletteNum = this->spriteAttrs[this->chosenSpritePixelIndex] & 0x3;
		uint8_t spriteColorNum = this->chosenSpritePixelColor;
		bool spritePriorityIs1 = (this->spriteAttrs[this->chosenSpritePixelIndex] >> 5) & 0x1;
		if(spritePriorityIs1 || spriteColorNum == 0) color = this->getBGColor(bgPaletteNum, bgColorNum);
		else color = this->getSpriteColor(spritePaletteNum, spriteColorNum);
		if(this->chosenSpritePixelIndex == 0 && this->sprite0IsInSOAM && spriteColorNum != 0 && bgColorNum != 0 &&
			!this->sprite0Hit && this->cycleNum != 255 + 1) {
			this->sprite0Hit = true;
			cout << (int)mem.OAM[3] << " " << (int)MASK << " " << (int)spriteColorNum << endl;
			// exit(0);
			this->STATUS |= (1 << 6); // Sets bit 6 or Sprite 0 Hit
		}
	}

	// TODO: Add emphasis based on PPU MASK
	int index = ((Graphics::height-1 - this->scanlineNum) * (Graphics::width)*3) + (3*(this->cycleNum-1));
	Graphics::screenTexPixels[index]     = color.R;
	Graphics::screenTexPixels[index + 1] = color.G;
	Graphics::screenTexPixels[index + 2] = color.B;
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
		this->tileNum = mem.get8(0x2000 | (v & 0x0FFF));
		break;
	case 4: // AT Byte
		{
		uint8_t atByte = mem.get8(0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07));
		uint8_t yBit = (v & 0x40) >> 5; // Bit 1 of coarse y in pos 1
		uint8_t xBit = (v & 0x2) >> 1; // Bit 1 of carse x
		// yx is used to select the corresponding 2 bits from the attribute byte
		this->paletteNum = (atByte >> ((yBit | xBit) * 2)) & 0x3;
		}
		break;
	case 6: // Low BG Tile Byte
		this->lowTileByte = mem.get8(bgPage | (this->tileNum << 4) | fineY);
		break;
	case 0: // High BG Tile Byte
		this->highTileByte = mem.get8(bgPage | (this->tileNum << 4) | 8 | fineY);
		break;
	}
}

void PPU::evaluateSprites() { // Sprite Evaluation
	if(this->spritesFound < 8 && !this->spriteEvaluationDone) {
		if(this->cycleNum % 2 == 1) this->secondaryOAMBuffer = this->mem.OAM[this->OAMADDR];
		else {
			uint8_t m = this->OAMADDR % 4;
			this->secondaryOAM[4 * this->spritesFound + m] = secondaryOAMBuffer;
			// Increment OAMADDR when sprite is in range otherwise skip sprite
			int diff = this->scanlineNum - secondaryOAMBuffer;  // Checks with Y only when m = 0
			if((diff >= 0 && diff <= 7 + ((this->CTRL & 0x20) >> 2)) || m != 0) {
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
	this->prevSpritesFound = this->spritesFound;
	int spriteNum = (this->cycleNum - 1) / 8 - 32;
	if(spriteNum + 1 > this->prevSpritesFound) return;
	bool is8x16 = (this->CTRL >> 5) & 0x1;
	uint16_t spritePage;
	bool isVerticallyFlipped = (this->spriteAttrs[spriteNum] >> 7) & 0x1;
	uint8_t fineY = this->scanlineNum - this->spriteY;
	if(isVerticallyFlipped) fineY = ~fineY & 0x7;
	if(is8x16) spritePage = (this->tileNum & 0x1) << 12;
	else spritePage = (this->CTRL << 9) & 0x1000;

	switch(this->cycleNum % 8) {
	case 1: // Sprite Y
		this->spriteY = this->secondaryOAM[spriteNum * 4 + 0];
		break;
	case 2: // Sprite Tile Num
		if(is8x16) {
			// Top Half
			// TODO: Does not work
			if((this->spriteY - this->scanlineNum <= 8) != isVerticallyFlipped)
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
		this->lowTileByte = mem.get8(spritePage | (this->tileNum << 4) | fineY);
		if(((this->spriteAttrs[spriteNum] >> 6) & 0x1) == 1) 
			this->lowTileByte = this->BitReverseTable[this->lowTileByte];
		this->lowSpriteShiftRegs[spriteNum] = this->lowTileByte;
		break;
	case 0: // Hgih Sprite Tile Byte
		this->highTileByte = mem.get8(spritePage | (this->tileNum << 4) | 8 | fineY);
		if(((this->spriteAttrs[spriteNum] >> 6) & 0x1) == 1)
			this->highTileByte = this->BitReverseTable[this->highTileByte];
		this->highSpriteShiftRegs[spriteNum] = this->highTileByte;
		break;
	}
}

void PPU::selectSpritePixel() {
	this->chosenSpritePixelIndex = -1;
	for(int i = 0; i < this->prevSpritesFound; i++) {
		if(this->cycleNum - 1 >= this->spriteXs[i] && this->cycleNum - 1 <= this->spriteXs[i] + 7) {
			uint8_t low = this->lowSpriteShiftRegs[i] << (this->cycleNum - 1 - this->spriteXs[i]);
			uint8_t high = this->highSpriteShiftRegs[i] << (this->cycleNum - 1- this->spriteXs[i]);

			uint8_t lowSpriteBit = (low >> 7) & 0x1;
			uint8_t highSpriteBit = (high >> 6) & 0x2;
			uint8_t color = highSpriteBit | lowSpriteBit;
			int priority = (this->spriteAttrs[i] >> 5) & 0x1;
			int chosenPriority = 0;
			if(this->chosenSpritePixelIndex != -1)
				chosenPriority = (this->spriteAttrs[this->chosenSpritePixelIndex] >> 5) & 0x1;
			if((this->chosenSpritePixelIndex == -1 || priority > chosenPriority) && color != 0) {
				this->chosenSpritePixelIndex = i;
				this->chosenSpritePixelColor = color;
			}
		}
	}
}

void PPU::incrementScrollX() {
	if((this->currentVramAddr & 0x001F) == 31) {
		// Switch horizontal nametable
		this->currentVramAddr = (this->currentVramAddr & ~0x001F) ^ 0x0400;
	} else {
		this->currentVramAddr++;
	}
}

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

PPU::Color PPU::getBGColor(uint8_t paletteNum, uint8_t colorNum) {
	uint8_t color;
	if(this->isRenderingBG()) {
		if(colorNum == 0) color = mem.get8(0x3F00);
		else color = mem.get8(0x3F00 | (paletteNum << 2) | colorNum);
	} else {
		if(this->currentVramAddr >= 0x3F00 && this->currentVramAddr <= 0x3FFF) 
			color = mem.get8(this->currentVramAddr);
		color = mem.get8(0x3F00);
	}
	return this->paletteTable[color];
}

PPU::Color PPU::getSpriteColor(uint8_t paletteNum, uint8_t colorNum) {
	if(colorNum == 0) throw std::runtime_error("Sprite Color was 0");
	uint8_t color = mem.get8(0x3F10 | (paletteNum << 2) | colorNum);
	return this->paletteTable[color];
}

void PPU::initPaletteTable(std::string paletteFile) {
	std::basic_ifstream<uint8_t> paletteData(paletteFile, std::ios::binary);
	if(!paletteData.good()) {
		throw std::runtime_error("Could not read palette table file");
	}

	uint8_t buffer[3];
	for(size_t i = 0; i < this->paletteTable.max_size(); i++) {
		paletteData.read(buffer, 3);
		this->paletteTable[i] = {buffer[0], buffer[1], buffer[2]};
	}
}
