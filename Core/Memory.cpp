#include "stdafx.h"

#include "Memory.h"
#include "CPU.h"
#include "PPU.h"
#include "APU.h"


Memory::Memory(CPU& cpu, PPU& ppu, APU& apu) : cpu(cpu), ppu(ppu), apu(apu) {}

void Memory::setMemoryHandlers() {
	this->mapper->setRAMHandlers(0x0000, 0x1FFF, this->internalRAMHandler);
	this->mapper->setRAMHandlers(0x2000, 0x3FFF, this->ppu);
	this->mapper->setRAMHandlers(0x4000, 0x4017, this->apu);
	this->mapper->setRAMHandlers(0x4014, 0x4014, this->ppu);
	this->mapper->setRAMHandlers(0x4016, 0x4016, this->controllerHandler);
	this->mapper->setRAMHandlers(0x4018, 0x401F, this->blankHandler);
	this->mapper->setRAMHandlers(0x4020, 0xFFFF, *this->mapper);
}

// CPU
// Main
uint8_t Memory::getRAM8(uint16_t addr) {
	return this->mapper->RAMHandlers[addr]->read(addr);
}

void Memory::setRAM8(uint16_t addr, uint8_t data) {
	return this->mapper->RAMHandlers[addr]->write(addr, data);
}

// PPU
uint8_t Memory::getVRAM8(uint16_t addr) {
	return *this->mapper->VRAMPtrs[addr].data;
}

void Memory::setVRAM8(uint16_t addr, uint8_t data) {
	*this->mapper->VRAMPtrs[addr].data = data;
}
