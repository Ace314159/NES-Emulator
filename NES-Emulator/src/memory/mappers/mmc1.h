#pragma once

#include "baseMapper.h"


class MMC1 : public BaseMapper {
private:
	std::array<uint8_t, 0x2000> prgRam;
	std::vector<uint8_t> prgRom;
	std::vector<uint8_t> CHR;

	// Registers
	uint8_t shiftReg;
	uint8_t CTRL = 0x0C;
	uint8_t CHRBank0;
	uint8_t CHRBank1;
	uint8_t PRGBank;

	// Useful Variables
	uint8_t shiftRegCount = 0;

	// Useful Methods
	uint8_t& getRAMLoc(uint16_t addr);
	uint8_t& getVRAMLoc(uint16_t addr);
public:
	MMC1(iNESHeader header, std::vector<uint8_t>& PRG, std::vector<uint8_t>& CHR);

	void setRAM8(uint16_t addr, uint8_t data) override;
	uint8_t& getRAM8(uint16_t addr) override;
	void setVRAM8(uint16_t addr, uint8_t data) override;
	uint8_t& getVRAM8(uint16_t addr) override;
};

