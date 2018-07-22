#include "mmc1.h"

MMC1::MMC1(std::vector<uint8_t>& PRG, std::vector<uint8_t>& CHR) {
	this->prgRom = PRG;
	this->CHR = CHR;
}

uint8_t& MMC1::getRAMLoc(uint16_t addr) {
	if(addr < 0x8000) return this->prgRam[addr - 0x6000];

	uint8_t bankMode = (this->CTRL >> 2) & 0x3;
	switch(bankMode) {
	case 0:
	case 1:
		addr += (this->PRGBank & 0x1E) * 0x8000;
		break;
	case 2:
		if(addr > 0xC000) addr += this->PRGBank * 0x4000;
		// 0x8000 - 0xC000 is fixed to first bank
		break;
	case 3:
		if(addr < 0xC000) addr += this->PRGBank * 0x4000;
		else addr += (this->header.prgRomSize - 2) * 0x4000;
		break;
	}
	return this->prgRom[addr - 0x8000];
}

uint8_t& MMC1::getVRAMLoc(uint16_t addr) {
	bool is4K = this->CTRL >> 4;
	if(is4K) {
		if(addr < 0x1000) addr += this->CHRBank0 * 0x1000;
		else addr += this->CHRBank1 * 0x1000;
	} else {
		addr += (this->CHRBank0 & 0x1E) * 0x2000;
	}
	
	return CHR[addr];
}

void MMC1::setRAM8(uint16_t addr, uint8_t data) {
	if(addr < 0x6000) return;
	this->getRAMLoc(addr) = data;

	// Register Logic
	if(addr >= 0x8000 && !this->writeCycleDone) {
		if(addr >> 7) {
			this->shiftRegCount = 0;
			this->shiftReg = 0;
		} else {
			this->shiftReg |= (data & 0x1) << this->shiftRegCount;
			this->shiftRegCount++;
			if(this->shiftRegCount == 5) {
				if(addr < 0xA000) {
					this->CTRL = this->shiftReg;
					uint8_t mirroring = this->CTRL & 0x3;
					switch(mirroring) {
					case 0:
						this->setNametableMirroringType(NametableMirroringType::ONE_A);
						break;
					case 1:
						this->setNametableMirroringType(NametableMirroringType::ONE_B);
						break;
					case 2:
						this->setNametableMirroringType(NametableMirroringType::VERTICAL);
						break;
					case 3:
						this->setNametableMirroringType(NametableMirroringType::HORIZONTAL);
						break;
					}
				}
				else if(addr < 0xC000) this->CHRBank0 = this->shiftReg;
				else if(addr < 0xE000) this->CHRBank1 = this->shiftReg;
				else this->PRGBank = this->shiftReg;
				
				this->shiftReg = 0;
				this->shiftRegCount = 0;
			}
		}
	}
}

uint8_t& MMC1::getRAM8(uint16_t addr) {
	if(addr < 0x6000) return temp;
	return this->getRAMLoc(addr);
}

void MMC1::setVRAM8(uint16_t addr, uint8_t data) {
	this->getVRAMLoc(addr) = data;
}

uint8_t& MMC1::getVRAM8(uint16_t addr) {
	return this->getVRAMLoc(addr);
}
