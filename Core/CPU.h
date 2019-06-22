// Ricoh 2A03 (MOS 6502 without decimal mode) @ 1.79 MHz in NTSC (1.66 MHz in PAL)
#pragma once

#include "Memory.h"


class CPU {
public:
	class StatusRegister {
	public:
		std::bitset<8> byte{0x04};
		uint8_t getByte() { return (uint8_t)(byte.to_ulong() | 0x20); }
		void setByte(uint8_t val) { byte = val & 0xCF; }

		std::bitset<8>::reference N() { return byte[7]; } // Negative
		std::bitset<8>::reference V() { return byte[6]; } // Overflow
                                                          // Expansion Bit - Bit 5 is unused, always 1
		std::bitset<8>::reference B() { return byte[4]; } // Break - Doesn't physically exist
		std::bitset<8>::reference D() { return byte[3]; } // Decimal - Removed in the NES
		std::bitset<8>::reference I() { return byte[2]; } // Interrupt Disable
		std::bitset<8>::reference Z() { return byte[1]; } // Zero
		std::bitset<8>::reference C() { return byte[0]; } // Carry
	};

	
	CPU(Memory& m);
	Memory& mem;

	// Registers
	uint8_t A = 0; // Accumulator
	uint8_t X = 0; // Index X
	uint8_t Y = 0; // Index Y
	uint16_t PC; // Program Counter
	uint8_t S = 0xFD; // Stack Pointer
	StatusRegister P; // Processor Status Register

	// Useful variables to determine what to execute
	uint8_t opcode;
	uint8_t operand;
	uint16_t effectiveAddr;
	bool prevInterruptCalled = false;
	bool interruptCalled = false;

	// Get Appropriate Addressing Mode or Operation
	static void(CPU::*addressingModes[256])();
	static void(CPU::*operations[256])();

	// Other
	void handleInterrupts();
	void OAMDMA();

	void emulateInstr(); // Emulates a single instruction

	// Memory Accesses
	uint8_t getMem(uint16_t addr);
	void setMem(uint16_t addr, uint8_t data);
	void clocked();
	// Utilities for Repetitive Stuff
	void dummyRead() { this->getMem(this->PC); }
	uint8_t readOperand() { return this->operand = this->getMem(this->effectiveAddr); }
	uint8_t readByte() { return this->getMem(this->PC++); }
	uint16_t readWord() { return this->readByte() | (this->readByte() << 8); }
	uint16_t readWord(uint16_t addr) { return this->getMem(addr) | (this->getMem(addr + 1) << 8); }
	bool pageCrossed(uint16_t addr, uint8_t operand) { return ((addr + operand) & 0xFF00) != (addr & 0xFF00); }
	bool pageCrossed(uint16_t addr, int8_t operand) { return ((addr + operand) & 0xFF00) != (addr & 0xFF00); }
	void setReg(uint8_t& reg, uint8_t val) { reg = val; this->flagN(val); this->flagZ(val); }
	void setA(uint8_t val) { this->setReg(this->A, val); };
	void setX(uint8_t val) { this->setReg(this->X, val); };
	void setY(uint8_t val) { this->setReg(this->Y, val); };

	// Addressing Modes
	void implied();
	void immediate();
	void relative();
	
	void absolute();
	void absoluteI(uint8_t& I);
	void absoluteIR(uint8_t& I);
	void absoluteX() { return this->absoluteI(this->X); }
	void absoluteXR() { return this->absoluteIR(this->X); }
	void absoluteY() { return this->absoluteI(this->Y); }
	void absoluteYR() { return this->absoluteIR(this->Y); }
	
	void zeroPage();
	void zeroPageI(uint8_t& I);
	void zeroPageX() { return this->zeroPageI(this->X); }
	void zeroPageY() { return this->zeroPageI(this->Y); }
	
	void indirectX();
	void indirectY();
	void indirectYR();

	// Flag Control
	void flagN(uint8_t result);
	void flagZ(uint8_t result);
	void flagC(uint16_t result);

	// Stack Operations
	void stackPush(uint8_t data);
	void stackPush(uint16_t data) { this->stackPush((uint8_t)(data >> 8)); this->stackPush((uint8_t)data);  }
	uint8_t stackPull();
	uint16_t stackPullWord() { return this->stackPull() | (this->stackPull() << 8); };

	// Operations
	void interrupt(bool isBRK);
	uint8_t RMW(uint8_t(CPU::*operation)(uint8_t val)); // Read, Modify, Write Operations
	void branchOp(bool branchResult); // Branch Operations
	
	void ADD(uint8_t val);
	void ADC() { this->ADD(this->readOperand()); };
	void AHX(); // Undocumented
	void ALR(); // Undocumented
	void ANC(); // Undocumented
	void AND();
	void ARR(); // Undocumented
	uint8_t ASL(uint8_t val) { return val << 1; }
	void ASL_A();
	void ASL_M();
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
	void CMP(uint8_t reg, uint8_t val);
	void CPA() { this->CMP(this->A, this->readOperand()); };
	void CPX() { this->CMP(this->X, this->readOperand()); };
	void CPY() { this->CMP(this->Y, this->readOperand()); };
	void DCP(); // Undocumented
	uint8_t dec(uint8_t val) { return val - 1; }
	void DEC();
	void DEX();
	void DEY();
	void EOR();
	void IGN(); // Undocumented
	uint8_t inc(uint8_t val) { return val + 1; }
	void INC();
	void INX();
	void INY();
	void ISC(); // Undocumented
	void JMP_Abs();
	void JMP_Ind();
	void JSR();
	void KIL(); // Undocumented
	void LAS(); // Undocumented
	void LAX(); // Undocumented
	void LDA();
	void LDX();
	void LDY();
	uint8_t LSR(uint8_t val) { return val >> 1; }
	void LSR_A();
	void LSR_M();
	void NOP();
	void ORA();
	void PHA();
	void PHP();
	void PLA();
	void PLP();
	void RLA(); // Undocumented
	uint8_t ROL(uint8_t val) { return (val << 1) | (int)this->P.C(); };
	void ROL_A();
	void ROL_M();
	uint8_t ROR(uint8_t val) { return (val >> 1) | (this->P.C() << 7); };
	void ROR_A();
	void ROR_M();
	void RRA(); // Undocumented
	void RTI();
	void RTS();
	void SAX(); // Undocumented
	void SBC() { this->ADD(~this->readOperand()); }
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

