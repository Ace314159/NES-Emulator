#pragma once

#include "audio.h"

class Channel {
protected:
	Channel(uint8_t* registerStart, Audio& audio);
	Audio& audio;
	uint8_t* registerStart;

	int cycleCount = 0;
	int sampleSum = 0;
	const double cylesPerSample;
	const int globalVolumeFactor = 50;
public:
	// Useful Functions
	void queueAudio();
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
	virtual Sint16 generateSample() = 0;
	virtual void emulateCycle() = 0;

	// Useful Variables
	bool enabled = false;
	uint8_t lengthCounter = 0;
	static const std::array<uint8_t, 0x20> lengthCounterTable;
};

