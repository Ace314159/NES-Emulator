#include "stdafx.h"
#include "Test.h"

Test::Test(const fs::path& testType, const fs::path& testCategory, const fs::path& testName) : 
	nes(testType / testCategory / (testName.string() + ".nes")) {}

void Test::makeTest() {
	while(glfwGetKey(this->nes.ppu.window.window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
		this->nes.tick();
		this->nes.dumpScreen();
	}
}