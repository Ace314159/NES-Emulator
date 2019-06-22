#pragma once

#include "BaseMapper.h"

class CPU;
class PPU;
class APU;

class Memory {
public:
	Memory(CPU& cpu, PPU& ppu, APU& apu);
	std::unique_ptr<BaseMapper> mapper;
	CPU& cpu;
	PPU& ppu;
	APU& apu;

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

	// NMI
	bool NMICalled = false; // PPU Request

	// PPU
	// Main
	uint8_t& getNametableLoc(uint16_t offsettedAddr);
	uint8_t& getCHRLoc(uint16_t addr);
	uint8_t& getPaletteLoc(uint16_t offsettedAddr);
	uint8_t& getVRAMLoc(uint16_t addr); // Gets memory location from mapper
	std::array<uint8_t, 0x100> OAM = {}; // Object Attribute Memory
	uint8_t getVRAM8(uint16_t addr);
	void setVRAM8(uint16_t addr, uint8_t data);

	
	// Buses between CPU and PPU
	uint8_t cpuPpuBus = 0;
	uint8_t ppuDATAReadBuffer = 0;
	bool ppuCanWrite = 0;
};

