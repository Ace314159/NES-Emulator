#pragma once

#include "memory/memory.h"
#include "pulse.h"
#include "triangle.h"
#include "noise.h"


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
	uint16_t& registerRead = mem.apuRegisterRead;
	uint16_t& registerWritten = mem.apuRegisterWritten;
	// Frame Counter
	bool resetFrameCounter = false;
	uint8_t newFrameCounter = 0;
	unsigned int frameCounterCycle = 5;

	// Channels
	Pulse pulse1{mem.apuRegisters.data() + 0x0, false};
	Pulse pulse2{mem.apuRegisters.data() + 0x4, true};
	Triangle triangle{mem.apuRegisters.data() + 0x8};
	Noise noise{mem.apuRegisters.data() + 0xC};

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
	// double prevFactorTime = -1;
	// double factor = 1;
	// double latencySum = 0;
	// int latencyCount = 0;
	double sampleSum = 0;
	double cycleCount = 0;
	int numSamples = 0;
	double cyclesPerSample = NTSC_FREQ / Audio::sampleRate;
	// Functions
	void fillLookupTables();
	double generateSample();
	void queueAudio();
};
