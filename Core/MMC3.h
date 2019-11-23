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
	
	void setPRGBanks() {
		if(this->bankSelect & 0x40) this->setPRGMapping({-2, this->R[7], this->R[6], -1});
		else this->setPRGMapping({this->R[6], this->R[7], -2, -1});
	}

	void setCHRBanks() {
		if(this->bankSelect & 0x80)
			this->setCHRMapping({this->R[2], this->R[3], this->R[4], this->R[5],
				this->R[0], this->R[0] | 0x01, this->R[1], this->R[1] | 0x01});
		else
			this->setCHRMapping({this->R[0], this->R[0] | 0x01, this->R[1], this->R[1] | 0x01,
				this->R[2], this->R[3], this->R[4], this->R[5]});
	}
public:
	MMC3(iNESHeader header) : BaseMapper(header) {
		this->setWRAMEnabled(true);
		this->setPRGBanks();
		this->setCHRBanks();
	}

	virtual uint8_t read(uint16_t addr) override {
		return *this->RAMPtrs[addr - 0x4020].data;
	};

	virtual void write(uint16_t addr, uint8_t data) override {
		if(this->RAMPtrs[addr - 0x4020].canWrite) *this->RAMPtrs[addr - 0x4020].data = data;
		if(addr < 0x8000) return;

		uint8_t Rval;
		switch(addr & 0xE001) {
		case 0x8000:
			this->bankSelect = data;
			this->setPRGBanks();
			this->setCHRBanks();
			break;
		case 0x8001:
			Rval = this->bankSelect & 0x7;
			if(Rval >= 6) {
				this->R[Rval] = data & ~0xC0;
				this->setPRGBanks();
			} else {
				if(Rval <= 1) data &= ~0x01;
				this->R[Rval] = data;
				this->setCHRBanks();
			}
			break;
		case 0xA000:
			if(this->nametableMirroringType != NametableMirroringType::FOUR) {
				if(data & 0x01) this->setNametableMirroringType(NametableMirroringType::HORIZONTAL);
				else this->setNametableMirroringType(NametableMirroringType::VERTICAL);
			}
			break;
		case 0xA001:
			// Unimplemented to avoid incompatibility with MMC6
			break;
		case 0xC000:
			this->IRQLatch = data;
			break;
		case 0xC001:
			this->IRQCounter = 0;
			this->IRQReload = true;
			break;
		case 0xE000:
			this->IRQEnabled = false;
			this->IRQCalled = false;
			break;
		case 0xE001:
			this->IRQEnabled = true;
			break;
		}
	};

	void setPPUBusAddress(uint16_t addr, int cycleNum) {
		if(addr & 0x1000) {
			if(!this->prevA12 &&
				(cycleNum - this->prevCycleNum > 8 || this->CPUcycleCount - this->prevCPUcycleCount > 3)) {
				if(this->IRQCounter == 0 || this->IRQReload) this->IRQCounter = this->IRQLatch;
				else this->IRQCounter--;
				if(this->IRQCounter == 0 && this->IRQEnabled) this->IRQCalled = true;
				this->IRQReload = false;
			}
			this->prevCPUcycleCount = this->CPUcycleCount;
			this->prevCycleNum = cycleNum;
			this->prevA12 = true;
		} else this->prevA12 = false;
	}
};

