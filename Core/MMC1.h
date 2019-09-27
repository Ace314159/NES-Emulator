#pragma once

#include "BaseMapper.h"


class MMC1 : public BaseMapper {
private:
	// Registers
	// CTRL
	uint8_t PRGBankMode = 0;
	bool CHRBankMode = 0;
	// Regular
	uint8_t load = 0;
	uint8_t CHRBank0 = 0;
	uint8_t CHRBank1 = 0;
	uint8_t PRGBank = 0;

	// Useful Variables
	unsigned int prevCPUCycleCount = 0;
	uint8_t loadCount = 0;

	void setPRGBank(uint8_t bank) {
		this->PRGBank = bank;
		switch(this->PRGBankMode) {
			case 0: case 1: this->setPRGMapping({this->PRGBank & ~0x1}); break;
			case 2: this->setPRGMapping({0, this->PRGBank}); break;
			case 3: this->setPRGMapping({this->PRGBank, -1}); break;
		}
	}

	void setCHRBanks(uint8_t bank0, uint8_t bank1) {
		this->CHRBank0 = bank0;
		this->CHRBank1 = bank1;
		switch(this->CHRBankMode) {
			case 0: this->setCHRMapping({this->CHRBank0 & ~0x1}); break;
			case 1: this->setCHRMapping({this->CHRBank0, this->CHRBank1}); break;
		}
	}

	void setCTRL(uint8_t CTRL) {
		this->CHRBankMode = (CTRL >> 4) & 0x1;
		this->PRGBankMode = (CTRL >> 2) & 0x3;

		this->setWRAMEnabled(!(CTRL >> 4));
		switch(CTRL & 0x3) {
			case 0: this->setNametableMirroringType(NametableMirroringType::ONE_A); break;
			case 1: this->setNametableMirroringType(NametableMirroringType::ONE_B); break;
			case 2: this->setNametableMirroringType(NametableMirroringType::VERTICAL); break;
			case 3: this->setNametableMirroringType(NametableMirroringType::HORIZONTAL);  break;
		}
		this->setPRGBank(this->PRGBank);
		this->setCHRBanks(this->CHRBank0, this->CHRBank1);
	};
public:
	MMC1(iNESHeader header) : BaseMapper(header) {
		this->setCTRL(0x0C);
	};

	virtual uint8_t read(uint16_t addr) override {
		return *this->RAMPtrs[addr - 0x4020].data;
	};

	virtual void write(uint16_t addr, uint8_t data) override {
		if(this->RAMPtrs[addr - 0x4020].canWrite)* this->RAMPtrs[addr - 0x4020].data = data;
		if(addr < 0x8000) return;

		if(this->CPUcycleCount - this->prevCPUCycleCount <= 1) return;
		if(data >> 7) {
			this->loadCount = 0;
			this->load = 0;
		} else {
			this->load |= (data & 0x01) << this->loadCount;
			this->loadCount++;
			if(this->loadCount == 5) {
				switch(addr & 0xE000) {
					case 0x8000: this->setCTRL(this->load); break;
					case 0xA000: this->setCHRBanks(this->load, this->CHRBank1); break;
					case 0xC000: this->setCHRBanks(this->CHRBank0, this->load);  break;
					case 0xE000: this->setPRGBank(this->load & 0x0F); break;
				}

				this->load = 0;
				this->loadCount = 0;
			}
		}
		this->prevCPUCycleCount = this->CPUcycleCount;
	};
};
