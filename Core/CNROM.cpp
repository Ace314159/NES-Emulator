#include "stdafx.h"
#include "CNROM.h"

/*
bool CNROM::canWriteRAM8(uint16_t addr) {
	return false;
}

void CNROM::wroteRAM8(uint16_t addr, uint8_t data) {
	this->CHRBank = data & 0x3;
}

uint8_t CNROM::getPRGBank(uint16_t& addr) {
	addr -= 0x8000;
	return 0;
}

uint16_t CNROM::getPRGBankSize() {
	return 0;
}

uint8_t CNROM::getCHRBank(uint16_t& addr) {
	assert(addr < 0x2000);
	return this->CHRBank;
}

uint16_t CNROM::getCHRBankSize(uint16_t addr) {
	return 0x2000;
}

bool CNROM::WRAMEnabled() {
	return false;
}*/