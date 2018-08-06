#pragma once

#include "channel.h"


class Noise : public Channel {
public:
	Noise(uint8_t* registerStart);

	// Registers
	uint8_t& LCV; // Length Counter Halt, Constant Volume/Envelope Flag, Volume/Envelope Divider Period
	// Unused
	uint8_t& modePeriod;
	uint8_t& LCL; // Length Counter Load

	// Get Register Values
	// LCVV
	bool lengthCounterHalt() { return (this->LCV >> 5) & 0x1; };
	bool envelopeLoop() { return this->lengthCounterHalt(); };
	bool constantVolume() override { return (this->LCV >> 4) & 0x1; };
	uint8_t volume() override { return this->LCV & 0xF; };
	// modePeriod
	bool mode() { return modePeriod >> 7; };
	uint16_t timer() override { return this->modePeriod & 0xF; };

	// Useful Variables
	// Envelope
	bool envelopeStartFlag = false;
	uint8_t decayLevelCounter = 0;
	uint8_t envelopeDividerCounter = 0;
	// Timer
	static const std::array<uint16_t, 0x10> timerPeriods;
	uint16_t timerCounter;
	// Linear-Feedback Shift Register
	uint16_t LFSR = 0x1;

	// Channel Functions
	void emulateCycle() override;
	uint8_t generateSample() override;
	uint8_t getVolume() override;
	void quarterFrame() override;
};

