#include "stdafx.h"

#include "BaseMapper.h"

#include "NROM.h"
#include "MMC1.h"
#include "UxROM.h"
#include "CNROM.h"
#include "MMC3.h"


BaseMapper::BaseMapper(iNESHeader header) : header(header) {
	auto type = static_cast<NametableMirroringType>(header.nametableMirroringType);
	this->setNametableMirroringType(type);
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

	for(size_t i = 0; i < this->nametablePtrs.size(); i++) {
		uint8_t index = indices[i / 0x400];
		this->nametablePtrs[i] = &this->nametables[index * 0x400 + i % 0x400];
	}
}
