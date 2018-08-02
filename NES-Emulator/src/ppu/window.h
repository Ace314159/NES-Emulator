#pragma once

#include "stdafx.h"

struct Color {
	GLubyte R, G, B;
};

class Window {
private:
	GLuint screenTexID;
	GLuint fboID = 0;
	double prevRenderTime;
	double framesPassed;
public:
	static const int width = 256;
	static const int height = 240;
	static const int windowScale = 3;
	const char* const windowTitle = "NES";
	
	std::array<Color, width * height> screenTexPixels;
	GLFWwindow* window;

	Window();
	void renderScreen();
};

void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
	const GLchar *message, const void *userParam);
