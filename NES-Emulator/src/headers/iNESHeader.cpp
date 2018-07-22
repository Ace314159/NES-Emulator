#include "iNESHeader.h"

#include <stdexcept>
#include <iostream>


void iNESHeader::parse() {
	// NES0x1A in ASCII is flipped for endiannes
	if(this->NES != 0x1A53454E || this->_0.z1 != 0 || this->_0.z2 != 0) {
		std::cout << "NES is " << (int)this->NES << std::endl;
		std::cout << "0 is " << (int)this->_0.z1 << (int)this->_0.z2 << std::endl;
		throw std::runtime_error("Corrupted ROM!");
	}

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