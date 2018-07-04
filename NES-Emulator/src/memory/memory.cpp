#include "memory.h"

#include <iostream>

using std::cout;
using std::endl;

Memory::Memory(uint16_t& cu, int& scan, int& cyc) : ppuMem(this->cpuMem, cu, scan), cpuMem(ppuMem, cyc) { }

// CPU
Memory::CPU::CPU(Memory::PPU& p, int& cyc) : ppuMem(p), cycleNum(cyc) {}

uint16_t Memory::CPU::getAddr(uint16_t addr) {
	if(addr > 0x7FFF && addr < 0x2000) return addr & 0x07FF; // Memory Mirroring
	if(addr > 0x1FFF && addr < 0x4000) return addr & 0xE007; // PPU Register Mirroring
	return addr;
}

uint8_t Memory::CPU::get8(uint16_t addr) {
	/*if(this->DMACalled) {
		this->inDMA = true;
		this->DMACalled = false;
		throw "DMA";
	}*/
	addr = getAddr(addr);
	if(addr >= 0x2000 && addr <= 0x2007) {
		addr &= 0xE007;
		if(addr == 0x2000 || addr == 0x2001 || addr == 0x2003 || addr == 0x2005 || addr == 0x2006) {
			return this->ppuMem.cpuBus; // PPU Write Only Registers
		} else if(addr == 0x2002) {
			this->ppuMem.registerRead = 0x2002;
			return arr[0x2002] = (arr[0x2002] & ~0x1F) | (this->ppuMem.cpuBus & 0x1F);
		} else if(addr == 0x2007) {
			this->ppuMem.registerRead = 0x2007;
			if(this->ppuMem.currentVramAddr <= 0x3EFF) return this->ppuMem.DATAReadBuffer;
			else {
				this->ppuMem.DATAReadBuffer  = this->ppuMem.get8(this->ppuMem.currentVramAddr & ~0x1000);
				return this->ppuMem.get8(this->ppuMem.currentVramAddr);
			}
		}
		// Has to be 0x2004
		// this->ppuMem.registerRead = 0x2004; - Nothing happens anyway
		return this->ppuMem.OAM[this->arr[0x2003]]; // Gets OAM data at OAMADDR
	}
	else if(addr == 0x4014) return this->ppuMem.cpuBus; // PPU Write Only Register
	else if(addr == 0x4016) return buttons1[buttons1Index++ % 8];
	else if(addr == 0x4017) return buttons2[buttons2Index++ % 8];
	return arr[addr];
}

void Memory::CPU::set8(uint16_t addr, uint8_t data) {
	addr = getAddr(addr);
	if(addr < 0x07FF) { // Regular Memory
		arr[addr] = data;
		return;
	} else if(addr != 0x2002) { // PPU Write Registers
		if(addr == 0x2004 && (this->ppuMem.scanlineNum < 240 || this->ppuMem.scanlineNum == 261))
			// || (addr == 0x2000 && this->ppuMem.canWrite)) // Can't write to 0x2000 for first 3000 cycles
			return; // Ignore writes to OAM Data during rendering
		arr[addr] = this->ppuMem.cpuBus = data;
		// Set low 5 bits of PPUSTATUS
		arr[0x2002] = (arr[0x2002] & ~(0x1F)) | (this->ppuMem.cpuBus & 0x1F);

		this->ppuMem.registerWritten = addr;
	} else if(addr == 0x4014) { // PPU Write Only Register
		arr[0x4014] = this->ppuMem.cpuBus = data;
		// Set low 5 bits of PPUSTATUS
		//arr[0x2002] = (arr[0x2002] & ~(0x1F)) | (this->ppuMem.cpuBus & 0x1F);
		this->ppuMem.registerWritten = 0x4014;
		return;
	}
	arr[addr] = data;
}


// PPU
Memory::PPU::PPU(Memory::CPU& c, uint16_t& cu, int& scan) : currentVramAddr(cu), scanlineNum(scan), 
NMICalled(c.NMICalled), inNMI(c.inNMI), inDMA(c.inDMA), DMAAddr(c.DMAAddr), 
DMAPage(c.DMAPage), DMAVal(c.DMAVal), inOddCycle(c.inOddCycle), DMAdoneDummy(c.DMAdoneDummy), 
register4014(c.arr[0x4014]) {
	this->registers = &c.arr[0x2000];
}

uint16_t Memory::PPU::getAddr(uint16_t addr) {
	if(addr >= 0x3000 && addr <= 0x3eFF) return addr & ~0x1000; // Memory Mirroring
	if(addr >= 0x3F20 && addr <= 0x3FFF) return addr & 0x3F1F; // Memory Mirroring
	if(addr >= 0x3F10 && addr <= 0x3F1C && addr % 4 == 0) return addr & 0xFF0F; // Palette Mirroring

	if(addr > 0x23FF && addr < 0x3000) { // Nametable Mirroring
		if(this->nametableMirroringType == NametableMirroringType::HORIZONTAL) return addr & ~0x400;
		if(this->nametableMirroringType == NametableMirroringType::VERTICAL) return addr & ~800;
		if(this->nametableMirroringType == NametableMirroringType::ONE) return addr & 0x23FF;
	}

	return addr;
}

uint8_t Memory::PPU::get8(uint16_t addr) {
	return arr[getAddr(addr)];
}

void Memory::PPU::set8(uint16_t addr, uint8_t data) {
	arr[this->getAddr(addr)] = data;
}
