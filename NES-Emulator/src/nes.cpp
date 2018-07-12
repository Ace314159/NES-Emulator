#include "nes.h"
#include "memory/memory.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>

using std::cout;
using std::endl;

void NES::corruptedROM() {
	throw std::runtime_error("Corrupted ROM!");
}

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

	// Check if first 4 bytes are valid - Contain ASCII Text for NES and ends with 0x1A
	uint32_t buffer32;
	//romData.seekg(0, std::ios::beg);
	romData.read(reinterpret_cast<uint8_t*>(&buffer32), 4);
	if(buffer32 != 0x1A53454e) { // ASCII For NES/1A flipped for endianness
		this->corruptedROM();
	}

	// Get number of 16K PRG-ROM blocks - Program ROM
	const uint8_t prgBlocks = romData.get() & 0xff;
	// Get number of 8K CHR-ROM blocks - Character ROM
	const uint8_t chrBlocks = romData.get() & 0xff;
	// Get mapper id and control bytes
	uint8_t buffer = romData.get() & 0xff;
	uint8_t mapperID = buffer >> 4; // Lower 4 bits of mapper id
	bool fourScreenVRAM = buffer & (1 << 3);
	bool trainerExists = buffer & (1 << 2);
	this->sRAMUsed = buffer & (1 << 1);
	bool isVerticalMirroring = buffer & (1 << 0);
	if(fourScreenVRAM) {
		this->mem.nametableMirroringType = Memory::NametableMirroringType::FOUR;
	} else {
		if(isVerticalMirroring)
			this->mem.nametableMirroringType = Memory::NametableMirroringType::VERTICAL;
		else this->mem.nametableMirroringType = Memory::NametableMirroringType::HORIZONTAL;
	}
	// Get upper 4 bytes of mapper id
	buffer = romData.get() & 0xff;
	if((buffer & 0xff) != 0) { // Check if lower 4 bits of control byte 2 are 0s
		this->corruptedROM();
	}
	mapperID |= buffer;
	// Get size of PRG RAM - in 8k blocks,  0 means 1 8k
	const uint8_t prgRamBlocks = std::max(1, static_cast<int>(romData.get()));
	// Check if last 7 bytes are 0s
	uint64_t buffer64;
	romData.read(reinterpret_cast<uint8_t*>(&buffer64), 7);
	if(buffer != 0) {
		this->corruptedROM();
	}
	romData.seekg(512 * trainerExists, std::ios::cur);

	// Use mapper to load into memory
	std::vector<uint8_t> PRG, CHR;
	PRG.resize(prgBlocks * 0x4000);
	CHR.resize(chrBlocks * 0x2000);
	romData.read(&PRG[0], prgBlocks * 0x4000); // PRG
	romData.read(&CHR[0], chrBlocks * 0x2000); // CHR
	this->mem.mapper = BaseMapper::getMapper(mapperID, PRG, CHR, prgRamBlocks);

	// Set the PC to the memory address at the RESET vector location (0xFFFC/D)
	this->cpu.PC = this->mem.getRAM8(0xFFFC) | (this->mem.getRAM8(0xFFFD) << 8);
	return mem;
}

void NES::handleInput() {
	if((this->cpu.mem.mapper->apuRegisters[0x4016 - 0x4000] & 0x1) == 1) {
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
	if((this->cpu.mem.mapper->apuRegisters[0x4017 - 0x4000] & 0x1) == 1) {
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
