#pragma once

#include "baseMapper.h"

class NROM : public BaseMapper {
private:
	std::array<uint8_t, 0x10000 - 0x4020> cartridge = {};
	std::array<uint8_t, 0x4000> vram = {};
public:
	NROM(std::vector<uint8_t>& PRG, std::vector<uint8_t>& CHR, uint8_t prgRamSize);

	void setRAM8(uint16_t addr, uint8_t data);
	uint8_t& getRAM8(uint16_t addr);
	void setVRAM8(uint16_t addr, uint8_t data);
	uint8_t getVRAM8(uint16_t addr) const;
};

