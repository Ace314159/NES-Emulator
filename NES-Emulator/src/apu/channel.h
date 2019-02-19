#pragma once

#include "audio.h"

class Channel {
private:
	uint8_t* registerStart;
protected:
	Channel(uint8_t* registerStart);
public:
	// Register Functions
	uint8_t lengthCounterReload() { return registerStart[3] >> 3; };
	uint8_t timerHigh() { return registerStart[3] & 0x7; };
	virtual uint16_t timer() { return registerStart[2] | (timerHigh() << 8); };
	virtual uint8_t volume() = 0;
	virtual bool lengthCounterHalt() = 0;
	virtual bool constantVolume() = 0;
	
	// Common Functions
	void loadLengthCounter();
	virtual uint8_t getVolume() = 0;
	virtual void quarterFrame() = 0;
	virtual void halfFrame();

	// Functions all channels should implement
	virtual void emulateCycle() = 0;
	virtual uint8_t generateSample() = 0;

	// Useful Variables
	bool lengthCounterHaltVal = false;
	bool dontChangeLengthCounter = false;
	bool enabled = false;
	uint8_t lengthCounter = 0;
	static const std::array<uint8_t, 0x20> lengthCounterTable;
};

