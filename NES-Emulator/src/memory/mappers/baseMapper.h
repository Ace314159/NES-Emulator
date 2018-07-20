#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <memory>
#include <unordered_map>


class BaseMapper {
protected:
	uint8_t temp; // Used when accessing from 0x4020 to 0x5FFF
public:
	virtual void setRAM8(uint16_t addr, uint8_t data) = 0;
	virtual uint8_t& getRAM8(uint16_t addr) = 0;
	virtual void setVRAM8(uint16_t addr, uint8_t data) = 0;
	virtual uint8_t getVRAM8(uint16_t addr) const = 0;

	std::array<uint8_t, 0x0800> internalRAM; //  0x0000 - 0x07FF
	std::array<uint8_t, 0x0008> ppuRegisters; // 0x2000 - 0x2007
	std::array<uint8_t, 0x0018> apuRegisters; // 0x4000 - 0x4017 - Not really APU registers, but almost all are

	static std::unique_ptr<BaseMapper> getMapper(uint8_t mapperID, std::vector<uint8_t>& PRG,
		std::vector<uint8_t>& CHR, uint8_t prgRamSize);
};

