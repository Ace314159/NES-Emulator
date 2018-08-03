#include "stdafx.h"
#include "apu.h"


APU::APU(Memory& m) : mem(m) {}

void APU::emulateCycle() {
	if(this->registerRead) this->handleRegisterReads();
	else if(this->registerWritten) this->handleRegisterWrites();

	if(this->evenCycle) this->pulse1.emulateCycle();
	this->pulse1.queueAudio();

	this->evenCycle = !this->evenCycle;
}

void APU::handleRegisterReads() {
	this->registerRead = 0;
}

void APU::handleRegisterWrites() {
	switch(this->registerWritten) {
	case 0x4003:
		this->pulse1.dutyCyclePositon = 0;
		break;
	case 0x4015:
		this->pulse1.enabled = this->status & 0x1;
		break;
	}
	this->registerWritten = 0;
}


}
