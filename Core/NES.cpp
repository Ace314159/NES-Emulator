#include "stdafx.h"
#include "NES.h"
#include "Memory.h"

NES::NES(const fs::path& romFileName) { loadRom(romFileName); for(int i = 0; i < 30; i++) ppu.emulateDot(); }

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
	romData.read(reinterpret_cast<uint8_t*>(&headerData), 16);
	iNESHeader header;
	header.init(headerData);
	romData.seekg(512 * header.containsTrainer, std::ios::cur);

	// Initialize mapper
#ifdef _DEBUG
	// cout << "Using Mapper " << (int)header.mapperID << endl;
#endif
	this->mem.mapper = BaseMapper::getMapper(header);

	this->mem.PRG.resize(header.prgRomSize * 0x4000);
	if(header.chrRomSize > 0) this->mem.CHR.resize(header.chrRomSize * 0x2000);
	else this->mem.CHR.resize(0x2000); // Otherwise initializes 8K of CHR RAM
	romData.read(this->mem.PRG.data(), header.prgRomSize * 0x4000); // PRG
	romData.read(this->mem.CHR.data(), header.chrRomSize * 0x2000); // CHR


	// Set the PC to the memory address at the RESET vector location (0xFFFC/D)
	this->cpu.PC = this->mem.getRAM8(0xFFFC) | (this->mem.getRAM8(0xFFFD) << 8);
}

void NES::handleInput() {
	if(this->cpu.mem.apuRegisters[0x16] & 0x01) {
		this->cpu.mem.buttons1[0] = glfwGetKey(this->ppu.window.window, GLFW_KEY_A) == GLFW_PRESS;
		this->cpu.mem.buttons1[1] = glfwGetKey(this->ppu.window.window, GLFW_KEY_B) == GLFW_PRESS;
		this->cpu.mem.buttons1[2] = glfwGetKey(this->ppu.window.window, GLFW_KEY_E) == GLFW_PRESS;
		this->cpu.mem.buttons1[3] = glfwGetKey(this->ppu.window.window, GLFW_KEY_T) == GLFW_PRESS;
		this->cpu.mem.buttons1[4] = glfwGetKey(this->ppu.window.window, GLFW_KEY_UP) == GLFW_PRESS;
		this->cpu.mem.buttons1[5] = glfwGetKey(this->ppu.window.window, GLFW_KEY_DOWN) == GLFW_PRESS;
		this->cpu.mem.buttons1[6] = glfwGetKey(this->ppu.window.window, GLFW_KEY_LEFT) == GLFW_PRESS;
		this->cpu.mem.buttons1[7] = glfwGetKey(this->ppu.window.window, GLFW_KEY_RIGHT) == GLFW_PRESS;
		this->cpu.mem.buttons1Index = 0;
	}
}

bool NES::isRunning() {
	return !glfwWindowShouldClose(this->ppu.window.window);
}
