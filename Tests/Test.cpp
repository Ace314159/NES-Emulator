#include "stdafx.h"

#include "Test.h"

Test::Test(const std::string& testFileName) : nes(testFileName) {}

void Test::makeTest() {
	while(glfwGetKey(this->nes.ppu.window.window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
		this->nes.tick();
		this->nes.dumpScreen();
	}
}