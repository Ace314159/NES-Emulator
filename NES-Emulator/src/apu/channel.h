#pragma once

#include "audio.h"

class Channel {
protected:
	Channel(Audio& audio);
	Audio& audio;
	int cycleCount = 0;
	int sampleSum = 0;
	const double cylesPerSample;
	const int globalVolumeFactor = 50;
public:
	void queueAudio();
	void quarterFrame();
	virtual void halfFrame();

	// Functions all channels should implement
	virtual Sint16 generateSample() = 0;
	virtual void emulateCycle() = 0;

	// Useful Variables
	bool enabled = false;
	uint8_t lengthCounter = 0;
};

