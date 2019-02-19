#include "stdafx.h"
#include "memory.h"
#include "cpu/cpu.h"
#include "ppu/ppu.h"
#include "apu/apu.h"


Memory::Memory(CPU& cpu, PPU& ppu, APU& apu) : cpu(cpu), ppu(ppu), apu(apu) {}

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
			this->cpuPpuBus = addrValue = (addrValue & ~0x1F) | (this->cpuPpuBus & 0x1F);
			break;
		case 0x2004:
			//this->ppuRegisterRead = 0x2004; // Doesn't do anything
			this->cpuPpuBus = this->OAM[this->ppuRegisters[0x3]]; // Gets OAM data at OAMADDR
			break;
		case 0x2007:
			this->ppuRegisterRead = 0x2007;
			if(this->ppu.currentVramAddr <= 0x3EFF) this->cpuPpuBus = this->ppuDATAReadBuffer;
			else this->cpuPpuBus = this->getPaletteLoc(this->ppu.currentVramAddr - 0x3F00);
			break;
		default:
			break;
		}
		return this->cpuPpuBus;
		break;
	case RAMAddrType::APU_REGISTER:
		if(addr == 0x4016) { // Controller 1
			if(this->buttons1Index < 8) return this->buttons1[this->buttons1Index++];
			else return 0x1;
		}
		switch(addr) {
		case 0x4015:
			this->apuRegisterRead = 0x4015;
			return (this->IRQCalled << 6) | ((this->apu.noise.lengthCounter > 0) << 3) |
				((this->apu.triangle.lengthCounter > 0) << 2) | ((this->apu.pulse2.lengthCounter > 0) << 1) |
				((this->apu.pulse1.lengthCounter > 0) << 0);
			break;
		}
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
		this->cpuPpuBus = data;
		if(addr == 0x2002 || (addr == 0x2004 && this->ppu.scanlineNum < 240 && (this->ppuRegisters[1] >> 3) & 0x3))
			return;
		addrValue = this->cpuPpuBus;
		this->ppuRegisterWritten = addr;
		// Set low 5 bits of PPUSTATUS - TODO: Find better way
		this->ppuRegisters[0x2] = (this->ppuRegisters[0x2] & ~0x1F) | (this->cpuPpuBus & 0x1F);
		break;
	case RAMAddrType::APU_REGISTER:
		if(addr != 0x4016) this->apuRegisterWritten = addr;
		switch(addr) {
		case 0x4015:
			addrValue = (addrValue & ~0x1F) | (data & 0x1F);
			break;
		case 0x4017:
			this->apu.newFrameCounter = data;
			break;
		default:
			addrValue = data;
			break;
		}
		break;
	case RAMAddrType::WRAM:
		addrValue = data;
		break;
	case RAMAddrType::CARTRIDGE:
		assert(addr >= 0x8000);
		if(this->mapper->canWriteRAM8(addr)) addrValue = data;
		this->mapper->wroteRAM8(addr, data);
		break;
	default:
		assert(0);
		break;
	}
}


// PPU
uint8_t& Memory::getNametableLoc(uint16_t offsettedAddr) {
	return *this->mapper->nametablePtrs[offsettedAddr];
}

uint8_t& Memory::getCHRLoc(uint16_t addr) {
	size_t bank = this->mapper->getCHRBank(addr); // Offsets addr
	size_t bankSize = this->mapper->getCHRBankSize();
	return this->CHR[bank * bankSize + addr];
}

uint8_t& Memory::getPaletteLoc(uint16_t offsettedAddr) {
	if(offsettedAddr < 0x4000 - 0x3F00) offsettedAddr &= 0x3F1F - 0x3F00;
	if(offsettedAddr >= 0x3F10 - 0x3F00 && offsettedAddr <= 0x3F1C - 0x3F00 && offsettedAddr % 4 == 0)
		offsettedAddr &= 0xFF0F - 0x3F00;
	return this->mapper->palette[offsettedAddr];
}

uint8_t& Memory::getVRAMLoc(uint16_t addr) {
	addr %= 0x4000;
	// Pattern Tables
	if(addr < 0x2000) return this->getCHRLoc(addr);

	// Nametables
	if(addr >= 0x3000 && addr < 0x3F00) addr &= ~0x1000;
	if(addr < 0x3000) return this->getNametableLoc(addr - 0x2000);

	// Palette
	assert(addr < 0x4000);
	return this->getPaletteLoc(addr - 0x3F00);
}

uint8_t Memory::getVRAM8(uint16_t addr) {
	return this->getVRAMLoc(addr);
}

void Memory::setVRAM8(uint16_t addr, uint8_t data) {
	this->getVRAMLoc(addr) = data;
}
