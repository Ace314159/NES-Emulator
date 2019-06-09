#pragma once

#include "NESTest.h"

class Test {
protected:
	NESTest nes;
public:
	explicit Test(const std::filesystem::path testFileName) : nes(testFileName.string()) {};

	inline void makeTest() {
		while(glfwGetKey(this->nes.ppu.window.window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
			this->nes.tick();
			this->nes.dumpScreen();
		}
	}
	virtual bool passed() = 0;
};

