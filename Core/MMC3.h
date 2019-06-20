#pragma once

#include "BaseMapper.h"


class MMC3 : public BaseMapper {
private:
	// Registers
	uint8_t bankSelect = 0;
	uint8_t WRAMProtect = 0;
	// Bank Registers
	std::array<uint8_t, 8> R = {0};
	// IRQ
	uint8_t IRQCounter = 0;
	uint8_t IRQLatch = 0;
	bool IRQReload = false;
	bool IRQEnabled = false;
	bool prevA12 = 0;
	int prevCycleNum = 0;
	unsigned int prevCPUcycleCount = 0;
public:
	MMC3(iNESHeader header) : BaseMapper(header) {}

	bool canWriteRAM8(uint16_t addr) override;
	void wroteRAM8(uint16_t addr, uint8_t data) override;
	void setPPUBusAddress(uint16_t addr, int cycleNum) override;
	uint8_t getPRGBank(uint16_t& addr) override;
	uint16_t getPRGBankSize() override;
	uint8_t getCHRBank(uint16_t& addr) override;
	uint16_t getCHRBankSize(uint16_t addr) override;
	bool WRAMEnabled() override;
};

