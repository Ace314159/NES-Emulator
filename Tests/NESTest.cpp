#include "stdafx.h"
#include "nesTest.h"

std::array<Color, Window::width* Window::height> NESTest::prevScreenTexPixels;

NESTest::NESTest(const fs::path& testFilePath) : NES(testFilePath) {
	this->framesFolder = testFilePath.parent_path() / testFilePath.stem();
	std::fill(NESTest::prevScreenTexPixels.begin(), NESTest::prevScreenTexPixels.end(), Color{0, 0, 0});

	std::basic_ifstream<GLubyte> paletteData("palette.pal", std::ios::binary);
	if(!paletteData.good()) {
		throw std::runtime_error("Could not read palette table file");
	}
	paletteData.read(reinterpret_cast<GLubyte*>(this->ppu.paletteTable.data()),
		this->ppu.paletteTable.size() * 3);
}

void NESTest::dumpScreen() const {
	if(this->prevScreenTexPixels == this->ppu.window.screenTexPixels) return;
	this->dumpNum++;
#ifdef MAKE_TESTS
	fs::path filePath = this->framesFolder / (std::to_string(this->dumpNum) + ".ppm");
	std::ofstream file(filePath, std::ios::binary);


	file << this->screenDumpHeader;
	std::copy(this->ppu.window.screenTexPixels.begin(), this->ppu.window.screenTexPixels.end(),
		std::ostream_iterator<Color>(file));
#endif
	NESTest::prevScreenTexPixels = this->ppu.window.screenTexPixels;
}

void NESTest::tick() {
	NES::tick();

	this->cycleNum++;
}