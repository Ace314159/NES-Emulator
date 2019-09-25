#pragma once

#include "BaseMapper.h"

/*
class MMC1 : public BaseMapper {
private:
	// Registers
	uint8_t shiftReg = 0;
	uint8_t CTRL = 0x0C;
	uint8_t CHRBank0 = 0;
	uint8_t CHRBank1 = 0;
	uint8_t PRGBank = 0;

	// Useful Variables
	unsigned int prevCPUCycleCount = 0;
	uint8_t shiftRegCount = 0;
public:
	MMC1(iNESHeader header) : BaseMapper(header) {};

	bool canWriteRAM8(uint16_t addr) override;
	void wroteRAM8(uint16_t addr, uint8_t data) override;
	uint8_t getPRGBank(uint16_t& addr) override;
	uint16_t getPRGBankSize() override;
	uint8_t getCHRBank(uint16_t& addr) override;
	uint16_t getCHRBankSize(uint16_t addr) override;
	bool WRAMEnabled() override;
};
*/
