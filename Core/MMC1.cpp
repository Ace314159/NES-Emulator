#include "stdafx.h"
#include "MMC1.h"


bool MMC1::canWriteRAM8(uint16_t addr) {
	return false;
}

void MMC1::wroteRAM8(uint16_t addr, uint8_t data) {
	if(this->CPUcycleCount - this->prevCPUCycleCount <= 1) return;
	if(data >> 7) {
		this->shiftRegCount = 0;
		this->shiftReg = 0;
	} else {
		this->shiftReg |= (data & 0x01) << this->shiftRegCount;
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
	this->prevCPUCycleCount = this->CPUcycleCount;
}

uint8_t MMC1::getPRGBank(uint16_t& addr) {
	uint8_t bankMode = (this->CTRL >> 2) & 0x3;

	if(bankMode < 2) {
		addr -= 0x8000;
		return this->PRGBank & 0x0E;
	} else if(addr < 0xC000) {
		addr -= 0x8000;
		if(bankMode == 2) return 0;
		else return this->PRGBank & 0x0F;
	} else {
		addr -= 0xC000;
		if(bankMode == 2) return this->PRGBank & 0x0F;
		else return this->header.prgRomSize - 1;
	}
}

uint16_t MMC1::getPRGBankSize() {
	uint8_t bankMode = (this->CTRL >> 2) & 0x3;
	if(bankMode < 2) return 0x8000;
	else return 0x4000;
}

uint8_t MMC1::getCHRBank(uint16_t& addr) {
	if(CTRL >> 4) {
		if(addr < 0x1000) return this->CHRBank0;
		else {
			addr -= 0x1000;
			return this->CHRBank1;
		}
	} else return this->CHRBank0 & ~0x01;
}

uint16_t MMC1::getCHRBankSize(uint16_t addr) {
	return 0x1000;
}

bool MMC1::WRAMEnabled() {
	return !(this->PRGBank >> 4);
}
