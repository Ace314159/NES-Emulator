#pragma once

#include "CPU.h"
#include "PPU.h"
#include "APU.h"


class NES {
public:
	NES(const fs::path& romFilePath);
	~NES();

	// Important Functions
	void loadRom(const fs::path& romFilePath);
	void handleInput();
	void tick();
	bool isRunning();

	// Main
	Memory mem{cpu, ppu, apu};
	CPU cpu{mem};
	PPU ppu{mem};
	APU apu{mem};
};

