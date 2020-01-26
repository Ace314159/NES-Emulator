#include "stdafx.h"
#include "NES.h"
#include "Memory.h"

NES::NES(const fs::path& romFileName) { loadRom(romFileName); }

NES::~NES() { glfwTerminate(); }

void NES::tick() {
	this->cpu.emulateInstr();

	this->handleInput();
}

void NES::loadRom(const fs::path& romFilePath) {
	std::basic_ifstream<uint8_t> romData(romFilePath, std::ios::binary);
	if(!romData.good()) {
		throw std::runtime_error("Could not read ROM file");
	}

	std::array<uint8_t, 16> headerData;
	romData.read(headerData.data(), 16);
	iNESHeader header;
	header.init(headerData);
	if(header.containsTrainer) romData.seekg(512, std::ios::cur);

	// Initialize mapper
	this->mem.mapper = BaseMapper::getMapper(header);

	romData.read(this->mem.mapper->PRG.data(), 0x4000 * header.prgRomSize); // PRG
	romData.read(this->mem.mapper->CHR.data(), 0x2000 * header.chrRomSize); // CHR

	this->mem.setMemoryHandlers();

	// Set the PC to the memory address at the RESET vector location (0xFFFC/D)
	this->cpu.PC = this->mem.getRAM8(0xFFFC) | (this->mem.getRAM8(0xFFFD) << 8);
}

void NES::handleInput() {
	if(this->mem.controllerHandler.poll) {
		this->mem.controllerHandler.buttons1[0] = glfwGetKey(this->ppu.window.window, GLFW_KEY_A) == GLFW_PRESS;
		this->mem.controllerHandler.buttons1[1] = glfwGetKey(this->ppu.window.window, GLFW_KEY_B) == GLFW_PRESS;
		this->mem.controllerHandler.buttons1[2] = glfwGetKey(this->ppu.window.window, GLFW_KEY_E) == GLFW_PRESS;
		this->mem.controllerHandler.buttons1[3] = glfwGetKey(this->ppu.window.window, GLFW_KEY_T) == GLFW_PRESS;
		this->mem.controllerHandler.buttons1[4] = glfwGetKey(this->ppu.window.window, GLFW_KEY_UP) == GLFW_PRESS;
		this->mem.controllerHandler.buttons1[5] = glfwGetKey(this->ppu.window.window, GLFW_KEY_DOWN) == GLFW_PRESS;
		this->mem.controllerHandler.buttons1[6] = glfwGetKey(this->ppu.window.window, GLFW_KEY_LEFT) == GLFW_PRESS;
		this->mem.controllerHandler.buttons1[7] = glfwGetKey(this->ppu.window.window, GLFW_KEY_RIGHT) == GLFW_PRESS;
		this->mem.controllerHandler.buttons1Index = 0;
	}
}

bool NES::isRunning() {
	return !glfwWindowShouldClose(this->ppu.window.window);
}
