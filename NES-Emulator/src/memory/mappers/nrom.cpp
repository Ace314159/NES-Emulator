#include "stdafx.h"
#include "nrom.h"


NROM::NROM(iNESHeader header, std::vector<uint8_t>& PRG, std::vector<uint8_t>& CHR) : BaseMapper(header) {
	// If there is just 1 16K PRG Block, then it is mirrored to the next 16K, otherwise the next 16K is filled
	// with the next part of the ROM
	std::copy(PRG.begin(), PRG.end(), this->prgRom.begin());
	if(this->header.prgRomSize == 1) {
		std::copy(PRG.begin(), PRG.end(), this->prgRom.begin() + 0x4000);
	}

	std::copy(CHR.begin(), CHR.end(), this->CHR.begin());
}

void NROM::setRAM8(uint16_t addr, uint8_t data) {
	if(addr < 0x6000) return;
	if(addr < 0x8000) this->prgRam[addr - 0x6000] = data;
	else cout << "Tried to edit PRG ROM!" << endl;
		//this->prgRom[addr - 0x8000] = data;
}


uint8_t& NROM::getRAM8(uint16_t addr) {
	if(addr < 0x6000) return temp;
	if(addr < 0x8000) return this->prgRam[addr - 0x6000];
	return this->prgRom[addr - 0x8000];
}

void NROM::setVRAM8(uint16_t addr, uint8_t data) {
	cout << "Tried to edit CHR!" << endl;
	//this->CHR[addr] = data;
}

uint8_t& NROM::getVRAM8(uint16_t addr) {
	return CHR[addr];
}
