#include "memory.h"

#include <iostream>

using std::cout;
using std::endl;

Memory::Memory(uint16_t& cu, int& scan, int& cyc) : currentVramAddr(cu), scanlineNum(scan), cycleNum(cyc) { }

// CPU

uint8_t& Memory::getRAMLoc(uint16_t& addr) {
	// Memory Mirroring
	if(addr >= 0x0800 && addr < 0x2000) addr &= 0x07FF; // RAM Mirroring
	if(addr > 0x2007 && addr < 0x4000) addr &= 0xE007; // PPU Register Mirroring
	
	if(addr < 0x0800) return this->mapper->internalRAM[addr];
	if(addr >= 0x2000 && addr <= 0x2007) return this->mapper->ppuRegisters[addr - 0x2000];
	if(addr >= 0x4000 && addr <= 0x4017) return this->mapper->apuRegisters[addr - 0x4000];
	return this->mapper->getRAM8(addr);
}

uint8_t Memory::getRAM8(uint16_t addr) {
	/*if(this->DMACalled) {
		this->inDMA = true;
		this->DMACalled = false;
		throw "DMA";
	}*/
	uint8_t& ramLoc = this->getRAMLoc(addr);
	if(addr >= 0x2000 && addr <= 0x2007) {
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
		return this->OAM[this->mapper->ppuRegisters[0x2003 - 0x2000]]; // Gets OAM data at OAMADDR
	}
	else if(addr == 0x4014) return this->cpuPpuBus; // PPU Write Only Register
	else if(addr == 0x4016) return buttons1[buttons1Index++ % 8];
	else if(addr == 0x4017) return buttons2[buttons2Index++ % 8];
	return ramLoc;
}

void Memory::setRAM8(uint16_t addr, uint8_t data) {
	uint8_t& ramLoc = this->getRAMLoc(addr);
	if(addr < 0x07FF) { // Regular Memory
		ramLoc = data;
		return;
	} else if(addr != 0x2002) { // PPU Write Registers
		if(addr == 0x2004 && (this->scanlineNum < 240 || this->scanlineNum == 261))
			// || (addr == 0x2000 && this->ppuMem.canWrite)) // Can't write to 0x2000 for first 3000 cycles
			return; // Ignore writes to OAM Data during rendering
		ramLoc = this->cpuPpuBus = data;
		// Set low 5 bits of PPUSTATUS
		mapper->ppuRegisters[0x2002 - 0x2000] = (mapper->ppuRegisters[2] & ~(0x1F)) | (this->cpuPpuBus & 0x1F);

		this->ppuRegisterWritten = addr;
	} else if(addr == 0x4014) { // PPU Write Only Register
		ramLoc = this->cpuPpuBus = data;
		// Set low 5 bits of PPUSTATUS
		//arr[0x2002] = (arr[0x2002] & ~(0x1F)) | (this->ppuMem.cpuBus & 0x1F);
		this->ppuRegisterWritten = 0x4014;
		return;
	}
	ramLoc = data;
}


// PPU
uint16_t Memory::getVRAMAddr(uint16_t addr) {
	if(addr >= 0x3000 && addr <= 0x3EFF) return addr & ~0x1000; // Memory Mirroring
	if(addr >= 0x3F20 && addr <= 0x3FFF) return addr & 0x3F1F; // Memory Mirroring
	if(addr >= 0x3F10 && addr <= 0x3F1C && addr % 4 == 0) return addr & 0xFF0F; // Palette Mirroring

	if(addr >= 0x2400 && addr < 0x3000) { // Nametable Mirroring
		if(this->nametableMirroringType == NametableMirroringType::HORIZONTAL) return addr & ~0x0400;
		if(this->nametableMirroringType == NametableMirroringType::VERTICAL) return addr & ~0x0800;
		if(this->nametableMirroringType == NametableMirroringType::ONE) return addr & 0x23FF;
	}

	return addr;
}

uint8_t Memory::getVRAM8(uint16_t addr) {
	return mapper->getVRAM8(this->getVRAMAddr(addr));
}

void Memory::setVRAM8(uint16_t addr, uint8_t data) {
	mapper->setVRAM8(this->getVRAMAddr(addr), data);
}
