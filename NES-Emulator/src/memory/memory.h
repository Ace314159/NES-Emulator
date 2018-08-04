#pragma once

#include "mappers/baseMapper.h"


class Memory {
public:
	Memory(uint16_t& cu, int& scan, int& cyc);
	std::unique_ptr<BaseMapper> mapper;

	// Cartridge
	std::array<uint8_t, 0x2000> WRAM; // CPU 0x6000 - 0x7FFF
	std::vector<uint8_t> PRG; // CPU 0x8000 - 0xFFFF
	std::vector<uint8_t> CHR; // PPU 0x0000 - 0x1FFF
	uint8_t temp; // A return value when no memory should be changed

	// CPU
	// Main
	enum class RAMAddrType { INTERNAL_RAM, PPU_REGISTER, APU_REGISTER, WRAM, CARTRIDGE };
	std::pair<RAMAddrType, uint8_t*> getRAMAddrData(uint16_t& addr);
	uint8_t getRAM8(uint16_t addr);
	void setRAM8(uint16_t addr, uint8_t data);
	// RAM
	std::array<uint8_t, 0x0800> internalRAM;  // 0x0000 - 0x07FF
	std::array<uint8_t, 0x0008> ppuRegisters{}; // 0x2000 - 0x2007
	std::array<uint8_t, 0x0018> apuRegisters; // 0x4000 - 0x4017 - Not really APU registers, but almost all are

	// Input
	std::array<bool, 8> buttons1{};
	uint8_t buttons1Index = 0;
	std::array<bool, 8> buttons2{};
	uint8_t buttons2Index = 0;

	// Useful Variables
	int& cycleNum; // CYC in CPU

	// NMI
	bool NMICalled = false; // PPU Request
	bool inNMI = false; // CPU Response
	// DMA
	bool inDMA = false;
	//bool DMACalled = false;
	uint16_t DMAAddr;
	uint8_t DMAPage;
	uint8_t DMAVal;
	bool inOddCycle = false;
	bool DMAdoneDummy = false;
	// IRQ
	bool inIRQ = false;

	// PPU
	// Main
	uint8_t& getVRAMLoc(uint16_t addr); // Gets memory location from mapper
	std::array<uint8_t, 0x100> OAM = {}; // Object Attribute Memory
	uint8_t getVRAM8(uint16_t addr);
	void setVRAM8(uint16_t addr, uint8_t data);

	// Variables
	uint16_t& currentVramAddr;
	int& scanlineNum;

	// Buses between CPU and PPU
	uint8_t cpuPpuBus = 0;
	uint8_t ppuDATAReadBuffer = 0;
	uint16_t ppuRegisterRead = 0;
	uint16_t ppuRegisterWritten = 0;
	bool ppuCanWrite = 0;
	// Buses between CPU and APU
	uint16_t apuRegisterRead = 0;
	uint16_t apuRegisterWritten = 0;
};

