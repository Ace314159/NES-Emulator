#pragma once

#include "channel.h"


class Pulse : public Channel {
public:
	Pulse(uint8_t* registerStart, uint16_t& registerWritten, Audio& audio);
	
	// Registers
	uint8_t& DLCV; // Duty cycle, Length counter halt, Constant volume/envelope, Volume/envelope divider period
	uint8_t& sweep;
	uint8_t& timerLow;
	uint8_t& LCLTH; // Length Counter Load, Timer High

	// Get Registers Values
	// DLCV
	uint8_t dutyCycle() { return DLCV >> 6; };
	bool lengthCounterHalt() { return (DLCV >> 5) & 0x1; };
	bool constantVolumeEnvelope() { return (DLCV >> 4) & 0x1; };
	uint8_t volumeEnvelopePeriod() { return DLCV & 0xF; };
	// LCLTH
	uint8_t lengthCounterLoad() { return LCLTH >> 3; };
	uint8_t timerHigh() { return LCLTH & 0x7; };

	// Useful Variables
	uint8_t dutyCyclePositon;
	static const std::array<std::array<bool, 8>, 4> dutyCycleSequences;
	uint16_t& registerWritten;
	uint16_t timerDividerCounter;

	// Useful Functions
	uint16_t timer() { return timerLow | (timerHigh() << 8); };

	// Channel Functions
	Sint16 generateSample() override;
	void emulateCycle() override;
};

