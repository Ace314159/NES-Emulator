#pragma once


#include "NES.h"

class NESTest : public NES {
public:
	NESTest(const fs::path& testFilePath);
	void tick();


	unsigned long long cycleNum = 0;
	const std::string screenDumpHeader =
		"P6\n" +std::to_string(Window::width) + " " + std::to_string(Window::height) + "\n255\n";
	std::filesystem::path framesFolder;

	static std::array<Color, Window::width * Window::height> prevScreenTexPixels;
	void dumpScreen() const;
};

