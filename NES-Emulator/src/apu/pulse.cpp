#include "stdafx.h"
#include "pulse.h"

const std::array<std::array<bool, 8>, 4> Pulse::dutyCycleSequences = {{ {{0, 1, 0, 0, 0, 0, 0, 0}},
{{0, 1, 1, 0, 0, 0, 0, 0}}, {{0, 1, 1, 1, 1, 0, 0, 0}}, {{1, 0, 0, 1, 1, 1, 1, 1}} }};

Pulse::Pulse(uint8_t* registerStart, uint16_t& registerWritten, Audio &audio) : DLCV(registerStart[0]),
sweep(registerStart[1]), timerLow(registerStart[2]), LCLTH(registerStart[3]), registerWritten(registerWritten),
Channel(audio) {

}

Sint16 Pulse::generateSample() {
	if(this->timer() < 8) return 0;
	return 500 * this->dutyCycleSequences[this->dutyCycle()][this->dutyCyclePositon];
}

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

