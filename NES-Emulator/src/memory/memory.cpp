#include "memory.h"

#include <iostream>

using std::cout;
using std::endl;

Memory::Memory(uint16_t& cu, int& scan, int& cyc) : currentVramAddr(cu), scanlineNum(scan), cycleNum(cyc) { }

// CPU
uint8_t& Memory::getRAMLoc(uint16_t& addr) {
	// Memory Mirroring
	if(addr >= 0x0800 && addr < 0x2000) addr &= 0x07FF; // Internal RAM Mirroring
	if(addr > 0x2007 && addr < 0x4000) addr &= 0xE007; // PPU Register Mirroring
	
	if(addr < 0x0800) return this->mapper->internalRAM[addr];
	if(/*addr >= 0x2000 && */addr <= 0x2007) return this->mapper->ppuRegisters[addr - 0x2000];
	if(/*addr >= 0x4000 && */addr <= 0x4017) return this->mapper->apuRegisters[addr - 0x4000];
	return this->mapper->getRAM8(addr);
}

uint8_t Memory::getRAM8(uint16_t addr) {
	this->mapper->writeCycleDone = false;
	/*if(this->DMACalled) {
		this->inDMA = true;
		this->DMACalled = false;
		throw "DMA";
	}*/
	uint8_t& ramLoc = this->getRAMLoc(addr);
	if(addr >= 0x2000 && addr <= 0x2007) { // PPU Registers
		if(addr == 0x2000 || addr == 0x2001 || addr == 0x2003 || addr == 0x2005 || addr == 0x2006) {
			return this->cpuPpuBus; // PPU Write Only Registers
		} else if(addr == 0x2002) {
			this->ppuRegisterRead = 0x2002;
			return ramLoc = (ramLoc & ~0x1F) | (this->cpuPpuBus & 0x1F);;
		} else if(addr == 0x2007) {
			this->ppuRegisterRead = 0x2007;
			if(this->currentVramAddr <= 0x3EFF) return this->ppuDATAReadBuffer;
			else {
				this->ppuDATAReadBuffer  = this->getVRAM8(this->currentVramAddr & ~0x1000);
				return this->getVRAM8(this->currentVramAddr);
			}
		}
		// Has to be 0x2004
		// this->ppuMem.registerRead = 0x2004; - Nothing happens anyway
		return this->OAM[this->mapper->ppuRegisters[0x3]]; // Gets OAM data at OAMADDR
	}
	else if(addr == 0x4014) return this->cpuPpuBus; // PPU Write Only Register
	else if(addr == 0x4016) return buttons1[buttons1Index++ % 8];
	else if(addr == 0x4017) return buttons2[buttons2Index++ % 8];
	return ramLoc;
}

void Memory::setRAM8(uint16_t addr, uint8_t data) {
	this->mapper->writeCycleDone = true;
	uint8_t& ramLoc = this->getRAMLoc(addr);
	if(addr < 0x0800) { // Internal RAM
		ramLoc = data;
		return;
	} else if(addr >= 0x2000 && addr <= 0x2007 && addr != 0x2002) { // PPU Registers - Read Only Register
		if(addr == 0x2004 && (this->scanlineNum < 240 || this->scanlineNum == 261))
			// || (addr == 0x2000 && this->ppuMem.canWrite)) // Can't write to 0x2000 for first 3000 cycles
			return; // Ignore writes to OAM Data during rendering
		ramLoc = this->cpuPpuBus = data;
		// Set low 5 bits of PPUSTATUS
		mapper->ppuRegisters[0x2] = (mapper->ppuRegisters[0x2] & ~(0x1F)) | (this->cpuPpuBus & 0x1F);

		this->ppuRegisterWritten = addr;
	} else if(addr == 0x4014) { // PPU Write Only Register
		ramLoc = this->cpuPpuBus = data;
		// Set low 5 bits of PPUSTATUS
		//arr[0x2002] = (arr[0x2002] & ~(0x1F)) | (this->ppuMem.cpuBus & 0x1F);
		this->ppuRegisterWritten = 0x4014;
		return;
	} else if(addr >= 0x4020) { // Cartridge RAM
		this->mapper->setRAM8(addr, data); // Run mapper specific code
	} else { // APU and I/O Registers (0x4000 - 0x4017)
		ramLoc = data;
	}
}


// PPU
uint8_t& Memory::getVRAMLoc(uint16_t& addr) {
	// Pattern Tables
	if(addr < 0x2000) return this->mapper->getVRAM8(addr);

	// Nametables
	if(addr >= 0x3000 && addr < 0x3F00) addr &= ~0x1000;
	if(addr < 0x3000) {
		switch(mapper->nametableMirroringType) {
		case BaseMapper::NametableMirroringType::HORIZONTAL:
			addr &= ~0xF400;
			break;
		case BaseMapper::NametableMirroringType::VERTICAL:
			addr &= ~0xF800;
			break;
		case BaseMapper::NametableMirroringType::ONE_A:
			addr &= ~0xFC00;
			break;
		case BaseMapper::NametableMirroringType::ONE_B:
			addr = (addr & ~0xFC00) | 0x0400;
			break;
		}
		return this->mapper->nametables[addr];
	}

	// Palette
	if(addr < 0x4000) addr &= 0x3F1F;
	if(addr >= 0x3F10 && addr <= 0x3F1C && addr % 4 == 0) addr &= 0xFF0F;
	return this->mapper->palette[addr - 0x3F00];
}

uint8_t Memory::getVRAM8(uint16_t addr) {
	return this->getVRAMLoc(addr);
}

void Memory::setVRAM8(uint16_t addr, uint8_t data) {
	if(addr < 0x2000) this->mapper->setVRAM8(addr, data);
	this->getVRAMLoc(addr) = data;
}
