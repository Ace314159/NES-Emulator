#include "stdafx.h"
#include "MMC3.h"


bool MMC3::canWriteRAM8(uint16_t addr) {
	return (this->WRAMProtect & 0x80) && addr < 0x8000; // To avoid compatibility issues with MMC6
}

void MMC3::wroteRAM8(uint16_t addr, uint8_t data) {
	uint8_t Rval;
	switch(addr & 0xE001) {
	case 0x8000:
		this->bankSelect = data;
		break;
	case 0x8001:
		Rval = this->bankSelect & 0x7;
		if(Rval >= 6) data &= ~0xC0;
		else if(Rval <= 1) data &= ~0x01;
		this->R[Rval] = data;
		break;
	case 0xA000:
		if(this->nametableMirroringType != NametableMirroringType::FOUR) {
			if(data & 0x01) this->setNametableMirroringType(NametableMirroringType::HORIZONTAL);
			else this->setNametableMirroringType(NametableMirroringType::VERTICAL);
		}
		break;
	case 0xA001:
		this->WRAMProtect = data & ~0xC;
		break;
	case 0xC000:
		this->IRQLatch = data;
		break;
	case 0xC001:
		this->IRQCounter = 0;
		this->IRQReload = true;
		break;
	case 0xE000:
		this->IRQEnabled = false;
		this->IRQCalled = false;
		break;
	case 0xE001:
		this->IRQEnabled = true;
		break;
	default:
		assert(false);
		break;
	}
}

void MMC3::setPPUBusAddress(uint16_t addr, int cycleNum) {
	if(!this->prevA12 && (addr & 0x1000)) {
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
	bool bankMode = this->bankSelect & 0x40;
	uint8_t possibilities[2] = {this->R[6], static_cast<uint8_t>(this->header.prgRomSize * 2 - 2)};

	uint16_t bank = addr & 0xE000;
	addr -= bank;
	switch(bank) {
	case 0x8000:
		return possibilities[bankMode];
		break;
	case 0xA000:
		return R[7];
		break;
	case 0xC000:
		return possibilities[!bankMode];
		break;
	case 0xE000:
		return this->header.prgRomSize * 2 - 1;
		break;
	default:
		assert(false);
		return -1;
		break;
	}
}

uint16_t MMC3::getPRGBankSize() {
	return 0x2000;
}

uint8_t MMC3::getCHRBank(uint16_t& addr) {
	bool bankMode = this->bankSelect & 0x80;
	uint16_t bank = addr & 0x1C00;
	addr -= bank;

	if(bankMode) {
		switch(bank) {
		case 0x0000:
			return this->R[2];
			break;
		case 0x0400:
			return this->R[3];
			break;
		case 0x0800:
			return this->R[4];
			break;
		case 0x0C00:
			return this->R[5];
			break;
		case 0x1000:
			return this->R[0];
			break;
		case 0x1400:
			return this->R[0] | 0x01;
			break;
		case 0x1800:
			return this->R[1];
			break;
		case 0x1C00:
			return this->R[1] | 0x01;
			break;
		default:
			assert(false);
			return -1;
			break;
		}
	} else {
		switch(bank) {
		case 0x0000:
			return this->R[0];
			break;
		case 0x0400:
			return this->R[0] | 0x01;
			break;
		case 0x0800:
			return this->R[1];
			break;
		case 0x0C00:
			return this->R[1] | 0x01;
			break;
		case 0x1000:
			return this->R[2];
			break;
		case 0x1400:
			return this->R[3];
			break;
		case 0x1800:
			return this->R[4];
			break;
		case 0x1C00:
			return this->R[5];
			break;
		default:
			assert(false);
			return -1;
			break;
		}
	}
}

uint16_t MMC3::getCHRBankSize(uint16_t addr) {
	return 0x0400;
}

bool MMC3::WRAMEnabled() {
	return true; // To avoid compatibility issues with MMC6
}