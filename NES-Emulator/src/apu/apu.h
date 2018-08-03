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
	uint8_t& frameCounter = mem.apuRegisters[0x17];

	// Useful Variables
	bool evenCycle = true;
	unsigned int frameCounterCycle = 0;
	uint16_t& registerRead = mem.apuRegisterRead;
	uint16_t& registerWritten = mem.apuRegisterWritten;

	// Channels
	Pulse pulse1{mem.apuRegisters.data() + 0, audio, false};

	void emulateCycle();

	// Useful Functions
	void handleRegisterReads();
	void handleRegisterWrites();
	void emulateFrameCounter();
	void quarterFrame();
	void halfFrame();
	void changeIRQ();
	void generateSamples(Sint16* buffer, int length);
};

