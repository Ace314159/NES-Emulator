#pragma once

#include "stdafx.h"


class Audio {
private:
	SDL_AudioSpec desiredSpec, obtainedSpec;
public:
	Audio();
	~Audio();
	
	SDL_AudioDeviceID device;
	static constexpr int sampleRate = 48000;
};
