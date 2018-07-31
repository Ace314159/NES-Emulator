#include "stdafx.h"
#include "window.h"
#include "memory/memory.h"


Window::Window() {
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0) {
		throw std::runtime_error("Could not initialize SDL! Error: " + std::string(SDL_GetError()));
	}
	this->window = SDL_CreateWindow(this->windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		this->width * this->windowScale, this->height * this->windowScale, SDL_WINDOW_SHOWN);
	if(this->window == nullptr)
		throw std::runtime_error("Could not create SDL Window! Error: " + std::string(SDL_GetError()));

	this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
	if(this->renderer == nullptr)
		throw std::runtime_error("Could not create SDL Renderer! Error: " + std::string(SDL_GetError()));
	

	this->screenTex = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING,
		this->width, this->height);
	if(this->screenTex == nullptr)
		throw std::runtime_error("Could not create SDL Screen Texture! Error: " + std::string(SDL_GetError()));

	SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(this->renderer);

	this->windowSurface = SDL_GetWindowSurface(this->window);
	SDL_SetWindowTitle(this->window, this->windowTitle);
}

Window::~Window() {
	SDL_DestroyWindow(this->window);
	this->window = nullptr;
	this->windowSurface = nullptr;
	SDL_Quit();
}

void Window::renderFrame() {
	// Attaches Texture Data to Texture
	/*glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, &screenTexPixels);

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
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);*/

	// Display to screen
	SDL_UpdateTexture(this->screenTex, NULL, this->screenTexPixels.data(), this->width * 3);
	SDL_RenderCopy(this->renderer, this->screenTex, NULL, NULL);
	SDL_RenderPresent(this->renderer);

	this->pollEvents();

	if(SDL_GetTicks() - this->prevFrameTick > 1000) {
		char buf[14];
		int fps = (int)((framesPassed / (SDL_GetTicks() - this->prevFrameTick) * 1000) + 0.5);
		snprintf(buf, sizeof(buf), "%s - %3d FPS", windowTitle, fps);
		SDL_SetWindowTitle(this->window, buf);
		framesPassed = 1;
		this->prevFrameTick = SDL_GetTicks();
	} else {
		framesPassed++;
	}
}

ControllerInput Window::pollEvents() {
	SDL_Event e;
	ControllerInput input;

	while(SDL_PollEvent(&e) != 0) {
		if(e.type == SDL_QUIT) {
			this->shouldQuit = true;
			return {};
		} else if(e.type == SDL_KEYDOWN) {
			switch(e.key.keysym.sym) {
			case SDLK_a:
				input.P1_A = true;
				break;
			case SDLK_b:
				input.P1_B = true;
				break;
			case SDLK_e:
				input.P1_SELECT = true;
				break;
			case SDLK_t:
				input.P1_START = true;
				break;
			case SDLK_UP:
				input.P1_UP = true;
				break;
			case SDLK_DOWN:
				input.P1_DOWN = true;
				break;
			case SDLK_LEFT:
				input.P1_LEFT = true;
				break;
			case SDLK_RIGHT:
				input.P1_RIGHT = true;
				break;
			}
		}
	}
	return input;
}
