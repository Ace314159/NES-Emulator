#pragma once

#include "BaseMapper.h"

class CNROM : public BaseMapper {
public:
	CNROM(iNESHeader header) : BaseMapper(header) {
		this->setWRAMEnabled(false);
		this->setPRGMapping({0});
		this->setCHRMapping({0});
	}

	virtual uint8_t read(uint16_t addr) override {
		return *this->RAMPtrs[addr - 0x4020].data;
	};

	virtual void write(uint16_t addr, uint8_t data) override {
		if(addr >= 0x8000) this->setCHRMapping({data & 0x3});
		if(this->RAMPtrs[addr - 0x4020].canWrite) *this->RAMPtrs[addr - 0x4020].data = data;
	};
};
