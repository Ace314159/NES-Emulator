#include "stdafx.h"
#include "pulse.h"

const std::array<std::array<bool, 8>, 4> Pulse::dutyCycleSequences{{ {{0, 1, 0, 0, 0, 0, 0, 0}},
{{0, 1, 1, 0, 0, 0, 0, 0}}, {{0, 1, 1, 1, 1, 0, 0, 0}}, {{1, 0, 0, 1, 1, 1, 1, 1}} }};

Pulse::Pulse(uint8_t* registerStart, bool is2) : Channel(registerStart), is2(is2),
DLCV(registerStart[0]), sweep(registerStart[1]), timerLow(registerStart[2]), LCLTH(registerStart[3]) {

}


// Channel Functions
void Pulse::emulateCycle() {
	if(!this->enabled) {
		this->lengthCounter = 0;
		return;
	}

	if(this->timerDividerCounter == 0) {
		++this->dutyCyclePositon %= 8;
		this->timerDividerCounter = this->timer();
	} else this->timerDividerCounter--;
}

uint8_t Pulse::generateSample() {
	if(this->sweepMuted() || this->lengthCounter == 0) return 0;
	return this->getVolume() * this->dutyCycleSequences[this->dutyCycle()][this->dutyCyclePositon];
}

void Pulse::halfFrame() {
	// Length Counter
	Channel::halfFrame();
	// Sweep
	//cout << "ENABLED " << (int)(this->sweep) << endl;
	if(this->sweepDividerCounter == 0 && this->sweepEnabled() && !this->sweepMuted()) {
		uint16_t targetPeriod = this->getTargetPeriod();
		this->timerLow = targetPeriod & 0xff;
		this->LCLTH = (this->LCLTH & ~0x7) | (targetPeriod >> 8);
	}
	if(this->sweepDividerCounter == 0 || this->sweepReloadFlag) {
		this->sweepDividerCounter = (this->sweep >> 4) & 0x7;
		this->sweepReloadFlag = false;
	} else this->sweepDividerCounter--;
}


// Useful Functions
uint16_t Pulse::getTargetPeriod() {
	uint16_t targetPeriodChange = this->timer() >> (this->sweep & 0x7);
	if((this->sweep >> 3) & 0x1) targetPeriodChange = ~targetPeriodChange + this->is2;
	return this->timer() + targetPeriodChange;
}

bool Pulse::sweepMuted() {
	return this->getTargetPeriod() > 0x7FF || this->timer() < 8;
}

