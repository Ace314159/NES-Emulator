#pragma once


struct iNESHeader {
	// Found in ROM
	// uint8_t *NES; // NES in ASCII, followed by MS-DOS end-of-file (0x4E, 0x45, 0x53, 0x1A)
	uint8_t prgRomSize; // in 16K units
	uint8_t chrRomSize; // in 8K units, if 0 it means CHR RAM is used
	uint8_t flag6;
	uint8_t flag7;
	uint8_t prgRamSize; // in 8K units, if 0 it means 8K
	uint8_t flag9;
	uint8_t flag10;
	// uint8_t *z;

	// Calculated
	int nametableMirroringType;
	bool containsSRAM;
	bool containsTrainer;
	uint8_t mapperID;
	bool isVS;
	bool isPC10;
	bool isNES2;
	// TODO: Parse Flags 9 - 15 and add support for iNES 2.0

	void init(const std::array<uint8_t, 16>& headerData);
};
