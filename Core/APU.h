#pragma once

#include "Memory.h"
#include "MemoryHandler.h"

#include "Pulse.h"
#include "Triangle.h"
#include "Noise.h"


class APU : public MemoryHandler {
public:
	APU(Memory& m);
	const Sint16 globalVolumeFactor = 5000;

	Memory& mem;
	Audio audio{};

	// Registers
	std::array<uint8_t, 0x0014> registers; // 0x4000 - 0x4013
	uint8_t status = 0; // 0x4015
	uint8_t frameCounter = 0; // 0x4017

	// Frame Counter
	int resetFrameCounterTime = -1;
	uint8_t newFrameCounter = 0;
	// After reset or power-up, APU acts as if $4017 were written with $00 from
	// 9 to 12 clocks before first instruction begins.
	unsigned int frameCounterCycle = 10;

	// Channels
	Pulse pulse1{registers.data() + 0x0, false};
	Pulse pulse2{registers.data() + 0x4, true};
	Triangle triangle{registers.data() + 0x8};
	Noise noise{registers.data() + 0xC};

	void emulateCycle();

	// Useful Functions
	virtual uint8_t read(uint16_t addr) override;
	virtual void write(uint16_t addr, uint8_t data) override;
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
