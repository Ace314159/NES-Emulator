#include "MemoryHandler.h"

class InternalRAMHandler: public MemoryHandler {
private:
	std::array<uint8_t, 0x0800> internalRAM;  // 0x0000 - 0x07FF
public:
	virtual uint8_t read(uint16_t addr) override {
		return this->internalRAM[addr & 0x07FF];
	};

	virtual void write(uint16_t addr, uint8_t data) override {
		this->internalRAM[addr & 0x07FF] = data;
	};
};