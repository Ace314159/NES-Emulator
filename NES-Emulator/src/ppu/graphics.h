#pragma once

#include "stdafx.h"


namespace Graphics {
	struct Color {
		GLubyte R, G, B;
	};

	const int width = 256;
	const int height = 240;
	const int windowScale = 3;
	const char* const windowTitle = "NES";
	
	extern std::array<Color, width * height> screenTexPixels;
	extern GLFWwindow* window;
	extern double prevRenerTime;
	extern double framesPassed;

	void renderScreen();
	GLFWwindow* init();
	void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
		const GLchar *message, const void *userParam);
}
