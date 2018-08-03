#include "stdafx.h"
#include "channel.h"

Channel::Channel(uint8_t* registerStart) : registerStart(registerStart) {};

const std::array<uint8_t, 0x20> Channel::lengthCounterTable{{10, 254, 20, 2, 40, 4, 80, 6, 160, 8,
60, 10, 14, 12, 26, 14, 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30}};


// Common Functions
void Channel::loadLengthCounter() {
	if(this->enabled) this->lengthCounter = this->lengthCounterTable[this->getLengthCounter()];
}

uint8_t Channel::getVolume() {
	if(constantVolume()) return volume();
	else return this->decayLevelCounter;
}

void Channel::quarterFrame() {
	if(this->envelopeStartFlag) {
		this->envelopeStartFlag = false;
		this->decayLevelCounter = 15;
		this->envelopeDividerCounter = this->volume();
	} else {
		if(this->envelopeDividerCounter == 0) {
			this->envelopeDividerCounter = this->volume();
			if(this->decayLevelCounter != 0) this->decayLevelCounter--;
			else if(this->envelopeLoop()) this->decayLevelCounter = 15;
		} else this->envelopeDividerCounter--;
	}
}

void Channel::halfFrame() {
	if(this->lengthCounter != 0 && !this->haltLengthCounter()) this->lengthCounter--;
}
