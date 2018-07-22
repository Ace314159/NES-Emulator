#pragma once

#include "../../headers/iNESHeader.h"

#include <array>
#include <vector>
#include <memory>
#include <iostream>

using std::cout;
using std::endl;


class BaseMapper {
protected:
	uint8_t temp; // A return value when no memory should be changed
public:
	// Nametable Mirroring - ONE_A is 0x2000 - 0x23FF and ONE_B is 0x2400 - 0x27FF
	enum class NametableMirroringType { HORIZONTAL = 0, VERTICAL = 1, ONE_A = 2, ONE_B = 3, FOUR = 4};
	NametableMirroringType nametableMirroringType;

	bool writeCycleDone = false;
	iNESHeader header;


	virtual void setRAM8(uint16_t addr, uint8_t data) = 0;
	virtual uint8_t& getRAM8(uint16_t addr) = 0;
	virtual void setVRAM8(uint16_t addr, uint8_t data) = 0;
	virtual uint8_t& getVRAM8(uint16_t addr) = 0;

	// RAM
	std::array<uint8_t, 0x0800> internalRAM;   // 0x0000 - 0x07FF
	std::array<uint8_t, 0x0008> ppuRegisters;  // 0x2000 - 0x2007
	std::array<uint8_t, 0x0018> apuRegisters;  // 0x4000 - 0x4017 - Not really APU registers, but almost all are
	// VRAM
	std::array<uint8_t, 0x1000> nametables;    // 0x2000 - 0x2FFF
	std::array<uint8_t*, 0x1000> nametablePtrs;
	std::array<uint8_t, 0x0020> palette{};     // 0x3F00 - 0x3F1F

	static std::unique_ptr<BaseMapper> getMapper(uint8_t mapperID, std::vector<uint8_t>& PRG,
		std::vector<uint8_t>& CHR);

	void setNametableMirroringType(NametableMirroringType type);
};

