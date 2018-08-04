#pragma once

#include "audio.h"

class Channel {
private:
	uint8_t* registerStart;
protected:
	Channel(uint8_t* registerStart);
public:
	// Register Functions
	uint8_t getLengthCounter() { return registerStart[3] >> 3; };
	uint8_t volume() { return this->registerStart[0] & 0xF; };
	bool envelopeLoop() { return (this->registerStart[0] >> 5) & 0x1; };
	bool haltLengthCounter() { return this->envelopeLoop(); };
	bool constantVolume() { return (this->registerStart[0] >> 4) & 0x1; };

	// Frame Counter
	// Quarter - Envelope
	bool envelopeStartFlag = false;
	uint8_t decayLevelCounter = 0;
	uint8_t envelopeDividerCounter = 0;
	
	// Common Functions
	void loadLengthCounter();
	uint8_t getVolume();
	void quarterFrame();
	virtual void halfFrame();

	// Functions all channels should implement
	virtual void emulateCycle() = 0;
	virtual uint8_t generateSample() = 0;

	// Useful Variables
	bool enabled = false;
	uint8_t lengthCounter = 0;
	static const std::array<uint8_t, 0x20> lengthCounterTable;
};

