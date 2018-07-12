#pragma once

#include "mappers\baseMapper.h"

#include <array>
#include <cstdint>


class Memory {
public:
	Memory(uint16_t& cu, int& scan, int& cyc);
	std::unique_ptr<BaseMapper> mapper;

	// CPU
	// Main
	uint8_t& getRAMLoc(uint16_t& addr); // Gets actual address after memory mirroring
	uint8_t getRAM8(uint16_t addr);
	void setRAM8(uint16_t addr, uint8_t data);

	// Input
	std::array<bool, 8> buttons1 = {};
	int buttons1Index = 0;
	std::array<bool, 8> buttons2 = {};
	int buttons2Index = 0;

	// Useful Variables
	int& cycleNum; // CYC in CPU

	// NMI and DMA
	bool NMICalled = false;
	bool inNMI = false;
	bool inDMA = false;
	//bool DMACalled = false;
	uint16_t DMAAddr;
	uint8_t DMAPage;
	uint8_t DMAVal;
	bool inOddCycle = false;
	bool DMAdoneDummy = false;

	// PPU
	enum class NametableMirroringType { HORIZONTAL, VERTICAL, ONE, FOUR };

	// Main
	uint16_t getVRAMAddr(uint16_t addr); // Gets Actual Address after memory mirroring
	std::array<uint8_t, 0x100> OAM = {}; // Object Attribute Memory
	uint8_t getVRAM8(uint16_t addr);
	void setVRAM8(uint16_t addr, uint8_t data);

	// Variables
	NametableMirroringType nametableMirroringType;
	uint16_t& currentVramAddr;
	int& scanlineNum;

	// Buses between CPU and PPU
	uint8_t cpuPpuBus;
	uint8_t ppuDATAReadBuffer;
	uint16_t ppuRegisterRead;
	uint16_t ppuRegisterWritten;
	bool ppuCanWrite;
};

