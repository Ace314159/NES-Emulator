#include "stdafx.h"
#include "nrom.h"


void NROM::wroteRAM8(uint16_t addr, uint8_t data) {
	return;
}

uint8_t NROM::getPRGBank(uint16_t& addr) {
	if(addr < 0xC000 || header.prgRomSize > 1) addr -= 0x8000;
	else addr -= 0xC000;
	return 0;
}

uint16_t NROM::getPRGBankSize() {
	return 0x4000;
}

uint8_t NROM::getCHRBank(uint16_t& addr) {
	return 0;
}

uint16_t NROM::getCHRBankSize() {
	return 0x2000;
}

bool NROM::WRAMEnabled() {
	return false;
}
