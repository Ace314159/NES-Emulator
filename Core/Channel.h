#pragma once

#include "Audio.h"

class Channel {
private:
	uint8_t* registerStart;
protected:
	Channel(uint8_t* registerStart) : registerStart(registerStart) {};
public:
	// Register Functions
	uint8_t lengthCounterReload() { return registerStart[3] >> 3; };
	uint8_t timerHigh() { return registerStart[3] & 0x7; };
	virtual uint16_t timer() { return registerStart[2] | (timerHigh() << 8); };
	virtual uint8_t volume() = 0;
	virtual bool lengthCounterHalt() = 0;
	virtual bool constantVolume() = 0;
	
	// Common Functions
	void loadLengthCounter() { 
		if(this->enabled) this->lengthCounter = this->lengthCounterTable[this->lengthCounterReload()];
	}
	virtual uint8_t getVolume() = 0;
	virtual void quarterFrame() = 0;
	virtual void halfFrame() {
		if(this->lengthCounter != 0 && !this->lengthCounterHaltVal) this->lengthCounter--;
	};

	// Functions all channels should implement
	virtual void emulateCycle() = 0 {
		if(!this->enabled) {
			this->lengthCounter = 0;
			return;
		}
	};
	virtual uint8_t generateSample() = 0;

	// Useful Variables
	bool lengthCounterHaltVal = false;
	bool enabled = false;
	uint8_t lengthCounter = 0;
	inline static const std::array<uint8_t, 0x20> lengthCounterTable{{10, 254, 20, 2, 40, 4, 80, 6, 160, 8,
	60, 10, 14, 12, 26, 14, 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30}};;
};
