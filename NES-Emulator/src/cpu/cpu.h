// Ricoh 2A03 (MOS 6502 without decimal mode) @ 1.79 MHz in NTSC (1.66 MHz in PAL)
#pragma once

#include "../memory/memory.h"

#include <cstdint>
#include <bitset>
#include <functional>
#include <array>
#include <stack>


class CPU {
public:
	//class Memory {
	//public:
	//	Memory(bool& c, int& s);

	//	std::array<uint8_t, 0x10000> arr = {};
	//	std::reference_wrapper<uint8_t> get8(uint16_t addr);
	//	uint16_t get16(uint16_t addr);
	//	void set8(uint16_t addr, uint8_t data);
	//	std::array<uint8_t, 8> buttons1 = {};
	//	int buttons1Index = 0;
	//	std::array<uint8_t, 8> buttons2 = {};
	//	int buttons2Index = 0;

	//	// PPU Related 
	//	uint8_t ppuBus = 0;
	//	uint16_t ppuRegisterRead = 0;
	//	uint16_t ppuRegisterWritten = 0;
	//	uint8_t ppuDataReadBuffer;
	//	bool ppuDataReadBufferValid = false;
	//	bool& ppuCanWrite;
	//	int& ppuScanlineNum;
	//};

	class StatusRegister {
	public:
		std::bitset<8> byte{0x24};

		std::bitset<8>::reference& N = byte[7]; // Negative
		std::bitset<8>::reference& V = byte[6]; // Overflow
												// Expansion Bit - Bit 5 is unused, always 1
		std::bitset<8>::reference& B = byte[4]; // Break - Doesn't physically exist
		std::bitset<8>::reference& D = byte[3]; // Decimal - Removed in the NES
		std::bitset<8>::reference& I = byte[2]; // Interrupt Disable
		std::bitset<8>::reference& Z = byte[1]; // Zero
		std::bitset<8>::reference& C = byte[0]; // Carry
	};


	CPU(Memory& m, int& cyc, int& scan);
	Memory& mem;

	int& CYC;
	int& SL;

	// Registers
	uint8_t A = 0; // Accumulator
	uint8_t X = 0; // Index X
	uint8_t Y = 0; // Index Y
	uint16_t PC; // Program Counter
	uint8_t S = 0xFD; // Stack Pointer
	StatusRegister P; // Processor Status Register

	// Useful variables to determine what to execute
	uint8_t effectiveAddrLow; // Acts as the low bit for the Effective, Base, and Indirect Address
	uint8_t effectiveAddrHigh;
	uint16_t effectiveAddr; // Acts as the Effective, Base, and Indirect Address
	//std::reference_wrapper<uint8_t> data = std::reference_wrapper<uint8_t>(this->mem.get8(0));
	uint8_t dataV; // data that is not a reference (value), if data changes between cycles
	uint8_t opcode;
	bool onAccumulator = false;
	bool gotData = false;
	bool branchResult;
	bool usedAbsoluteXIndirectY = false;
	bool doingIllegalOpcode = false;
	uint8_t addressingCyclesUsed;
	uint8_t cycleNum = 0;

	// Get Appropriate Addressing Mode or Operation
	void unknownOpcode();
	static void(CPU::*addressingModes[256])(uint8_t cycleNum);
	static void(CPU::*operations[256])();
	// std::function<void(uint8_t cycleNum)> getAddressingMode(uint8_t opcode);
	// std::function<void()> getOperation(uint8_t opcode);

	void emulateCycle(); // Emulates a single clock cycle

	// Addressing Modes
	void lastAddressingCycle(uint16_t newEffectiveAddr); // Run on the last addressing cycle
	void implied(uint8_t cycleNum);
	void immediate(uint8_t cycleNum);
	void absolute(uint8_t cycleNum);
	void zeroPage(uint8_t cycleNum);
	void relative(uint8_t cycleNum);
	void absoluteX(uint8_t cycleNum);
	void absoluteY(uint8_t cycleNum);
	void zeroPageX(uint8_t cycleNum);
	void zeroPageY(uint8_t cycleNum);
	void indirectX(uint8_t cycleNum);
	void indirectY(uint8_t cycleNum);

	// Flag Control
	void flagN(uint8_t result);
	void flagZ(uint8_t result);
	void flagC(uint16_t result);
	void flagV(uint8_t result);

	// Stack Operations
	void stackPush(uint8_t data);
	uint8_t stackPull();

	// Operations - Most return a trash value
	void lastOperationCycle(); // Run on the last cycle
	void interrupt(uint16_t vectorLocation);
	void RMW(uint8_t result); // Read, Modify, Write Operations
	void ADC();
	void AHX(); // Undocumented
	void ALR(); // Undocumented
	void ANC(); // Undocumented
	void AND();
	void ARR(); // Undocumented
	void ASL();
	void AXS(); // Undocumented
	void BCC();
	void BCS();
	void BEQ();
	void BIT();
	void BMI();
	void BNE();
	void BPL();
	void BRK();
	void BVC();
	void BVS();
	void CLC();
	void CLD();
	void CLI();
	void CLV();
	void CMP();
	void CPX();
	void CPY();
	void DCP(); // Undocumented
	void DEC();
	void DEX();
	void DEY();
	void EOR();
	void IGN(); // Undocumented
	void INC();
	void INX();
	void INY();
	void ISC(); // Undocumented
	void JMP();
	void JSR();
	void KIL(); // Undocumented
	void LAS(); // Undocumented
	void LAX(); // Undocumented
	void LDA();
	void LDX();
	void LDY();
	void LSR();
	void NOP();
	void ORA();
	void PHA();
	void PHP();
	void PLA();
	void PLP();
	void RLA(); // Undocumented
	void ROL();
	void ROR();
	void RRA(); // Undocumented
	void RTI();
	void RTS();
	void SAX(); // Undocumented
	void SBC();
	void SEC();
	void SED();
	void SEI();
	void SHX(); // Undocumented
	void SHY(); // Undocumented
	void SKB(); // Undocumented
	void SLO(); // Undocumented
	void SRE(); // Undocumented
	void STA();
	void STX();
	void STY();
	void TAS(); // Undocumented
	void TAX();
	void TAY();
	void TSX();
	void TXA();
	void TXS();
	void TYA();
	void XAA(); // Undocumented
};

