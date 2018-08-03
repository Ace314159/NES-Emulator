#pragma once

#include "channel.h"


class Pulse : public Channel {
public:
	Pulse(uint8_t* registerStart, Audio& audio, bool is2);
	
	// Registers
	bool is2;
	uint8_t& DLCV; // Duty cycle, Length counter halt, Constant volume/envelope, Volume/envelope divider period
	uint8_t& sweep;
	uint8_t& timerLow;
	uint8_t& LCLTH; // Length Counter Load, Timer High

	// Get Registers Values
	// DLCV
	uint8_t dutyCycle() { return DLCV >> 6; };
	// Others are from Channel
	// LCLTH
	uint8_t lengthCounterLoad() { return LCLTH >> 3; };
	uint8_t timerHigh() { return LCLTH & 0x7; };
	uint16_t timer() { return timerLow | (timerHigh() << 8); };
	// Sweep
	bool sweepEnabled() { return sweep >> 7; };

	// Useful Variables
	// Duty Cycle
	uint8_t dutyCyclePositon;
	static const std::array<std::array<bool, 8>, 4> dutyCycleSequences;
	uint16_t timerDividerCounter;
	// Sweep
	uint8_t sweepDividerCounter;
	bool sweepReloadFlag;

	// Channel Functions
	Sint16 generateSample() override;
	void emulateCycle() override;
	void halfFrame() override;

	// Useful Functions
	uint16_t getTargetPeriod();
	bool sweepMuted();
};

