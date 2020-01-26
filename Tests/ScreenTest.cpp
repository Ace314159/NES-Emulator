#include "stdafx.h"
#include "ScreenTest.h"

void ScreenTest::passed() {
	Test::passed();

	std::vector<fs::path> filePaths;
	for(const auto& entry : fs::directory_iterator(this->nes.framesFolder)) {
		filePaths.push_back(entry.path());
	}
	std::sort(filePaths.begin(), filePaths.end(), [](const auto& a, const auto& b) {
		return std::stoull(a.stem().string()) < std::stoull(b.stem().string());
	});
	std::array<Color, Window::width * Window::height> screenTexPixels;
	for(auto filePath : filePaths) {
		int dumpNum = std::stoul(filePath.stem().string());
		while(this->nes.dumpNum < dumpNum) {
			this->nes.tick();
		}

		std::basic_ifstream<GLubyte> file(filePath, std::ios::binary);
		file.seekg(this->nes.screenDumpHeader.size());
		file.read(reinterpret_cast<GLubyte*>(screenTexPixels.data()), screenTexPixels.size() * 3);
		if(this->nes.ppu.window.screenTexPixels != screenTexPixels) {
			throw std::runtime_error("Test Failed!");
		}
	}
}