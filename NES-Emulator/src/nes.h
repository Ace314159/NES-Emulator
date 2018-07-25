#pragma once

#include "cpu/cpu.h"
#include "ppu/ppu.h"


class NES {
private:
	const std::chrono::nanoseconds CLOCK_TIME;
public:
	NES(const std::chrono::nanoseconds ct, std::string romFileName);
	Memory& loadRom(std::string romFileName);
	void handleInput();
	void tick();

	Memory mem = Memory(ppu.currentVramAddr, ppu.scanlineNum, ppu.cycleNum);
	CPU cpu = CPU(mem, ppu.cycleNum, ppu.scanlineNum);
	PPU ppu;

	unsigned int cycleCount = 0;
};

