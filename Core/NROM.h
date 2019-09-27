#pragma once

#include "BaseMapper.h"


class NROM : public BaseMapper {
public:
	NROM(iNESHeader header) : BaseMapper(header) {
		this->setWRAMEnabled(true);
		this->setPRGMapping({0, -1});
		this->setCHRMapping({0});
	};

	virtual uint8_t read(uint16_t addr) override {
		return *this->RAMPtrs[addr - 0x4020].data;
	};

	virtual void write(uint16_t addr, uint8_t data) override {
		if(this->RAMPtrs[addr - 0x4020].canWrite) *this->RAMPtrs[addr - 0x4020].data = data;
	};
};

