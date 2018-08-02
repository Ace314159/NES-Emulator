#include "stdafx.h"
#include "window.h"
#include "memory/memory.h"

using std::cout;
using std::endl;

Window::Window() {
	double prevRenderTime = glfwGetTime();
	double framesPassed = 1;

	// Init GLFW
#ifdef _DEBUG
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	// Setup Window
	window = glfwCreateWindow(width * windowScale, height * windowScale, windowTitle, nullptr, nullptr);
	if(window == nullptr) {
		glfwTerminate();
		throw std::runtime_error("Failed to create window!");
	}
	glfwMakeContextCurrent(window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw std::runtime_error("Failed to initialize GLAD");
	}
#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugCallback, nullptr);
	glfwSetErrorCallback([](int errorCode, const char* desc) {
		char error[255];
		sprintf_s(error, "GLFW Error %d: %s\n", errorCode, desc);
		throw std::runtime_error(error);
	});
#endif
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);

	// Screen Texture Setup
	glGenTextures(1, &screenTexID);
	glBindTexture(GL_TEXTURE_2D, screenTexID);
	float colorBlack[] = {0, 0, 0};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, colorBlack);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	// Frame Buffer Setup
	glGenFramebuffers(1, &fboID);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboID);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexID, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void Window::renderScreen() {
	// Attaches Texture Data to Texture
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, &screenTexPixels);

	// Blit texture onto framebuffer
	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboID);

	GLint texX = 0, texY = 0;
	if(windowWidth * height > windowHeight * width) {
		int scaledWidth = static_cast<int>(static_cast<double>(width) / height * windowHeight);
		texX = (windowWidth - scaledWidth) / 2;
		texY = 0;
	} else if(windowWidth * height < windowHeight * width) {
		int scaledHeight = static_cast<int>(static_cast<double>(height) / width * windowWidth);
		texX = 0;
		texY = (windowHeight - scaledHeight) / 2;
	}
	glBlitFramebuffer(0, 0, width, height, texX, texY, windowWidth - texX, windowHeight - texY,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	// Display to screen
	glfwSwapBuffers(window);
	glfwPollEvents();
	if(glfwGetTime() - prevRenderTime > 1.0) {
		char buf[14];
		int fps = (int)((framesPassed / (glfwGetTime() - prevRenderTime)) + 0.5);
		snprintf(buf, sizeof(buf), "%s - %3d FPS", windowTitle, fps);
		glfwSetWindowTitle(window, buf);
		framesPassed = 1;
		prevRenderTime = glfwGetTime();
	} else {
		framesPassed++;
	}
}

void APIENTRY glDebugCallback(GLenum source, GLenum mType, GLuint id, GLenum severity, GLsizei length,
	const GLchar *message, const void *userParam) {
	std::string src;
	switch(source) {
	case GL_DEBUG_SOURCE_API:
		src = "Windows";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		src = "Shader Compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		src = "Third Party";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		src = "Application";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		src = "Other";
		break;
	}

	std::string type;
	switch(mType) {
	case GL_DEBUG_TYPE_ERROR:
		type = "Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		type = "Deprecated Behavior";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		type = "Undefined Behavior";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		type = "Portability Error";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		type = "Performance Error";
		break;
	case GL_DEBUG_TYPE_OTHER:
		type = "Other Error";
		break;
	}

	switch(severity) {
	case GL_DEBUG_SEVERITY_LOW:
		cout << "GL LOW - " + src + " " + type + ": " + message << endl;
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		cout << "GL MEDIUM - " + src + " " + type + ": " + message << endl;
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		throw std::runtime_error("GL HIGH - " + src + " " + type + ": " + message);
		break;
	}
}
