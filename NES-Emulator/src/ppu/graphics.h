#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>
#include <stdexcept>

namespace Graphics {
	const int width = 256;
	const int height = 240;
	const char* const windowTitle = "NES";
	
	extern std::array<GLubyte, width * height * 3> screenTexPixels;
	extern GLFWwindow* window;
	extern double prevRenerTime;
	extern double framesPassed;

	void renderScreen();
	GLFWwindow* init();
}
