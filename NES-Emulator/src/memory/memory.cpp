#include "stdafx.h"
#include "memory.h"

Memory::Memory(uint16_t& cu, int& scan, int& cyc) : currentVramAddr(cu), scanlineNum(scan), cycleNum(cyc) {}

// CPU
// Main
std::pair<Memory::RAMAddrType, uint8_t*> Memory::getRAMAddrData(uint16_t& addr) {
	if(addr < 0x2000) {
		addr &= 0x07FF;
		return std::make_pair(RAMAddrType::INTERNAL_RAM, &this->internalRAM[addr]);
	} else if(addr < 0x4000) {
		addr &= 0xE007;
		return std::make_pair(RAMAddrType::PPU_REGISTER, &this->ppuRegisters[addr - 0x2000]);
	} else if(addr == 0x4014)
		return std::make_pair(RAMAddrType::PPU_REGISTER, &this->apuRegisters[0x14]);
	else if(addr < 0x4018)
		return std::make_pair(RAMAddrType::APU_REGISTER, &this->apuRegisters[addr - 0x4000]);
	assert(addr >= 0x4020);
	if(addr < 0x8000) return std::make_pair(RAMAddrType::WRAM, &this->WRAM[addr - 0x6000]);

	uint16_t addr2 = addr;
	size_t bank = this->mapper->getPRGBank(addr2); // Offsets addr
	size_t bankSize = this->mapper->getPRGBankSize();
	return std::make_pair(RAMAddrType::CARTRIDGE, &this->PRG[bank * bankSize + addr2]);
}

uint8_t Memory::getRAM8(uint16_t addr) {
	this->mapper->writeCycleDone = false;
	auto addrData = this->getRAMAddrData(addr);
	RAMAddrType addrType = addrData.first;
	uint8_t& addrValue = *addrData.second;

	switch(addrType) {
	case RAMAddrType::INTERNAL_RAM:
		return addrValue;
		break;
	case RAMAddrType::PPU_REGISTER:
		switch(addr) {
		case 0x2002:
			this->ppuRegisterRead = 0x2002;
			return addrValue = (addrValue & ~0x1F) | (this->cpuPpuBus & 0x1F);
			break;
		case 0x2004:
			//this->ppuRegisterRead = 0x2004; // Doesn't do anything
			if(this->cycleNum >= 1 && this->cycleNum <= 65) return 0xFF;
			return this->OAM[this->ppuRegisters[0x3]]; // Gets OAM data at OAMADDR
			break;
		case 0x2007:
			this->ppuRegisterRead = 0x2007;
			if(this->currentVramAddr <= 0x3EFF) return this->ppuDATAReadBuffer;
			else {
				this->ppuDATAReadBuffer = this->getVRAM8(this->currentVramAddr & ~0x1000 & 0x3FFF);
				return this->getVRAM8(this->currentVramAddr & 0x3FFF);
			}
			break;
		default: // PPU Write Only Registers
			return this->cpuPpuBus;
			break;
		}
		break;
	case RAMAddrType::APU_REGISTER:
		if(addr == 0x4016) { // Controller 1
			if(this->buttons1Index < 8) return this->buttons1[this->buttons1Index++];
			else return 0x1;
		}
		this->apuRegisterRead = addr;
		return addrValue;
		break;
	case RAMAddrType::WRAM:
		return addrValue;
		break;
	case RAMAddrType::CARTRIDGE:
		return addrValue;
		break;
	default:
		assert(0);
		return temp;
		break;
	}
}

void Memory::setRAM8(uint16_t addr, uint8_t data) {
	auto addrData = this->getRAMAddrData(addr);
	RAMAddrType addrType = addrData.first;
	uint8_t& addrValue = *addrData.second;

	switch(addrType) {
	case RAMAddrType::INTERNAL_RAM:
		addrValue = data;
		break;
	case RAMAddrType::PPU_REGISTER:
		this->mapper->writeCycleDone = true;
		// || (addr == 0x2000 && this->ppuMem.canWrite)) // Can't write to 0x2000 for first 3000 
		if(addr == 0x2002 || (addr == 0x2004 && this->scanlineNum < 240 && (this->ppuRegisters[1] >> 3) & 0x3))
			return;
		addrValue = this->cpuPpuBus = data;
		this->ppuRegisterWritten = addr;
		// Set low 5 bits of PPUSTATUS - TODO: Find better way
		this->ppuRegisters[0x2] = (this->ppuRegisters[0x2] & ~0x1F) | (this->cpuPpuBus & 0x1F);
		break;
	case RAMAddrType::APU_REGISTER:
		if(addr != 0x4016) this->apuRegisterWritten = addr;
		addrValue = data;
		break;
	case RAMAddrType::WRAM:
		addrValue = data;
		break;
	case RAMAddrType::CARTRIDGE:
		addrValue = data;
		this->mapper->wroteRAM8(addr, data);
		break;
	default:
		assert(0);
		break;
	}
	this->mapper->writeCycleDone = true;
}


// PPU
uint8_t& Memory::getVRAMLoc(uint16_t addr) {
	addr %= 0x4000;
	// Pattern Tables
	if(addr < 0x2000) {
		size_t bank = this->mapper->getCHRBank(addr); // Offsets addr
		size_t bankSize = this->mapper->getCHRBankSize();
		return this->CHR[bank * bankSize + addr];
	}

	// Nametables
	if(addr >= 0x3000 && addr < 0x3F00) addr &= ~0x1000;
	if(addr < 0x3000) return *this->mapper->nametablePtrs[addr - 0x2000];

	// Palette
	if(addr < 0x4000) addr &= 0x3F1F;
	if(addr >= 0x3F10 && addr <= 0x3F1C && addr % 4 == 0) addr &= 0xFF0F;
	assert(addr < 0x4000);
	return this->mapper->palette[addr - 0x3F00];
}

uint8_t Memory::getVRAM8(uint16_t addr) {
	return this->getVRAMLoc(addr);
}

void Memory::setVRAM8(uint16_t addr, uint8_t data) {
	this->getVRAMLoc(addr) = data;
}
