#include "stdafx.h"
#include "Noise.h"


const std::array<uint16_t, 0x10> Noise::timerPeriods{{ 4, 8, 16, 32, 64, 96, 128, 160,
202, 254, 380, 508, 762, 1016, 2034, 4068 }};

Noise::Noise(uint8_t* registerStart) : Channel(registerStart), LCV(registerStart[0]),
modePeriod(registerStart[2]), LCL(registerStart[3]) {

}


// Channel Functions
void Noise::emulateCycle() {
	Channel::emulateCycle();

	if(this->timerCounter == 0) {
		// Feedback Bit = Bit 1 ^ (Bit 6 if modeFlag is set else Bit 1)
		bool feedbackBit = (this->LFSR & 0x01) ^ ((this->LFSR >> (1 + this->mode() * 5) & 0x01));
		this->LFSR >>= 1;
		// Clear Bits 15 and 14 and set Bit 14 to feedbackBit - LFSR is 15 bits wide
		this->LFSR = (this->LFSR & ~(0x3 << 14)) | (feedbackBit << 14);
		this->timerCounter = this->timerPeriods[this->timer()];
	} else this->timerCounter--;
}

uint8_t Noise::generateSample() {
	if(this->lengthCounter == 0 || (this->LFSR & 0x01)) return 0;
	return this->getVolume();
}

uint8_t Noise::getVolume() {
	if(constantVolume()) return volume();
	else return this->decayLevelCounter;
}

void Noise::quarterFrame() {
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
