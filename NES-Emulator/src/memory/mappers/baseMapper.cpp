#include "baseMapper.h"
#include "nrom.h"

#include <string>

std::unique_ptr<BaseMapper> BaseMapper::getMapper(uint8_t mapperID, std::vector<uint8_t>& PRG,
	std::vector<uint8_t>& CHR, uint8_t prgRamSize) {

	switch(mapperID) {
	case 0:
		return std::make_unique<NROM>(PRG, CHR);
		break;
	default:
		throw std::runtime_error("Mapper " + std::to_string(mapperID) + " is not supported!");
	}
}

