#include "nrom.h"

NROM::NROM(std::vector<uint8_t>& PRG, std::vector<uint8_t>& CHR, uint8_t prgRamSize) {
	// TODO: Use prgRamSize to limit RAM

	// If there is just 1 16K PRG Block, then it is mirrored to the next 16K, otherwise the next 16K is filled
	// with the next part of the ROM
	std::copy(PRG.begin(), PRG.end(), this->cartridge.begin() + 0x8000 - 0x6000);
	if(PRG.capacity() == 0x4000) {
		std::copy(this->cartridge.begin() + 0x8000 - 0x6000, this->cartridge.begin() + 0xC000 - 0x6000,
			this->cartridge.begin() + 0xC000 - 0x6000);
	}

	// 1 8K CHR Block, which is loaded into the pattern tables
	std::copy(CHR.begin(), CHR.end(), this->vram.begin());
}

void NROM::setRAM8(uint16_t addr, uint8_t data) {
	if(addr < 0x6000) return;
	cartridge[addr - 0x6000] = data;
}


uint8_t& NROM::getRAM8(uint16_t addr) {
	if(addr < 0x6000) return temp;
	return cartridge[addr - 0x6000];
}

void NROM::setVRAM8(uint16_t addr, uint8_t data) {
	vram[addr] = data;
}

uint8_t NROM::getVRAM8(uint16_t addr) {
	return vram[addr];
}
