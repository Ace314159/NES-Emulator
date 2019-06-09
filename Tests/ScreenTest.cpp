#include "stdafx.h"
#include "screenTest.h"


bool ScreenTest::passed() {
	std::vector<std::filesystem::path> filePaths;
	for(const auto& entry : std::filesystem::directory_iterator(this->nes.framesFolder)) {
		filePaths.push_back(entry.path());
	}
	std::array<Color, Window::width * Window::height> screenTexPixels;
	for(auto filePath : filePaths) {
		unsigned long long cycleNum = std::stoull(filePath.stem().string());
		while(this->nes.cycleNum < cycleNum) {
			this->nes.tick();
		}
		assert(this->nes.cycleNum == cycleNum);

		std::basic_ifstream<GLubyte> file(filePath, std::ios::binary);
		file.seekg(this->nes.screenDumpHeader.size());
		file.read(reinterpret_cast<GLubyte*>(screenTexPixels.data()), screenTexPixels.size() * 3);
		if(this->nes.ppu.window.screenTexPixels != screenTexPixels) {
			return false;
		}
	}
	return true;
}
