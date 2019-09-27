#pragma once

#include "iNESHeader.h"
#include "MemoryHandler.h"


class BaseMapper : public MemoryHandler {
private:
	struct MemoryLocation {
		uint8_t* data;
		bool canWrite;
	};
public:
	BaseMapper(iNESHeader header);

	// Nametable Mirroring
	enum class NametableMirroringType { HORIZONTAL = 0, VERTICAL = 1, ONE_A = 2, ONE_B = 3, FOUR = 4};
	NametableMirroringType nametableMirroringType;

	// ROM
	std::vector<uint8_t> PRG; // CPU 0x8000 - 0xFFFF
	std::vector<uint8_t> CHR; // PPU 0x0000 - 0x1FFF
	int PRGBankSize;
	int CHRBankSize;
	int numPRGBanks;
	int numCHRBanks;
	// RAM
	std::array<MemoryHandler*, 0x10000> RAMHandlers;
	std::array<uint8_t, 0x2000> WRAM; // CPU 0x6000 - 0x7FFF
	// Pointers
	std::array<MemoryLocation, 0xBFE0> RAMPtrs;  // CPU 0x4020 - 0xFFFF
	std::array<MemoryLocation, 0x4000> VRAMPtrs; // PPU 0x0000 - 0x3FFF
	uint8_t temp; // Used when nothing should be changed

	// VRAM
	std::array<uint8_t, 0x1000> nametables; // 0x2000 - 0x2FFF
	std::array<uint8_t, 0x001C> palette{};  // 0x3F00 - 0x3F1F

	// Useful Variables
	unsigned int CPUcycleCount = 0;
	bool IRQCalled = false;
	iNESHeader header;


	// Useful Functions
	virtual uint8_t read(uint16_t addr) = 0;
	virtual void write(uint16_t addr, uint8_t data) = 0;
	virtual void setPPUBusAddress(uint16_t addr, int cycleNum) {};
	
	void setRAMHandlers(uint16_t startAddr, uint16_t endAddr, MemoryHandler& memoryHandler);

	void setCPUMapping(uint16_t startAddr, uint16_t endAddr, uint8_t* startPtr, bool canWrite);
	void setCHRMapping(uint16_t startAddr, uint16_t endAddr, uint8_t* startPtr, bool canWrite);
	
	void setPRGMapping(std::initializer_list<int> banks);
	void setCHRMapping(std::initializer_list<int> banks);

	void setPRGBank(int memIndex, int bankIndex);
	void setCHRBank(int memIndex, int bankIndex);

	void setWRAMEnabled(bool enabled);

	void setNametableMirroringType(NametableMirroringType type);

	static std::unique_ptr<BaseMapper> getMapper(iNESHeader header);
};

