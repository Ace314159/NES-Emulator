#include "stdafx.h"
#include "nes.h"
#include "memory/memory.h"

NES::NES(const std::chrono::nanoseconds ct, std::string romFileName) : CLOCK_TIME(ct) { loadRom(romFileName); }

void NES::tick() {
	//auto endTime = std::chrono::high_resolution_clock::now() + CLOCK_TIME;
	if(this->cycleCount == 29658) this->ppu.canWrite = true;

	this->cpu.emulateCycle();

	this->ppu.emulateCycle(true);
	this->ppu.emulateCycle(false);
	this->ppu.emulateCycle(false);

	this->handleInput();

	// cout << "!" << (int)ppu.currentVramAddr << endl;
	// cout << "!!" << (int)ppu.tempVramAddr << endl;

	this->cycleCount++;
	/*while(std::chrono::high_resolution_clock::now() < endTime) {
		cout << "Ahead" << endl;
	}*/
}

void NES::loadRom(std::string romFileName) {
	std::basic_ifstream<uint8_t> romData(romFileName, std::ios::binary);
	if(!romData.good()) {
		throw std::runtime_error("Could not read ROM file");
	}

	std::array<uint8_t, 16> headerData;
	romData.read(reinterpret_cast<uint8_t*>(&headerData), 16);
	iNESHeader header;
	header.init(headerData);
	romData.seekg(512 * header.containsTrainer, std::ios::cur);

	// Initialize mapper
	cout << "Using Mapper " << (int)header.mapperID << endl;
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
	if(this->cpu.mem.apuRegisters[0x16] & 0x1) {
		this->cpu.mem.buttons1[0] = glfwGetKey(Graphics::window, GLFW_KEY_A);
		this->cpu.mem.buttons1[1] = glfwGetKey(Graphics::window, GLFW_KEY_B);
		this->cpu.mem.buttons1[2] = glfwGetKey(Graphics::window, GLFW_KEY_E);
		this->cpu.mem.buttons1[3] = glfwGetKey(Graphics::window, GLFW_KEY_T);
		this->cpu.mem.buttons1[4] = glfwGetKey(Graphics::window, GLFW_KEY_UP);
		this->cpu.mem.buttons1[5] = glfwGetKey(Graphics::window, GLFW_KEY_DOWN);
		this->cpu.mem.buttons1[6] = glfwGetKey(Graphics::window, GLFW_KEY_LEFT);
		this->cpu.mem.buttons1[7] = glfwGetKey(Graphics::window, GLFW_KEY_RIGHT);
		this->cpu.mem.buttons1Index = 0;
	}
	if(this->cpu.mem.apuRegisters[0x17] & 0x1) {
		this->cpu.mem.buttons2[0] = 0;
		this->cpu.mem.buttons2[1] = 0;
		this->cpu.mem.buttons2[2] = 0;
		this->cpu.mem.buttons2[3] = 0;
		this->cpu.mem.buttons2[4] = 0;
		this->cpu.mem.buttons2[5] = 0;
		this->cpu.mem.buttons2[6] = 0;
		this->cpu.mem.buttons2[7] = 0;
		this->cpu.mem.buttons2Index = 0;
	}
}
