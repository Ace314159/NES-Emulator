#include "stdafx.h"
#include "channel.h"

Channel::Channel(Audio& audio) : audio(audio), cycylesPerSample(39375000.0 / 11 * 6 / 12 / audio.sampleRate) {};

void Channel::queueAudio() {
	if(this->cycleCount++ > this->cycylesPerSample) {
		Sint16 sample = this->generateSample();
		SDL_QueueAudio(this->audio.device, &sample, sizeof(sample));
		this->cycleCount = 0;
	}
}
