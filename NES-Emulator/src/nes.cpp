#include "nes.h"
#include "memory/memory.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>

using std::cout;
using std::endl;

NES::NES(const std::chrono::nanoseconds ct, std::string romFileName) : CLOCK_TIME(ct), ppu(loadRom(romFileName)) {}

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

Memory& NES::loadRom(std::string romFileName) {
	std::basic_ifstream<uint8_t> romData(romFileName, std::ios::binary);
	if(!romData.good()) {
		throw std::runtime_error("Could not read ROM file");
	}

	iNESHeader header;
	romData.read(reinterpret_cast<uint8_t*>(&header), 16);
	header.parse();
	romData.seekg(512 * header.containsTrainer, std::ios::cur);

	// Initialize mapper
	std::vector<uint8_t> PRG, CHR;
	PRG.resize(header.prgRomSize * 0x4000);
	if(header.chrRomSize > 0) CHR.resize(header.chrRomSize * 0x2000);
	else CHR.resize(0x2000); // Otherwise initializes 8K of CHR RAM
	romData.read(&PRG[0], header.prgRomSize * 0x4000); // PRG
	if(header.chrRomSize > 0) romData.read(&CHR[0], header.chrRomSize * 0x2000); // CHR

	cout << "Using Mapper " << (int)header.mapperID << endl;
	this->mem.mapper = BaseMapper::getMapper(header.mapperID, PRG, CHR, header.prgRamSize);
	this->mem.mapper->header = header;
	this->mem.mapper->setNametableMirroringType(
		static_cast<BaseMapper::NametableMirroringType>(header.nametableMirroringType));

	// Set the PC to the memory address at the RESET vector location (0xFFFC/D)
	this->cpu.PC = this->mem.mapper->getRAM8(0xFFFC) | (this->mem.mapper->getRAM8(0xFFFD) << 8);
	return mem;
}

void NES::handleInput() {
	if((this->cpu.mem.mapper->apuRegisters[0x16] & 0x1) == 1) {
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
	if((this->cpu.mem.mapper->apuRegisters[0x17] & 0x1) == 1) {
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
