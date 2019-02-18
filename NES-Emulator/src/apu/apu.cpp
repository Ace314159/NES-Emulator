#include "stdafx.h"
#include "apu.h"


APU::APU(Memory& m) : mem(m) { this->fillLookupTables(); }

void APU::emulateCycle() {
	if(this->registerRead) this->handleRegisterReads();
	else if(this->registerWritten) this->handleRegisterWrites();

	this->emulateFrameCounter();

	if(this->mem.cycleNum % 2 == 0) {
		this->pulse1.emulateCycle();
		this->pulse2.emulateCycle();
		this->noise.emulateCycle();
	}
	this->triangle.emulateCycle();
	this->queueAudio();
}

void APU::handleRegisterReads() {
	this->registerRead = 0;
}

void APU::handleRegisterWrites() {
	switch(this->registerWritten) {
	// Pulse 1
	case 0x4001:
		this->pulse1.sweepReloadFlag = true;
		break;
	case 0x4003:
		this->pulse1.dutyCyclePositon = 0;
		this->pulse1.envelopeStartFlag = true;
		this->pulse1.loadLengthCounter();
		break;
	// Pulse 2
	case 0x4005:
		this->pulse2.sweepReloadFlag = true;
		break;
	case 0x4007:
		this->pulse2.dutyCyclePositon = 0;
		this->pulse2.envelopeStartFlag = true;
		this->pulse2.loadLengthCounter();
		break;
	// Triangle
	case 0x400B:
		this->triangle.linearCounterReloadFlag = true;
		this->triangle.loadLengthCounter();
		break;
	// Noise
	case 0x400F:
		this->noise.envelopeStartFlag = true;
		this->noise.loadLengthCounter();
		break;
	case 0x4015:
		this->pulse1.enabled = (this->status >> 0) & 0x1;
		this->pulse2.enabled = (this->status >> 1) & 0x1;
		this->triangle.enabled = (this->status >> 2) & 0x1;
		this->noise.enabled = (this->status >> 3) & 0x1;
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
	this->pulse2.quarterFrame();
	this->triangle.quarterFrame();
	this->noise.quarterFrame();
}

void APU::halfFrame() {
	this->quarterFrame();
	this->pulse1.halfFrame();
	this->pulse2.halfFrame();
	this->triangle.halfFrame();
	this->noise.halfFrame();
}

void APU::changeIRQ() {
	this->mem.inIRQ = ~((this->frameCounter >> 6) & 0x1);
}


// Audio Mixer
// From http://wiki.nesdev.com/w/index.php/APU_Mixer
void APU::fillLookupTables() {
	for(size_t i = 0; i < this->pulseTable.size(); i++) this->pulseTable[i] = 95.52 / (8128.0 / i + 100);
	for(size_t i = 0; i < this->tndTable.size(); i++) this->tndTable[i] = 163.67 / (24329.0 / i + 100);
}

 double APU::generateSample() {
	 double pulseOut = this->pulseTable[this->pulse1.generateSample() + this->pulse2.generateSample()];
	 double tndOut = this->tndTable[3 * this->triangle.generateSample() +
									2 * this->noise.generateSample()];
	 return pulseOut + tndOut;
}

void APU::queueAudio() {
	this->sampleSum += this->generateSample();
	this->numSamples++;
	this->cycleCount++;
	
	// Account for latency
	/*double curTime = glfwGetTime();
	this->latencySum += (double)SDL_GetQueuedAudioSize(this->audio.device) / sizeof(Sint16) / Audio::sampleRate;
	this->latencyCount++;
	if(curTime - this->prevFactorTime > 1) {
		double latency = this->latencySum / this->latencyCount;
		if(latency > 0.1) this->factor = 1 + 0.5 * (latency - 0.1);
		else this->factor = 1;
		this->prevFactorTime = curTime;
		this->latencySum = 0;
		this->latencyCount = 0;
	}*/

	if(this->cycleCount >= this->cyclesPerSample/* * this->factor*/) {
		Sint16 sample = static_cast<Sint16>(this->globalVolumeFactor * this->sampleSum / this->numSamples);
		SDL_QueueAudio(this->audio.device, &sample, sizeof(sample));
		this->cycleCount -= this->cyclesPerSample/* * this->factor*/;
		this->sampleSum = 0;
		this->numSamples = 0;
	}
}
