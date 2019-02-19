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

	Memory mem{cpu, ppu, apu};
	CPU cpu{mem};
	PPU ppu{mem};
	APU apu{mem};
};

