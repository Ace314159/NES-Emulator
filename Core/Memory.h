#pragma once

#include "BaseMapper.h"
#include "MemoryHandler.h"
#include "InternalRAMHandler.h"
#include "ControllerHandler.h"
#include "BlankHandler.h"

class CPU;
class PPU;
class APU;

class Memory {
public:
	Memory(CPU& cpu, PPU& ppu, APU& apu);
	std::unique_ptr<BaseMapper> mapper;
	CPU& cpu;
	PPU& ppu;
	APU& apu;

	// Memory Handlers
	InternalRAMHandler internalRAMHandler;
	ControllerHandler controllerHandler;
	BlankHandler blankHandler;
	void setMemoryHandlers();

	// CPU
	// Main
	uint8_t getRAM8(uint16_t addr);
	void setRAM8(uint16_t addr, uint8_t data);

	// NMI
	bool NMICalled = false; // PPU Request

	// PPU
	// Main
	std::array<uint8_t, 0x100> OAM = {}; // Object Attribute Memory
	uint8_t getVRAM8(uint16_t addr);
	void setVRAM8(uint16_t addr, uint8_t data);
};

