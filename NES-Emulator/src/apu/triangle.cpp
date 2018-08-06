#include "stdafx.h"
#include "triangle.h"


const std::array<uint8_t, 32> Triangle::sequence{{15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};

Triangle::Triangle(uint8_t* registerStart) : Channel(registerStart),
linearCounterReg(registerStart[0]), timerLow(registerStart[1]), LCLTH(registerStart[3]) {

}


// Channel Functions
void Triangle::emulateCycle() {
	if(!this->enabled) {
		this->lengthCounter = 0;
		return;
	}

	if(this->timerDividerCounter == 0) {
		++this->sequencePosition %= this->sequence.size();
		this->timerDividerCounter = this->timer();
	} else this->timerDividerCounter--;
}

uint8_t Triangle::generateSample() {
	if(this->lengthCounter == 0 || this->linearCounter == 0) return 0;
	return this->sequence[this->sequencePosition];
}

uint8_t Triangle::getVolume() {
	return this->volume();
}

void Triangle::quarterFrame() {
	if(this->linearCounterReloadFlag) this->linearCounter = this->linearCounterReload();
	else if(this->linearCounter != 0) this->linearCounter--;
	if(!this->linearCounterCTRL()) this->linearCounterReloadFlag = false;
}
