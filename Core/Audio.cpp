#include "stdafx.h"
#include "Audio.h"


Audio::Audio() {
	if(SDL_Init(SDL_INIT_AUDIO) < 0) {
		throw std::runtime_error("Could not initialize SDL! Error: " + std::string(SDL_GetError()));
	}

	SDL_zero(this->desiredSpec);
	this->desiredSpec.freq = this->sampleRate;
	this->desiredSpec.format = AUDIO_S16SYS;
	this->desiredSpec.channels = 1;
	this->desiredSpec.samples = 1024;
	this->desiredSpec.callback = NULL;
	this->desiredSpec.userdata = NULL;

	this->device = SDL_OpenAudioDevice(NULL, 0, &this->desiredSpec, &this->obtainedSpec, NULL);
	if(this->device == 0) {
		throw std::runtime_error("Could not open SDL audio device! Error: " + std::string(SDL_GetError()));
	}
	SDL_PauseAudioDevice(this->device, 0);
}

Audio::~Audio() {
	SDL_CloseAudioDevice(this->device);
}

