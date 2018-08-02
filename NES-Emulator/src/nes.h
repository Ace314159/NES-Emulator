#pragma once

#include "cpu/cpu.h"
#include "ppu/ppu.h"
#include "apu/apu.h"


class NES {
public:
	NES(std::string romFileName);
	void loadRom(std::string romFileName);
	void handleInput();
	void tick();

	Memory mem{ppu.currentVramAddr, ppu.scanlineNum, ppu.cycleNum};
	CPU cpu{mem, ppu.cycleNum, ppu.scanlineNum};
	PPU ppu{mem};
	APU apu{mem};

	unsigned int cycleCount = 0;
};

