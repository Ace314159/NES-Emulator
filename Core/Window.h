#pragma once

#include "stdafx.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct Color {
	GLubyte R, G, B;
};

inline std::ostream& operator<<(std::ostream& os, const Color& color) {
	return os << color.R << color.G << color.B;
}

inline std::istream& operator>>(std::istream& is, Color& color) {
	return is >> color.R >> color.G >> color.B;
}

inline bool operator==(const Color& c1, const Color& c2) {
	return c1.R == c2.R && c1.G == c2.G && c1.B == c2.B;
}

class Window {
private:
	GLuint screenTexID;
	GLuint fboID = 0;
	std::chrono::high_resolution_clock::time_point prevUpdateTitle;
	double framesPassed;
	std::chrono::high_resolution_clock::time_point prevFrameTime;
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
