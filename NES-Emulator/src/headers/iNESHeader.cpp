#include "iNESHeader.h"

#include <stdexcept>
#include <iostream>
#include <algorithm>


void iNESHeader::init(std::array<uint8_t, 16>& headerData) {
	// Move everything from headerData into struct
	this->NES = &headerData[0];
	this->prgRomSize = headerData[4];
	this->chrRomSize = headerData[5];
	this->flag6 = headerData[6];
	this->flag7 = headerData[7];
	this->prgRamSize = headerData[8];
	this->flag9 = headerData[9];
	this->flag10 = headerData[10];
	this->z = &headerData[11];
	// Verify if ROM is valid
	if(!!memcmp(this->NES, "\x4E\x45\x53\x1A", 4) || !!memcmp(this->z, "\x0\x0\x0\x0\x0", 5)) {
		std::cout << "NES is " << this->NES << std::endl;
		std::cout << "0 is ";
		for(int i = 0; i < 5; i++) std::cout << (int)this->z[i] << " ";
		std::cout << std::endl;
		throw std::runtime_error("Corrupted ROM!");
	}
	// Clear dangling pointers
	this->NES = nullptr;
	this->z = nullptr;

	bool fourScreenVRAM = (this->flag6 >> 3) & 0x1;
	bool isVerticalMirroring = this->flag6 & 0x1;
	if(fourScreenVRAM) this->nametableMirroringType = 4;
	else if(isVerticalMirroring) this->nametableMirroringType = 1;
	else this->nametableMirroringType = 0;

	this->containsSRAM = (this->flag6 >> 1) & 0x1;
	this->containsTrainer = (this->flag6 >> 2) & 0x1;
	this->mapperID = (this->flag7 & ~0xF) | (this->flag6 >> 4);
	this->isVS = this->flag7 & 0x1;
	this->isPC10 = (this->flag7 >> 1) & 0x1;
	this->isNES2 = ((this->flag7 >> 2) & 0x3) == 2;

	if(this->isNES2) throw std::runtime_error("Does not support NES 2.0");
}