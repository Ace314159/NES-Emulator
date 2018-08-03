#include "stdafx.h"
#include "channel.h"

Channel::Channel(Audio& audio) : audio(audio), cycylesPerSample(39375000.0 / 11 * 6 / 12 / audio.sampleRate) {};

void Channel::queueAudio() {
	this->sampleSum += this->generateSample();
	if(this->cycleCount > this->cylesPerSample) {
		Sint16 sample = globalVolumeFactor * this->sampleSum / this->cycleCount;
		SDL_QueueAudio(this->audio.device, &sample, sizeof(sample));
		this->cycleCount = 0;
		this->sampleSum = 0;
	} else this->cycleCount++;
}
void Channel::quarterFrame() {
	}
}
void Channel::halfFrame() {
}
