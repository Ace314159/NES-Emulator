#include "stdafx.h"
#include "UxROM.h"


bool UxROM::canWriteRAM8(uint16_t addr) {
	return false;
}

void UxROM::wroteRAM8(uint16_t addr, uint8_t data) {
	this->PRGBank = data;
}

uint8_t UxROM::getPRGBank(uint16_t& addr) {
	if(addr < 0xC000) {
		addr -= 0x8000;
		return this->PRGBank;
	} else {
		addr -= 0xC000;
		return this->header.prgRomSize - 1;
	}
}

uint16_t UxROM::getPRGBankSize() {
	return 0x4000;
}

uint8_t UxROM::getCHRBank(uint16_t& addr) {
	return 0;
}

uint16_t UxROM::getCHRBankSize(uint16_t addr) {
	return 0x2000;
}

bool UxROM::WRAMEnabled() {
	return false;
}