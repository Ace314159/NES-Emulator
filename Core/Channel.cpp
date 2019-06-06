#include "stdafx.h"
#include "Channel.h"

Channel::Channel(uint8_t* registerStart) : registerStart(registerStart) {};

const std::array<uint8_t, 0x20> Channel::lengthCounterTable{{10, 254, 20, 2, 40, 4, 80, 6, 160, 8,
60, 10, 14, 12, 26, 14, 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30}};


// Common Functions
void Channel::emulateCycle() {
	this->lengthCounterHaltVal = this->lengthCounterHalt();
	if(!this->enabled) {
		this->lengthCounter = 0;
		return;
	}
}

void Channel::loadLengthCounter() {
	if(this->enabled && !this->dontChangeLengthCounter) this->lengthCounter = this->lengthCounterTable[this->lengthCounterReload()];
}

void Channel::halfFrame() {
	if(this->lengthCounter != 0) this->dontChangeLengthCounter = true;
	if(this->lengthCounter != 0 && !this->lengthCounterHaltVal) this->lengthCounter--;
}
