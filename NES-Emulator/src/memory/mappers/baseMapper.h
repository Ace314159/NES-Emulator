#pragma once

#include "headers/iNESHeader.h"


class BaseMapper {
public:
	BaseMapper(iNESHeader header);

	// Nametable Mirroring
	enum class NametableMirroringType { HORIZONTAL = 0, VERTICAL = 1, ONE_A = 2, ONE_B = 3, FOUR = 4};
	NametableMirroringType nametableMirroringType;

	// VRAM
	std::array<uint8_t, 0x1000> nametables;     // 0x2000 - 0x2FFF
	std::array<uint8_t*, 0x1000> nametablePtrs; // 0x2000 - 0x2FFF
	std::array<uint8_t, 0x0020> palette{};      // 0x3F00 - 0x3F1F

	// Useful Variables
	unsigned int CPUcycleCount = 0;
	unsigned prevCPUCycleCount;
	iNESHeader header;


	// Useful Functions
	virtual void wroteRAM8(uint16_t addr, uint8_t data) = 0;
	virtual uint8_t getPRGBank(uint16_t& addr) = 0; // Is reference in order to offset addr when accessing PRG ROM
	virtual uint16_t getPRGBankSize() = 0;
	virtual uint8_t getCHRBank(uint16_t& addr) = 0; // Is reference in order to offset addr when accessing CHR
	virtual uint16_t getCHRBankSize() = 0;
	virtual bool WRAMEnabled() = 0;
	
	void setNametableMirroringType(NametableMirroringType type);
	static std::unique_ptr<BaseMapper> getMapper(iNESHeader header);

};

