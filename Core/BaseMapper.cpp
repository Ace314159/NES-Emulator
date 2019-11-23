#include "stdafx.h"

#include "BaseMapper.h"

#include "NROM.h"
#include "MMC1.h"
#include "UxROM.h"
#include "CNROM.h"
#include "MMC3.h"


BaseMapper::BaseMapper(iNESHeader header) : header(header) {
	this->PRG.resize(header.prgRomSize * 0x4000);
	this->CHR.resize((header.chrRomSize == 0) ? 0x2000 : header.chrRomSize * 0x2000);

	auto type = static_cast<NametableMirroringType>(header.nametableMirroringType);
	this->setNametableMirroringType(type);

	// Undefined RAM
	for(uint16_t addr = 0x4020; addr < 0x6000; addr++) 
		this->RAMPtrs[addr - 0x4020] = {&this->temp, false};
	// WRAM
	this->setCPUMapping(0x6000, 0x7FFF, this->WRAM.data(), false);


	// CHR RAM
	if(this->header.chrRomSize == 0) {
		this->setCHRMapping(0x0000, 0x1FFF, this->CHR.data(), true);
	}
	// Palette
	for(uint16_t offset = 0; offset < 0x100; offset += 0x20) {
		this->VRAMPtrs[0x3F00 + offset] = {&this->palette[0], true};
		for(uint8_t i = 0x1; i <= 0xF; i++) this->VRAMPtrs[0x3F00 + offset + i] = {&this->palette[i], true};
		for(int i = 0; i < 4; i++) {
			this->VRAMPtrs[0x3F10 + offset + i * 4] = {&this->palette[0x00 + i * 4], true};
			for(uint8_t j = 0x1; j <= 0x3; j++)
				this->VRAMPtrs[0x3F10 + offset + i * 4 + j] = {&this->palette[0x0F + i * 3 + j], true};
		}
	}
}

std::unique_ptr<BaseMapper> BaseMapper::getMapper(iNESHeader header) {
	switch(header.mapperID) {
	case 0x00:
		return std::make_unique<NROM>(header);
		break;
	case 0x01:
		return std::make_unique<MMC1>(header);
		break;
	case 0x02:
		return std::make_unique<UxROM>(header);
		break;
	case 0x03:
		return std::make_unique<CNROM>(header);
		break;
	case 0x04:
		return std::make_unique<MMC3>(header);
		break;
	default:
		throw std::runtime_error("Mapper " + std::to_string(header.mapperID) + " is not supported!");
		break;
	}
}

void BaseMapper::setRAMHandlers(uint16_t startAddr, uint16_t endAddr, MemoryHandler& memoryHandler) {
	std::fill(this->RAMHandlers.begin() + startAddr, this->RAMHandlers.begin() + endAddr + 1, &memoryHandler);
}


void BaseMapper::setCPUMapping(uint16_t startAddr, uint16_t endAddr, uint8_t* startPtr, bool canWrite) {
	for(uint32_t addr = startAddr; addr <= endAddr; addr++) {
		this->RAMPtrs[addr - 0x4020] = {startPtr + addr - startAddr, canWrite};
	}
}

void BaseMapper::setCHRMapping(uint16_t startAddr, uint16_t endAddr, uint8_t* startPtr, bool canWrite) {
	for(uint32_t addr = startAddr; addr <= endAddr; addr++) {
		this->VRAMPtrs[addr] = {startPtr + addr - startAddr, canWrite};
	}
}


void BaseMapper::setPRGMapping(std::initializer_list<int> banks) {
	this->PRGBankSize = 0x8000 / banks.size();
	this->numPRGBanks = this->header.prgRomSize * 0x4000 / this->PRGBankSize;

	int i = 0;
	for(const int& bank : banks) this->setPRGBank(i++, bank);
}

void BaseMapper::setCHRMapping(std::initializer_list<int> banks) {
	if(this->header.chrRomSize == 0) return; // CHR RAM
	this->CHRBankSize = 0x2000 / banks.size();
	this->numCHRBanks = this->header.chrRomSize * 0x2000 / this->CHRBankSize;

	int i = 0;
	for(const int& bank : banks) this->setCHRBank(i++, bank);
}


void BaseMapper::setPRGBank(int memIndex, int bankIndex) {
	if(bankIndex < 0) bankIndex += this->numPRGBanks;
	this->setCPUMapping(0x8000 + memIndex * this->PRGBankSize, 0x8000 + (memIndex + 1) * this->PRGBankSize - 1, 
		&this->PRG[bankIndex * this->PRGBankSize], false);
}

void BaseMapper::setCHRBank(int memIndex, int bankIndex) {
	if(bankIndex < 0) bankIndex += this->numCHRBanks;
	this->setCHRMapping(memIndex * this->CHRBankSize, (memIndex + 1) * this->CHRBankSize - 1,
		&this->CHR[bankIndex * this->CHRBankSize], false);
}


void BaseMapper::setWRAMEnabled(bool enabled) {
	for(uint16_t addr = 0x6000; addr < 0x8000; addr++)
		this->RAMPtrs[addr - 0x4020].canWrite = enabled;
}


void BaseMapper::setNametableMirroringType(NametableMirroringType type) {
	this->nametableMirroringType = type;
	std::array<uint8_t, 4> indices;
	switch(type) {
	case NametableMirroringType::HORIZONTAL:
		indices = {0, 0, 1, 1};
		break;
	case NametableMirroringType::VERTICAL:
		indices = {0, 1, 0, 1};
		break;
	case NametableMirroringType::ONE_A:
		indices = {0, 0, 0, 0};
		break;
	case NametableMirroringType::ONE_B:
		indices = {1, 1, 1, 1};
		break;
	case NametableMirroringType::FOUR:
		indices = {0, 1, 2, 3};
		break;
	}

	for(size_t i = 0; i < 0x1000; i++) {
		uint8_t index = indices[i / 0x400];
		this->VRAMPtrs[i + 0x2000] = {&this->nametables[index * 0x400 + i % 0x400], true};
	}
	for(uint16_t addr = 0x3000; addr < 0x3F00; addr++) this->VRAMPtrs[addr] = this->VRAMPtrs[addr & ~0x1000];
}
