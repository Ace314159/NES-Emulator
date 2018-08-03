#include "stdafx.h"
#include "apu.h"


APU::APU(Memory& m) : mem(m) {}

void APU::emulateCycle() {
	if(this->registerRead) this->handleRegisterReads();
	else if(this->registerWritten) this->handleRegisterWrites();

	this->emulateFrameCounter();

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
		this->pulse1.envelopeStartFlag = true;
		if(this->pulse1.enabled) this->pulse1.loadLengthCounter();
		break;
	case 0x4015:
		this->pulse1.enabled = this->status & 0x1;
		break;
	}
	this->registerWritten = 0;
}

void APU::emulateFrameCounter() {
	if(this->frameCounter >> 7) {
		switch(this->frameCounterCycle++) {
		case 7457:
			this->quarterFrame();
			break;
		case 14913:
			this->halfFrame();
			break;
		case 22371:
			this->quarterFrame();
			break;
		case 29829:
			break;
		case 37281:
			this->halfFrame();
			break;
		case 37282:
			this->frameCounterCycle = 0;
			break;
		}
	} else {
		switch(this->frameCounterCycle++) {
		case 7457:
			this->quarterFrame();
			break;
		case 14913:
			this->halfFrame();
			break;
		case 22371:
			this->quarterFrame();
			break;
		case 29828:
			this->changeIRQ();
			break;
		case 29829:
			this->halfFrame();
			this->changeIRQ();
			break;
		case 29830:
			this->changeIRQ();
			this->frameCounterCycle = 0;
			break;
		}
	}
}

void APU::quarterFrame() {
	this->pulse1.quarterFrame();
}

void APU::halfFrame() {
	this->quarterFrame();
	this->pulse1.halfFrame();
}

void APU::changeIRQ() {
	this->mem.inIRQ = ~((this->frameCounter >> 6) & 0x1);
}
