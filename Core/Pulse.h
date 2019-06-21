#pragma once

#include "Channel.h"


class Pulse : public Channel {
public:
	Pulse(uint8_t* registerStart, bool is2);
	
	// Registers
	bool is2;
	uint8_t& DLCV; // Duty cycle, Length counter halt, Constant volume/envelope, Volume/envelope divider period
	uint8_t& sweep;
	uint8_t& timerLow;
	uint8_t& LCLTH; // Length Counter Load, Timer High

	// Get Registers Values
	// DLCV
	uint8_t dutyCycle() { return DLCV >> 6; };
	bool lengthCounterHalt() { return this->DLCV & 0x20; };
	bool envelopeLoop() { return this->lengthCounterHalt(); };
	bool constantVolume() override { return this->DLCV & 0x10; };
	uint8_t volume() override { return this->DLCV & 0xF; };
	// Others are from Channel
	// Sweep
	bool sweepEnabled() { return sweep >> 7; };

	// Useful Variables
	// Duty Cycle
	uint8_t dutyCyclePositon;
	static const std::array<std::array<bool, 8>, 4> dutyCycleSequences;
	uint16_t timerDividerCounter;
	// Envelope
	bool envelopeStartFlag = false;
	uint8_t decayLevelCounter = 0;
	uint8_t envelopeDividerCounter = 0;
	// Sweep
	uint8_t sweepDividerCounter;
	bool sweepReloadFlag;

	// Channel Functions
	void emulateCycle() override;
	uint8_t generateSample() override;
	uint8_t getVolume() override;
	void quarterFrame() override;
	void halfFrame() override;

	// Useful Functions
	uint16_t getTargetPeriod();
	bool sweepMuted();
};

