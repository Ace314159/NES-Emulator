#pragma once

#include "cpu/cpu.h"
#include "ppu/ppu.h"

#include <string>
#include <chrono>


class NES {
private:
	const std::chrono::nanoseconds CLOCK_TIME;
	void corruptedROM();

	// Constants from ROM Header
	uint8_t mapper;
	bool sRAMUsed;
public:
	NES(const std::chrono::nanoseconds ct);
	void loadRom(std::string fileName);
	void handleInput();
	void tick();

	Memory mem = Memory(ppu.currentVramAddr, ppu.scanlineNum, ppu.cycleNum);
	CPU cpu = CPU(mem.cpuMem, ppu.cycleNum, ppu.scanlineNum);
	PPU ppu = PPU(mem.ppuMem);

	unsigned int cycleCount = 0;
};

