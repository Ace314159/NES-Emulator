#pragma once

#include "baseMapper.h"


class NROM : public BaseMapper {
private:
	std::array<uint8_t, 0x2000> prgRam;
	std::array<uint8_t, 0x8000> prgRom;
	std::array<uint8_t, 0x2000> CHR;
public:
	NROM(iNESHeader header, PRGBank& PRG, CHRBank& CHR);

	void setRAM8(uint16_t addr, uint8_t data) override;
	uint8_t& getRAM8(uint16_t addr) override;
	void setVRAM8(uint16_t addr, uint8_t data) override;
	uint8_t& getVRAM8(uint16_t addr) override;
};

