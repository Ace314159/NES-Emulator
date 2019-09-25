#pragma once

#include "BaseMapper.h"


class NROM : public BaseMapper {
public:
	NROM(iNESHeader header) : BaseMapper(header) {
		this->setCPUMapping(0x6000, 0x7FFF, this->WRAM.data(), true);
		this->setCPUMapping(0x8000, 0xBFFF, this->PRG.data(), false);
		this->setCPUMapping(0xC000, 0xFFFF, &this->PRG[(this->header.prgRomSize == 1) ? 0 : 0x4000], false);

		this->setCHRMapping(0x0000, 0x1FFF, this->CHR.data(), false);
	};

	virtual uint8_t read(uint16_t addr) override {
		return *this->RAMPtrs[addr - 0x4020].data;
	};

	virtual void write(uint16_t addr, uint8_t data) override {
		if(this->RAMPtrs[addr - 0x4020].canWrite) *this->RAMPtrs[addr].data = data;
	};
};

