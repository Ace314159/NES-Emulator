#pragma once

#include "CPU.h"
#include "PPU.h"
#include "APU.h"


class NES {
public:
	NES(const std::string& romFileName);
	~NES();

	// Important Functions
	void loadRom(const std::string& romFileName);
	void handleInput();
	void tick();
	bool isRunning();

	// Main
	Memory mem{cpu, ppu, apu};
	CPU cpu{mem};
	PPU ppu{mem};
	APU apu{mem};
};

