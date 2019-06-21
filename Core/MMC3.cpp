#include "stdafx.h"
#include "MMC3.h"


bool MMC3::canWriteRAM8(uint16_t addr) {
	return true; // To avoid compatibility issues with MMC6
}

void MMC3::wroteRAM8(uint16_t addr, uint8_t data) {
	if(addr < 0xA000) {
		if(addr % 2 == 0) {
			this->bankSelect = data;
		} else {
			uint8_t Rval = this->bankSelect & 0x7;
			if(Rval >= 6) data &= ~0xC0;
			else if(Rval <= 1) data &= ~0x01;
			this->R[this->bankSelect & 0x7] = data;
		}
	} else if(addr < 0xC000) {
		if(addr % 2 == 0) {
			if(this->nametableMirroringType != NametableMirroringType::FOUR) {
				if(data & 0x01) this->setNametableMirroringType(NametableMirroringType::HORIZONTAL);
				else this->setNametableMirroringType(NametableMirroringType::VERTICAL);
			}
		} else {
			this->WRAMProtect = data & ~0xC;
		}
	} else if(addr < 0xE000) {
		if(addr % 2 == 0) {
			this->IRQLatch = data;
		} else {
			this->IRQCounter = 0;
			this->IRQReload = true;
		}
	} else {
		if(addr % 2 == 0) {
			this->IRQEnabled = false;
			this->IRQCalled = false;
		} else {
			this->IRQEnabled = true;
		}
	}
}

void MMC3::setPPUBusAddress(uint16_t addr, int cycleNum) {
	if(this->prevA12 == 0 && (addr & 0x1000)) {
		if(cycleNum - this->prevCycleNum > 8 || this->CPUcycleCount - this->prevCPUcycleCount > 3) {
			uint8_t prevCount = this->IRQCounter;
			if(this->IRQCounter == 0 || this->IRQReload) this->IRQCounter = this->IRQLatch;
			else this->IRQCounter--;
			if((prevCount || this->IRQReload) && this->IRQCounter == 0 && this->IRQEnabled) this->IRQCalled = true;
			this->IRQReload = false;
		}
		this->prevCPUcycleCount = this->CPUcycleCount;
		this->prevCycleNum = cycleNum;
	}
	this->prevA12 = addr & 0x1000;
}

uint8_t MMC3::getPRGBank(uint16_t& addr) {
	bool bankMode = (this->bankSelect >> 6) & 0x01;
	uint8_t possibilities[2] = {this->R[6], static_cast<uint8_t>(this->header.prgRomSize * 2 - 2)};
	if(addr < 0xA000) {
		addr -= 0x8000;
		return possibilities[bankMode];
	} else if(addr < 0xC000) {
		addr -= 0xA000;
		return this->R[7];
	} else if(addr < 0xE000) {
		addr -= 0XC000;
		return possibilities[!bankMode];
	} else {
		addr -= 0xE000;
		return this->header.prgRomSize * 2 - 1;
	}
}

uint16_t MMC3::getPRGBankSize() {
	return 0x2000;
}

uint8_t MMC3::getCHRBank(uint16_t& addr) {
	bool bankMode = this->bankSelect;
	if(bankMode) {
		if(addr < 0x0400) {
			return this->R[2];
		} else if(addr < 0x0800) {
			addr -= 0x0400;
			return this->R[3];
		} else if(addr < 0x0C00) {
			addr -= 0x0800;
			return this->R[4];
		} else if(addr < 0x1000) {
			addr -= 0x0C00;
			return this->R[5];
		} else if(addr < 0x1800) {
			addr -= 0x1000;
			return this->R[0];
		} else {
			addr -= 0x1800;
			return this->R[1];
		}
	} else {
		if(addr < 0x0800) {
			return this->R[0];
		} else if(addr < 0x1000) {
			addr -= 0x0800;
			return this->R[1];
		} else if(addr < 0x1400) {
			addr -= 0x1000;
			return this->R[2];
		} else if(addr < 0x1800) {
			addr -= 0x1400;
			return this->R[3];
		} else if(addr < 0x1C00) {
			addr -= 0x1800;
			return this->R[4];
		} else {
			addr -= 0x1C00;
			return this->R[5];
		}
	}
}

uint16_t MMC3::getCHRBankSize(uint16_t addr) {
	bool bankMode = this->bankSelect >> 7;
	uint16_t possibilities[2] = {0x400, 0x800};
	if(addr < 0x1000) {
		return possibilities[!bankMode];
	} else {
		return possibilities[bankMode];
	}
}

bool MMC3::WRAMEnabled() {
	return true; // To avoid compatibility issues with MMC6
}