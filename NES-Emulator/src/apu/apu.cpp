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

void APU::generateSamples(Sint16* buffer, int length) {
	for(int i = 0; i < length; i++) {
		buffer[i] = this->pulse1.generateSample();
	}
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

void audioCallback(void *context, Uint8 *raw_buffer, int bytes) {
	APU* apu = reinterpret_cast<APU*>(context);
	Sint16 *buffer = reinterpret_cast<Sint16*>(raw_buffer);
	int length = bytes / sizeof(Sint16);

	apu->generateSamples(buffer, length);
}
