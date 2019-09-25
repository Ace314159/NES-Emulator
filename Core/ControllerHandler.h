#pragma once

#include "MemoryHandler.h"
#include <cassert>

class ControllerHandler : public MemoryHandler {
public:
	bool poll = false;

	std::array<bool, 8> buttons1{};
	uint8_t buttons1Index = 0;
	std::array<bool, 8> buttons2{};
	uint8_t buttons2Index = 0;

	virtual uint8_t read(uint16_t addr) override {
		switch(addr) {
		case 0x4016:
			if(this->buttons1Index < 8) return this->buttons1[this->buttons1Index++];
			else return 0x01;
			break;
		case 0x4017:
			if(this->buttons2Index < 8) return this->buttons2[this->buttons2Index++];
			else return 0x01;
			break;
		default:
			assert(false);
			return 0;
		}
	}

	virtual void write(uint16_t addr, uint8_t data) override {
		assert(addr == 0x4016);
		this->poll = data & 0x1;
	}
};
