#include "stdafx.h"
#include "Test.h"


TestFailException::TestFailException(const fs::path& framesFolder, const std::string& error) :
	framesFolder(framesFolder), error(error), runtime_error(error) {}

Test::Test(const fs::path& testType, const fs::path& testCategory, const fs::path& testName) 
try : nes(testType / testCategory / (testName.string() + ".nes")) {
} catch(const std::runtime_error& e) {
	throw TestFailException(testType / testCategory / testName.string(), e.what());
}

void Test::makeTest() {
	fs::remove_all(this->nes.framesFolder);
	fs::create_directory(this->nes.framesFolder);
	
	try {
		while(glfwGetKey(this->nes.ppu.window.window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
			this->nes.tick();
			this->nes.dumpScreen();
			if(glfwGetKey(this->nes.ppu.window.window, GLFW_KEY_F) == GLFW_PRESS)
				throw std::runtime_error("Test Failed!");
		}
	} catch(const std::runtime_error& e) {
		throw TestFailException(this->nes.framesFolder, e.what());
	}
}

void Test::passed() {
	fs::path failFile = this->nes.framesFolder / "FAIL";

	if(fs::exists(failFile)) {
		std::ifstream file(failFile);
		std::stringstream error;
		error << file.rdbuf();
		throw std::runtime_error(error.str());
	}
}
