#pragma once

#include "MemoryHandler.h"

class BlankHandler : public MemoryHandler {
public:
	virtual uint8_t read(uint16_t addr) override {
		return 0;
	}

	virtual void write(uint16_t addr, uint8_t data) override {
		
	}
};
