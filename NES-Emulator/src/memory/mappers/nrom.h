#pragma once

#include "baseMapper.h"


class NROM : public BaseMapper {
public:
	NROM(iNESHeader header) : BaseMapper(header) {};

	bool canWriteRAM8(uint16_t addr);
	void wroteRAM8(uint16_t addr, uint8_t data) override;
	uint8_t getPRGBank(uint16_t& addr) override;
	uint16_t getPRGBankSize() override;
	uint8_t getCHRBank(uint16_t& addr) override;
	uint16_t getCHRBankSize() override;
	bool WRAMEnabled() override;
};

