#include "graphics.h"
#include "../memory/memory.h"

#include <iostream>

using std::cout;
using std::endl;

namespace Graphics {
	GLuint screenTexID;
	GLuint fboID = 0;

	std::array<GLubyte, width * height * 3> screenTexPixels;
	GLFWwindow* window;
	double prevRenderTime = glfwGetTime();
	double framesPassed = 1;


	void renderScreen() {
		// Attaches Texture Data to Texture
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, &screenTexPixels);

		// Blit texture onto framebuffer
		int windowWidth, windowHeight;
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fboID);
		glBlitFramebuffer(0, 0, width, height, 0, 0, windowWidth, windowHeight, 
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

	GLFWwindow* init() {
		// Init GLFW
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Setup Window
		window = glfwCreateWindow(width * 2, height * 2, windowTitle, nullptr, nullptr);
		if(window == nullptr) {
			glfwTerminate();
			throw std::runtime_error("Failed to create window!");
		}
		glfwMakeContextCurrent(window);

		if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			throw std::runtime_error("Failed to initialize GLAD");
		}

		glfwSetWindowAspectRatio(window, width, height);
		/*glViewport(0, 0, width, height);
		glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, GLint width, GLint height) {
			glViewport(0, 0, width, height);
		});*/

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);

		// Screen Texture Setup
		glGenTextures(1, &screenTexID);
		glBindTexture(GL_TEXTURE_2D, screenTexID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
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

		return window;
	}
}