#pragma once

#include "memory/memory.h"
#include "pulse.h"


class APU {
public:
	APU(Memory& m);

	Memory& mem;
	Audio audio{};

	// Registers
	uint8_t& status = mem.apuRegisters[0x15];

	// Useful Variables
	bool evenCycle = true;
	uint16_t& registerRead = mem.apuRegisterRead;
	uint16_t& registerWritten = mem.apuRegisterWritten;

	// Channels
	Pulse pulse1{mem.apuRegisters.data() + 0, registerWritten, audio};

	void emulateCycle();

	// Useful Functions
	void handleRegisterReads();
	void handleRegisterWrites();
	void generateSamples(Sint16* buffer, int length);
};

