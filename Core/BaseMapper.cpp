#include "stdafx.h"

#include "BaseMapper.h"

#include "NROM.h"
/*#include "MMC1.h"
#include "UxROM.h"
#include "CNROM.h"
#include "MMC3.h"*/


BaseMapper::BaseMapper(iNESHeader header) : header(header) {
	this->PRG.resize(header.prgRomSize * 0x4000);
	this->CHR.resize((header.chrRomSize == 0) ? 1 : header.chrRomSize * 0x2000);

	auto type = static_cast<NametableMirroringType>(header.nametableMirroringType);
	this->setNametableMirroringType(type);

	// Undefined RAM
	for(uint16_t addr = 0x4020; addr < 0x6000; addr++) 
		this->RAMPtrs[addr - 0x4020] = {&this->temp, false};

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
	/*case 0x01:
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
		break;*/
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
