#include "stdafx.h"
#include "mmc1.h"

MMC1::MMC1(iNESHeader header, PRGBank& PRG, CHRBank& CHR) : BaseMapper(header), 
prgRom(PRG), CHR(CHR)  {}

uint8_t& MMC1::getRAMLoc(uint16_t addr) {
	if(addr < 0x8000) {
		if(!(this->PRGBankReg >> 4)) return this->prgRam[addr - 0x6000];
		return temp;
	}

	uint8_t bankMode = (this->CTRL >> 2) & 0x3;
	uint8_t bank;
	switch(bankMode) {
	case 0:
	case 1:
		bank = this->PRGBankReg & 0x1E;
		break;
	case 2:
		if(addr >= 0xC000) {
			bank = this->PRGBankReg;
			addr -= 0xC000;
		}
		else {
			bank = 0;
			addr -= 0x8000;
		}
		break;
	case 3:
		if(addr < 0xC000) {
			bank = this->PRGBankReg;
			addr -= 0x8000;
		} else {
			bank = this->header.prgRomSize - 1;
			addr -= 0xC000;
		}
		break;
	}
	return this->prgRom[bank][addr];
}

uint8_t& MMC1::getVRAMLoc(uint16_t addr) {
	bool is4K = this->CTRL >> 4;
	uint8_t bank;
	if(is4K) {
		if(addr < 0x1000) bank = this->CHRBank0;
		else bank = this->CHRBank1;
	} else {
		bank = this->CHRBank0 & 0x1E;
	}
	
	return CHR[bank][addr];
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
				else this->PRGBankReg = this->shiftReg;
				
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
