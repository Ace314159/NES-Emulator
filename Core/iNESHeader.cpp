#include "stdafx.h"
#include "iNESHeader.h"


void iNESHeader::init(const std::array<uint8_t, 16>& headerData) {
	// Move everything from headerData into struct
	const uint8_t* NES = &headerData[0];
	this->prgRomSize = headerData[4];
	this->chrRomSize = headerData[5];
	this->flag6 = headerData[6];
	this->flag7 = headerData[7];
	this->prgRamSize = headerData[8];
	this->flag9 = headerData[9];
	this->flag10 = headerData[10];
	const uint8_t* z = &headerData[11];
	// Verify if ROM is valid
	if(!!memcmp(NES, "\x4E\x45\x53\x1A", 4) || !!memcmp(z, "\x0\x0\x0\x0\x0", 5)) {
#ifdef _DEBUG
		std::cout << "NES is " << this->NES << std::endl;
		std::cout << "0 is ";
		for(int i = 0; i < 5; i++) std::cout << (int)this->z[i] << " ";
		std::cout << std::endl;
#endif
		throw std::runtime_error("Corrupted ROM!");
	}

	bool fourScreenVRAM = this->flag6 & 0x08;
	bool isVerticalMirroring = this->flag6 & 0x01;
	if(fourScreenVRAM) this->nametableMirroringType = 4;
	else if(isVerticalMirroring) this->nametableMirroringType = 1;
	else this->nametableMirroringType = 0;

	this->containsSRAM = this->flag6 & 0x02;
	this->containsTrainer = this->flag6 & 0x04;
	this->mapperID = (this->flag7 & ~0x0F) | (this->flag6 >> 4);
	this->isVS = this->flag7 & 0x1;
	this->isPC10 = this->flag7 & 0x2;
	this->isNES2 = this->flag7 & 0xC;

#ifdef _DEBUG
	if(this->isNES2) cout << "Is NES 2.0 ROM";
#endif
}