#pragma once

#include "memory/memory.h"
#include "pulse.h"


class APU {
public:
	APU(Memory& m);
	const Sint16 globalVolumeFactor = 5000;

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
	Pulse pulse1{mem.apuRegisters.data() + 0, false};
	Pulse pulse2{mem.apuRegisters.data() + 4, true};

	void emulateCycle();

	// Useful Functions
	void handleRegisterReads();
	void handleRegisterWrites();
	void emulateFrameCounter();
	void quarterFrame();
	void halfFrame();
	void changeIRQ();

	// Audio Mixer
	// Lookup Tables
	std::array<double, 31> pulseTable;
	std::array<double, 203> tndTable;
	// Downsampling Variables
	double sampleSum = 0;
	int cycleCount = 0;
	const int cyclesPerSample = static_cast<int>(NTSC_FREQ / audio.sampleRate);
	//Functions
	void fillLookupTables();
	double generateSample();
	void queueAudio();
};

