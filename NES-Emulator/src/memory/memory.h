#pragma once

#include <array>
#include <cstdint>


class Memory {
public:
	class PPU;
	class CPU {
	public:
		CPU(Memory::PPU& p, int& cyc);

		// Main
		Memory::PPU& ppuMem;
		uint16_t getAddr(uint16_t addr); // Gets actual address after memory mirroring
		std::array<uint8_t, 0x10000> arr;
		// std::array<uint8_t, 0x10000> arr = {};
		uint8_t get8(uint16_t addr);
		void set8(uint16_t addr, uint8_t data);

		// Input
		std::array<uint8_t, 8> buttons1 = {};
		int buttons1Index = 0;
		std::array<uint8_t, 8> buttons2 = {};
		int buttons2Index = 0;

		// Useful Variables
		int& cycleNum; // CYC in CPU

		// NMI and DMA - References in PPU
		bool NMICalled = false;
		bool inNMI = false;
		bool inDMA = false;
		//bool DMACalled = false;
		uint16_t DMAAddr;
		uint8_t DMAPage;
		uint8_t DMAVal;
		bool inOddCycle = false;
		bool DMAdoneDummy = false;
	};

	class PPU {
	public:
		PPU(Memory::CPU& c, uint16_t& cu, int& scan);
		enum class NametableMirroringType { HORIZONTAL, VERTICAL, ONE, FOUR };

		// Main
		uint16_t getAddr(uint16_t addr); // Gets Actual Address after memory mirroring
		std::array<uint8_t, 0x10000> arr = {};
		std::array<uint8_t, 0x100> OAM = {}; // Object Attribute Memory
		uint8_t get8(uint16_t addr);
		void set8(uint16_t addr, uint8_t data);

		// Variables
		NametableMirroringType nametableMirroringType;
		uint16_t& currentVramAddr;
		int& scanlineNum;

		// Buses between CPU and PPU
		uint8_t cpuBus;
		uint8_t DATAReadBuffer;
		uint16_t registerRead;
		uint16_t registerWritten;
		bool canWrite;

		// Everything shared by CPU and PPU
		bool& NMICalled;
		bool& inNMI;
		bool& inDMA;
		// bool& DMACalled;
		uint16_t& DMAAddr;
		uint8_t& DMAPage;
		uint8_t& DMAVal;
		bool& inOddCycle;
		bool& DMAdoneDummy;
		uint8_t *registers; // $2000 to $2007
		uint8_t& register4014; // $4014
	};

	Memory(uint16_t& cu, int& scan, int& cyc);
	CPU cpuMem;
	PPU ppuMem;
};

