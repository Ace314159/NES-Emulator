#include "stdafx.h"
#include "nesTest.h"


NESTest::NESTest(const std::string& testFileName) : NES(testFileName) {
	this->framesFolder = testFileName.substr(0, testFileName.find_last_of("."));
	std::filesystem::create_directory(this->framesFolder);
	std::fill(this->prevScreenTexPixels.begin(), this->prevScreenTexPixels.end(), Color{0, 0, 0});

	std::basic_ifstream<GLubyte> paletteData("palette.pal", std::ios::binary);
	if(!paletteData.good()) {
		throw std::runtime_error("Could not read palette table file");
	}
	paletteData.read(reinterpret_cast<GLubyte*>(this->ppu.paletteTable.data()), this->ppu.paletteTable.size() * 3);
}

void NESTest::dumpScreen() {
	if(this->ppu.scanlineNum != 241 || this->ppu.cycleNum != 1 ||
		this->prevScreenTexPixels == this->ppu.window.screenTexPixels) return;
	std::filesystem::path filePath = this->framesFolder / (std::to_string(this->cycleNum) + ".ppm");
	std::ofstream file(filePath, std::ios::binary);


	file << this->screenDumpHeader;
	std::copy(this->ppu.window.screenTexPixels.begin(), this->ppu.window.screenTexPixels.end(),
		std::ostream_iterator<Color>(file));
	this->prevScreenTexPixels = this->ppu.window.screenTexPixels;
}

void NESTest::tick() {
	NES::tick();
	this->cycleNum++;
}
