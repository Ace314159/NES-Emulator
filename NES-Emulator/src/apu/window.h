#pragma once

#include "stdafx.h"


struct Color {
	uint8_t R, G, B;
};

struct ControllerInput {
	std::array<bool, 8> buttons1;
	// Player 1
	bool& P1_A = buttons1[0];
	bool& P1_B = buttons1[1];
	bool& P1_SELECT = buttons1[2];
	bool& P1_START = buttons1[3];
	bool& P1_UP = buttons1[4];
	bool& P1_DOWN = buttons1[5];
	bool& P1_LEFT = buttons1[6];
	bool& P1_RIGHT = buttons1[7];
	// Player 2
	std::array<bool, 8> buttons2;
	bool& P2_A = buttons2[0];
	bool& P2_B = buttons2[1];
	bool& P2_SELECT = buttons2[2];
	bool& P2_START = buttons2[3];
	bool& P2_UP = buttons2[4];
	bool& P2_DOWN = buttons2[5];
	bool& P2_LEFT = buttons2[6];
	bool& P2_RIGHT = buttons2[7];
};

class Window {
private:
	const int windowScale = 3;

	SDL_Window* window = nullptr;
	SDL_Surface* windowSurface = nullptr;
	SDL_Renderer* renderer = nullptr;

	uint32_t prevFrameTick;
	double framesPassed;
public:
	static const int width = 256;
	static const int height = 240;
	const char* const windowTitle = "NES";

	SDL_Texture* screenTex = nullptr;
	std::array<uint8_t, width * height * 3> screenTexPixels{};

	bool shouldQuit = false;

	Window();
	~Window();
	void renderFrame();
	ControllerInput pollEvents();
};
