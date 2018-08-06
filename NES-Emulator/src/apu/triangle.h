#pragma once

#include "channel.h"


class Triangle : public Channel {
public:
	Triangle(uint8_t* registerStart);

	// Registers
	uint8_t& linearCounterReg; // Length Counter Halt/Linear Counter Control, Linear Counter Reload
	// Unused
	uint8_t& timerLow;
	uint8_t& LCLTH; // Length Counter Load, Timer High

	// Get Register Values
	// Linear Counter
	bool linearCounterCTRL() { return linearCounterReg >> 7; };
	uint8_t linearCounterReload() { return linearCounterReg & 0x7F; };
	bool lengthCounterHalt() override { return this->linearCounterCTRL(); };
	// Volume
	bool constantVolume() override { return true; }
	uint8_t volume() override { return 1; };

	// Useful Variables
	// Linear Counter
	bool linearCounterReloadFlag;
	uint8_t linearCounter;
	// Sequencer
	uint16_t timerDividerCounter;
	uint8_t sequencePosition;
	static const std::array<uint8_t, 32> sequence;

	// Channel Functions
	void emulateCycle() override;
	uint8_t generateSample() override;
	uint8_t getVolume() override;
	void quarterFrame() override;
};

