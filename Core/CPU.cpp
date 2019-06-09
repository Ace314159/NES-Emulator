#include "stdafx.h"
#include "CPU.h"

#include <cstdio>

CPU::CPU(Memory& m) : mem(m) {}


void CPU::emulateCycle() {
	if(mem.inDMA) {
		this->OAMDMA();
		return;
	}
	
	if(this->gotData) {
		if(this->mem.inNMI) {
			this->interrupt(0xFFFA);
		} else if(this->mem.inIRQ) {
			this->interrupt(0xFFFE);
		} else {
			(this->*(this->operations[this->opcode]))();
		}
	}
	if(!this->gotData) { // gotData may change after the operation
		if(this->cycleNum == 0) {
			if(this->mem.inNMI || this->mem.inIRQ) {
				this->opcode = 0x00; // BRK opcode
			} else {
				this->opcode = this->mem.getRAM8(this->PC++);
				/*printf("%04X  %02X    A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%3d SL:%1d\n", PC - 1, opcode,
					A, X, Y, P.byte.to_ulong(), S, 0, 0);*/
			}
		} else {
			(this->*(this->addressingModes[this->opcode]))(this->cycleNum);
		}
	}

	this->cycleNum++;
}


// DMAs
void CPU::OAMDMA() {
	if((this->mem.DMAAddr >> 8) == this->mem.DMAPage) { // If DMAAdrr has not overflowed
		if(this->mem.mapper->CPUcycleCount % 2 == 0 && this->mem.DMAdoneDummy) { // Read cycles are even cycles
			this->mem.DMAVal = this->mem.getRAM8(mem.DMAAddr++); // Incremented in PPU
		}
		this->mem.DMAdoneDummy = true;
	} else {
		this->mem.DMAdoneDummy = false;
		this->mem.inDMA = false;
	}
}


// Addressing Modes - Ends on the last cycle of the current instruction
void CPU::lastAddressingCycle(uint16_t newEffectiveAddr) {
	this->addressingCyclesUsed = this->cycleNum + 1; // Equals cycleNum after increment in lastCycle()
	this->effectiveAddr = newEffectiveAddr;
	if(this->effectiveAddr != 0x4016) this->dataV = this->mem.getRAM8(this->effectiveAddr);
	this->mem.ppuRegisterRead = 0;
	this->mem.apuRegisterRead = 0;
	this->gotData = true;
	this->onAccumulator = false;
}

void CPU::implied(uint8_t cycleNum) {
	this->lastAddressingCycle(0);
	this->onAccumulator = true;
}

void CPU::immediate(uint8_t cycleNum) {
	this->lastAddressingCycle(this->PC++);
}

void CPU::absolute(uint8_t cycleNum) {
	switch(cycleNum) {
	case 1:
		this->effectiveAddrLow = mem.getRAM8(this->PC++);
		if(opcode == 0x20) this->lastAddressingCycle(0); // JSR
		break;
	case 2:
		this->effectiveAddrHigh = mem.getRAM8(this->PC++);
		if(this->opcode == 0x4C || opcode == 0x6C) this->lastAddressingCycle(0); // JMP
		break;
	case 3:
		this->lastAddressingCycle(this->effectiveAddrLow | (this->effectiveAddrHigh << 8));
		break;
	}
}

void CPU::zeroPage(uint8_t cycleNum) {
	switch(cycleNum) {
	case 1:
		this->absolute(1);
		break;
	case 2:
		this->effectiveAddrHigh = 0;
		this->absolute(3); // Calls lastAddressingCycle()
		break;
	}
}

void CPU::relative(uint8_t cycleNum) {
	uint16_t currentPage;
	switch(cycleNum) {
	case 1:
		this->effectiveAddr = this->PC++;
		(this->*(this->operations[this->opcode]))();
		if(!this->branchResult) {
			this->cycleNum++; // Increment as this cycle has finished
			this->lastOperationCycle();
			this->cycleNum--; // Decrement it now as it will get incremented in emulateCycle
		}
		break;
	case 2:
		currentPage = this->PC >> 8;
		this->PC += static_cast<int8_t>(mem.getRAM8(this->effectiveAddr));
		if(currentPage == (this->PC >> 8)) { // Page Boundary not crossed
			this->cycleNum++; // Increment as this cycle has finished
			this->lastOperationCycle();
			this->cycleNum--; // Decrement it now as it will get incremented in emulateCycle
		}
		break;
	case 3:
		// Increment page boundary, but do nothing as it has already been incremented
		this->cycleNum++; // Increment as this cycle has finished
		this->lastOperationCycle();
		this->cycleNum--; // Decrement it now as it will get incremented in emulateCycle
		break;
	}
}

void CPU::absoluteX(uint8_t cycleNum) {
	usedAbsoluteXIndirectY = true;
	switch(cycleNum) {
	case 1:
		this->absolute(1); // Gets the low byte of the effective address
		break;
	case 2:
		this->absolute(2); // Gets the high byte of the effective address
		break;
	case 3:
		this->effectiveAddr = (this->effectiveAddrLow | (this->effectiveAddrHigh << 8)) + this->X;
		if(this->effectiveAddrHigh == (this->effectiveAddr >> 8)) { // Page boundary not crossed
			this->lastAddressingCycle(this->effectiveAddr);
		} else {
			this->dataV = this->mem.getRAM8(this->effectiveAddr - 0x100);
			this->mem.ppuRegisterRead = 0;
			this->mem.apuRegisterRead = 0;
		}
		break;
	case 4:
		this->lastAddressingCycle(this->effectiveAddr); // Page already incremened
		break;
	}
}

void CPU::absoluteY(uint8_t cycleNum) {
	switch(cycleNum) {
	case 1:
		this->absolute(1); // Gets the low byte of the effective address
		break;
	case 2:
		this->absolute(2); // Gets the high byte of the effective address
		break;
	case 3:
		this->effectiveAddr = (this->effectiveAddrLow | (this->effectiveAddrHigh << 8)) + this->Y;
		if(this->effectiveAddrHigh == (this->effectiveAddr >> 8)) { // Page boundary not crossed
			this->lastAddressingCycle(this->effectiveAddr);
		} else {
			this->dataV = this->mem.getRAM8(this->effectiveAddr - 0x100);
			this->mem.ppuRegisterRead = 0;
			this->mem.apuRegisterRead = 0;
		}
		break;
	case 4:
		this->lastAddressingCycle(this->effectiveAddr); // Page already incremened
		break;
	}
}

void CPU::zeroPageX(uint8_t cycleNum) {
	switch(cycleNum) {
	case 1:
		this->zeroPage(1);
		break;
	case 2:
		this->effectiveAddr = this->effectiveAddrLow;
		break;
	case 3:
		this->effectiveAddr = (this->effectiveAddr + this->X) & 0xff;
		this->lastAddressingCycle(this->effectiveAddr);
		break;
	}
}

void CPU::zeroPageY(uint8_t cycleNum) {
	switch(cycleNum) {
	case 1:
		this->zeroPage(1);
		break;
	case 2:
		this->effectiveAddr = this->effectiveAddrLow;
		break;
	case 3:
		this->effectiveAddr = (this->effectiveAddr + this->Y) & 0xff;
		this->lastAddressingCycle(this->effectiveAddr);
		break;
	}
}

void CPU::indirectX(uint8_t cycleNum) {
	switch(cycleNum) {
	case 1:
		this->absolute(1); // Gets the low byte of the base address
		break;
	case 2:
		this->effectiveAddr = this->effectiveAddrLow; // Acts as the base address
		break;
	case 3:
		this->effectiveAddr = (this->effectiveAddr + this->X) & 0xff; // Acts as the base address
		this->effectiveAddrLow = mem.getRAM8(this->effectiveAddr); // Uses the base address
		break;
	case 4:
		this->effectiveAddrHigh = mem.getRAM8((this->effectiveAddr + 1) & 0xff); // Uses the base address
		break;
	case 5:
		this->lastAddressingCycle(this->effectiveAddrLow | (this->effectiveAddrHigh << 8));
		break;
	}
}

void CPU::indirectY(uint8_t cycleNum) {
	this->usedAbsoluteXIndirectY = true;
	switch(cycleNum) {
	case 1:
		this->absolute(1); // Gets the low byte of the indirect address
		break;
	case 2:
		this->effectiveAddr = this->effectiveAddrLow; // Acts as the indirect address
		this->effectiveAddrLow = mem.getRAM8(this->effectiveAddr); // Gets the low byte of the base address
		break;
	case 3: // Gets the high byte of the base address
		this->effectiveAddrHigh = mem.getRAM8((this->effectiveAddr + 1) & 0xff);
		break;
	case 4:
		this->absoluteY(3); // Uses the base address to form an effective address
		break;
	case 5:
		this->absoluteY(4); // An extra cycle if a page boundary was crossed
		break;
	}
}


// Flag Control
void CPU::flagN(uint8_t result) {
	this->P.N() = result >> 7;
}

void CPU::flagZ(uint8_t result) {
	this->P.Z() = result == 0;
}

void CPU::flagC(uint16_t result) {
	this->P.C() = (result >> 8) & 0x1; // Gets ninth bit
}


void CPU::flagV(uint8_t result) { // Checks if the sign of the result differs from the signs of operands
	this->P.V() = ((this->A ^ result) & (mem.getRAM8(this->effectiveAddr) ^ result)) >> 7;
}


// Stack Operations
void CPU::stackPush(uint8_t data) {
	mem.setRAM8(this->S-- | (0x01 << 8), data);
}

uint8_t CPU::stackPull() {
	return mem.getRAM8(++this->S | (0x01 << 8));
}


// Operations - Ends on the first cycle of the next instruction
void CPU::lastOperationCycle() {
	if(this->doingIllegalOpcode) return;
	this->cycleNum = 0;
	this->usedAbsoluteXIndirectY = false;
	this->gotData = false;
	this->mem.inNMI = false;
	this->mem.inIRQ = false;
	this->mem.inDMA = false;
	if(this->mem.NMICalled) mem.inNMI = true;
	else if(this->mem.IRQCalled && !this->P.I()) {
		this->mem.inIRQ = true;
		this->mem.IRQCalled = false;
	}
	this->mem.NMICalled = false;
}

void CPU::interrupt(uint16_t vectorLocation) {
	switch(cycleNum - this->addressingCyclesUsed) {
	case 0:
		this->mem.getRAM8(this->PC);
		if(!this->mem.inNMI && !this->mem.inIRQ) this->PC++;
		break;
	case 1:
		this->stackPush(this->PC >> 8);
		break;
	case 2:
		this->stackPush(this->PC & 0xff);
		break;
	case 3:
		// Bit 4 is only set if interrupt caused by BRK
		this->stackPush((this->P.byte.to_ulong() & 0xff) | ((!this->mem.inNMI && !this->mem.inIRQ) << 4)); 
		break;
	case 4:
		this->P.I() = 1;
		this->effectiveAddrLow = mem.getRAM8(vectorLocation); // Low byte of interrupt vector
		break;
	case 5:
		this->effectiveAddrHigh = mem.getRAM8(vectorLocation + 1); // High byte of interrupt vector
		break;
	case 6:
		this->PC = this->effectiveAddrLow | (this->effectiveAddrHigh << 8);
		this->lastOperationCycle();
		this->mem.inIRQ = false;
		break;
	}
}

void CPU::RMW(uint8_t result) {
	switch(this->cycleNum - this->addressingCyclesUsed) {
	case 0:
		mem.setRAM8(this->effectiveAddr, this->dataV); // Changes value of data if it changed between cycles?
		break;
	case 1:
		mem.setRAM8(this->effectiveAddr, result);
		this->flagN(result);
		this->flagZ(result);
		break;
	case 2:
		this->lastOperationCycle();
		break;
	}
}

void CPU::ADC() { // Change SBC if changing ADC
	uint16_t result = this->A + mem.getRAM8(this->effectiveAddr) + this->P.C();
	this->flagN(result & 0xff);
	this->flagZ(result & 0xff);
	this->flagC(result);
	this->flagV(result & 0xff);
	this->A = result & 0xff;
	this->lastOperationCycle();
}

void CPU::AHX() { // Undocumented
	mem.setRAM8(this->effectiveAddr, this->A & this->X & (((this->effectiveAddr >> 8) & 0xff) + 1));
	this->lastOperationCycle();
}

void CPU::ALR() { // Undocumented
	this->AND();
	this->onAccumulator = true;
	this->LSR();
}

void CPU::ANC() { // Undocumented
	this->AND();
	this->P.C() = this->P.N();
}

void CPU::AND() {
	this->A = this->A & mem.getRAM8(this->effectiveAddr);
	this->flagN(this->A);
	this->flagZ(this->A);
	this->lastOperationCycle();
}

void CPU::ARR() { // Undocumented
	this->AND();
	this->onAccumulator = true;
	this->ROR();
	this->P.C() = (this->A >> 6) & 0x1;
	this->P.V() = ((this->A >> 6) ^ (this->A >> 5)) & 0x1;
}

void CPU::ASL() {
	if(this->onAccumulator) {
		uint8_t result = this->A << 1;
		this->P.C() = this->A >> 7;
		this->A = result;
		this->flagN(result);
		this->flagZ(result);
		this->lastOperationCycle();
	} else {
		if(this->cycleNum - this->addressingCyclesUsed == 1) this->P.C() = this->dataV >> 7;
		this->RMW(this->dataV << 1);
	}
}

void CPU::AXS() { // Undocumented
	uint16_t result = (this->A & this->X) - mem.getRAM8(this->effectiveAddr);
	this->X = result & 0xff;
	this->flagN(this->X);
	this->flagZ(this->X);
	this->flagC(result);
	this->P.C() = !this->P.C();
	this->lastOperationCycle();
}

void CPU::BCC() {
	this->branchResult = !this->P.C();
}

void CPU::BCS() {
	this->branchResult = this->P.C();
}

void CPU::BEQ() {
	this->branchResult = this->P.Z();
}

void CPU::BIT() {
	uint8_t result = this->A & mem.getRAM8(this->effectiveAddr);
	this->flagZ(result);
	this->P.N() = mem.getRAM8(this->effectiveAddr) >> 7;
	this->P.V() = (mem.getRAM8(this->effectiveAddr) >> 6) & 0x1;
	this->lastOperationCycle();
}

void CPU::BMI() {
	this->branchResult = this->P.N();
}

void CPU::BNE() {
	this->branchResult = !this->P.Z();
}

void CPU::BPL() {
	this->branchResult = !this->P.N();
}

void CPU::BRK() {
	this->interrupt(0xFFFE);
}

void CPU::BVC() {
	this->branchResult = !this->P.V();
}

void CPU::BVS() {
	this->branchResult = this->P.V();
}

void CPU::CLC() {
	this->P.C() = 0;
	this->lastOperationCycle();
}

void CPU::CLD() {
	this->P.D() = 0;
	this->lastOperationCycle();
}

void CPU::CLI() {
	this->P.I() = 0;
	this->mem.inIRQ = false;
	this->lastOperationCycle();
}

void CPU::CLV() {
	this->P.V() = 0;
	this->lastOperationCycle();
}

void CPU::CMP() {
	uint16_t result = this->A - mem.getRAM8(this->effectiveAddr);
	this->flagN(result & 0xff);
	this->flagZ(result & 0xff);
	this->flagC(result);
	this->P.C() = !this->P.C();
	this->lastOperationCycle();
}

void CPU::CPX() {
	uint16_t result = this->X - mem.getRAM8(this->effectiveAddr);
	this->flagN(result & 0xff);
	this->flagZ(result & 0xff);
	this->flagC(result);
	this->P.C() = !this->P.C();
	this->lastOperationCycle();
}

void CPU::CPY() {
	uint16_t result = this->Y - mem.getRAM8(this->effectiveAddr);
	this->flagN(result & 0xff);
	this->flagZ(result & 0xff);
	this->flagC(result);
	this->P.C() = !this->P.C();
	this->lastOperationCycle();
}

void CPU::DCP() { // Undocumented
	this->DEC();
	this->doingIllegalOpcode = true;
	if(this->cycleNum - this->addressingCyclesUsed == 1) this->CMP();
	this->doingIllegalOpcode = false;

}

void CPU::DEC() {
	this->RMW(this->dataV - 1);
}

void CPU::DEX() {
	this->X--;
	this->flagN(this->X);
	this->flagZ(this->X);
	this->lastOperationCycle();
}

void CPU::DEY() {
	this->Y--;
	this->flagN(this->Y);
	this->flagZ(this->Y);
	this->lastOperationCycle();
}

void CPU::EOR() {
	this->A ^= mem.getRAM8(this->effectiveAddr);
	this->flagN(this->A);
	this->flagZ(this->A);
	this->lastOperationCycle();
}

void CPU::IGN() {
	this->NOP();
}

void CPU::INC() {
	this->RMW(this->dataV + 1);
}

void CPU::INX() {
	this->X++;
	this->flagN(this->X);
	this->flagZ(this->X);
	this->lastOperationCycle();
}

void CPU::INY() {
	this->Y++;
	this->flagN(this->Y);
	this->flagZ(this->Y);
	this->lastOperationCycle();
}

void CPU::ISC() { // Undocumented
	this->INC();
	this->doingIllegalOpcode = true;
	if(this->cycleNum - this->addressingCyclesUsed == 1) this->SBC();
	this->doingIllegalOpcode = false;
}

void CPU::JMP() {
	if(opcode == 0x4C) { // JMP with absolute addressing
		this->PC = this->effectiveAddrLow | (this->effectiveAddrHigh << 8);
		this->lastOperationCycle();
	} else { // JMP with indirect addressing
		switch(this->cycleNum - this->addressingCyclesUsed) {
		case 0:
			this->PC = this->effectiveAddrLow | (this->effectiveAddrHigh << 8); // Indirect Address
			this->effectiveAddrLow = mem.getRAM8(this->PC);
			break;
		case 1:
			// Make sure that page boundaries are accounted for - 0x02ff + 1 should be 0x0200
			this->effectiveAddrHigh = mem.getRAM8((this->PC & 0xff00) | ((this->PC + 1) & 0xff));
			break;
		case 2:
			this->PC = this->effectiveAddrLow | (this->effectiveAddrHigh << 8);
			this->lastOperationCycle();
			break;
		}
	}
}

void CPU::JSR() {
	switch(this->cycleNum - this->addressingCyclesUsed) {
	case 0:
		// Doesn't do anything - Not sure?
		break;
	case 1:
		this->stackPush(this->PC >> 8);
		break;
	case 2:
		this->stackPush(this->PC & 0xff);
		break;
	case 3:
		this->effectiveAddrHigh = mem.getRAM8(this->PC++);
		break;
	case 4:
		this->PC = this->effectiveAddrLow | (this->effectiveAddrHigh << 8);
		this->lastOperationCycle();
		break;
	}
}

void CPU::KIL() { // Undocumented
	throw std::runtime_error("KIL Called!");
}

void CPU::LAS() { // Undocumented
	this->S &= mem.getRAM8(this->effectiveAddr);
	this->A = this->S;
	this->X = this->S;
	this->flagN(this->S);
	this->flagZ(this->S);
	this->lastOperationCycle();
}

void CPU::LAX() { // Undocumented
	this->LDA();
	this->LDX();
}

void CPU::LDA() {
	this->A = mem.getRAM8(this->effectiveAddr);
	this->flagN(this->A);
	this->flagZ(this->A);
	this->lastOperationCycle();
}

void CPU::LDX() {
	this->X = mem.getRAM8(this->effectiveAddr);
	this->flagN(this->X);
	this->flagZ(this->X);
	this->lastOperationCycle();
}

void CPU::LDY() {
	this->Y = mem.getRAM8(this->effectiveAddr);
	this->flagN(this->Y);
	this->flagZ(this->Y);
	this->lastOperationCycle();
}

void CPU::LSR() {
	if(this->onAccumulator) {
		uint8_t result = this->A >> 1;
		this->P.C() = this->A & 0x1;
		this->A = result;
		this->flagN(result);
		this->flagZ(result);
		this->lastOperationCycle();
	} else {
		if(this->cycleNum - this->addressingCyclesUsed == 1) this->P.C() = this->dataV & 0x1;
		this->RMW(this->dataV >> 1);
	}
}

void CPU::NOP() {
	this->lastOperationCycle();
}

void CPU::ORA() {
	this->A |= mem.getRAM8(this->effectiveAddr);
	this->flagN(this->A);
	this->flagZ(this->A);
	this->lastOperationCycle();
}

void CPU::PHA() {
	if(this->cycleNum - this->addressingCyclesUsed == 0) {
		this->stackPush(this->A);
	} else {
		this->lastOperationCycle();
	}
}

void CPU::PHP() {
	if(this->cycleNum - this->addressingCyclesUsed == 0) {
		this->stackPush((this->P.byte.to_ulong() & 0xff) | (1 << 4)); // Bit 4 is set
	} else {
		this->lastOperationCycle();
	}
}

void CPU::PLA() {
	if(this->cycleNum - this->addressingCyclesUsed == 1) {
		this->A = this->stackPull();
		this->flagN(this->A);
		this->flagZ(this->A);
	} else if(this->cycleNum - this->addressingCyclesUsed == 2) {
		this->lastOperationCycle();
	}
}

void CPU::PLP() {
	if(this->cycleNum - this->addressingCyclesUsed == 1) {
		this->P.byte = (this->stackPull() & ~(1 << 4)) | (1 << 5); // Bits 4(clear) and 5(set) are ignored
	} else if(this->cycleNum - this->addressingCyclesUsed == 2) {
		this->lastOperationCycle();
	}
}

void CPU::RLA() { // Undocumented
	this->ROL();
	this->doingIllegalOpcode = true;
	if(this->cycleNum - this->addressingCyclesUsed == 1) this->AND();
	this->doingIllegalOpcode = false;
}

void CPU::ROL() {
	if(this->opcode == 0x2A) {
		uint8_t result = (this->A << 1) | static_cast<int>(this->P.C());
		this->P.C() = this->A >> 7;
		this->A = result;
		this->flagN(result);
		this->flagZ(result);
		this->lastOperationCycle();
	} else {
		uint8_t oldData = mem.getRAM8(this->effectiveAddr);
		this->RMW((this->dataV << 1) | static_cast<int>(this->P.C()));
		if(this->cycleNum - this->addressingCyclesUsed == 1) this->P.C() = oldData >> 7;
	}
}

void CPU::ROR() {
	if(this->onAccumulator) {
		uint8_t result = (this->A >> 1) | (this->P.C() << 7);
		this->P.C() = this->A & 0x1;
		this->A = result;
		this->flagN(result);
		this->flagZ(result);
		this->lastOperationCycle();
	} else {
		uint8_t oldData = mem.getRAM8(this->effectiveAddr);
		this->RMW((this->dataV >> 1) | (this->P.C() << 7));
		if(this->cycleNum - this->addressingCyclesUsed == 1) this->P.C() = oldData & 0x1;
	}
}

void CPU::RRA() { // Undocumented
	this->ROR();
	this->doingIllegalOpcode = true;
	if(this->cycleNum - this->addressingCyclesUsed == 1) this->ADC();
	this->doingIllegalOpcode = false;
}

void CPU::RTI() {
	switch(this->cycleNum - this->addressingCyclesUsed) {
	case 0:
		this->mem.getRAM8(this->PC);
		break;
	case 1:
		this->P.byte = (this->stackPull() & ~(1 << 4)) | (1 << 5); // Bits 4(clear) and 5(set) are ignored
		break;
	case 2:
		this->PC = this->stackPull();
		this->PC &= 0xff;
		break;
	case 3:
		this->PC |= (this->stackPull() << 8);
		break;
	case 4:
		this->lastOperationCycle();
		break;
	}
}

void CPU::RTS() {
	switch(this->cycleNum - this->addressingCyclesUsed) {
	case 0:
		this->mem.getRAM8(this->PC);
		break;
	case 1:
		this->PC = this->stackPull();
		this->PC &= 0xff;
		break;
	case 2:
		this->PC |= (this->stackPull() << 8);
		break;
	case 3:
		this->PC++;
		break;
	case 4:
		this->lastOperationCycle();
		break;
	}
}

void CPU::SAX() { // Undocumented
	mem.setRAM8(this->effectiveAddr, (this->A & this->X));
	this->lastOperationCycle();
}

void CPU::SBC() { // ADC with flipped data
	uint8_t& ref = *this->mem.getRAMAddrData(this->effectiveAddr).second;
	ref = ~ref;
	this->ADC();
	ref = ~ref; // Flip back to original
}

void CPU::SEC() {
	this->P.C() = 1;
	this->lastOperationCycle();
}

void CPU::SED() {
	this->P.D() = 1;
	this->lastOperationCycle();
}

void CPU::SEI() {
	this->P.I() = 1;
	if(this->mem.IRQCalled) this->mem.inIRQ = true;
	this->lastOperationCycle();
}

void CPU::SHX() { // Undocumented
	uint8_t high = this->effectiveAddr >> 8;
	mem.setRAM8(((this->X & (high + 1)) << 8) | (this->effectiveAddr & 0xff), this->X & (high + 1));
	this->lastOperationCycle();
}

void CPU::SHY() { // Undocumented
	uint8_t high = this->effectiveAddr >> 8;
	mem.setRAM8( ((this->Y & (high + 1)) << 8) | (this->effectiveAddr & 0xff), this->Y & (high + 1));
	this->lastOperationCycle();
}

void CPU::SKB() { // Undocumented
	this->NOP();
}

void CPU::SLO() { // Undocumented
	this->ASL();
	this->doingIllegalOpcode = true;
	if(this->cycleNum - this->addressingCyclesUsed == 1) this->ORA();
	this->doingIllegalOpcode = false;
}

void CPU::SRE() { // Undocumented
	this->LSR();
	this->doingIllegalOpcode = true;
	if(this->cycleNum - this->addressingCyclesUsed == 1) this->EOR();
	this->doingIllegalOpcode = false;
}

void CPU::STA() {
	if(((this->opcode == 0x9D || this->opcode == 0x99) && this->cycleNum == 4) || // Absolute Indexed
		(this->opcode == 0x91 && this->cycleNum == 5)) { // Indirect, Y
		return; // Need to use up one more cycle - Special Cases
	}
	mem.setRAM8(this->effectiveAddr, this->A);
	this->lastOperationCycle();
}

void CPU::STX() {
	mem.setRAM8(this->effectiveAddr, this->X);
	this->lastOperationCycle();
}

void CPU::STY() {
	mem.setRAM8(this->effectiveAddr, this->Y);
	this->lastOperationCycle();
}

void CPU::TAS() {
	this->S = this->A & this->X;
	mem.setRAM8(this->effectiveAddr, this->S & (((this->effectiveAddr >> 8) & 0xff) + 1));
	this->lastOperationCycle();
}

void CPU::TAX() {
	this->X = this->A;
	this->flagN(this->X);
	this->flagZ(this->X);
	this->lastOperationCycle();
}

void CPU::TAY() {
	this->Y = this->A;
	this->flagN(this->Y);
	this->flagZ(this->Y);
	this->lastOperationCycle();
}

void CPU::TYA() {
	this->A = this->Y;
	this->flagN(this->A);
	this->flagZ(this->A);
	this->lastOperationCycle();
}

void CPU::TSX() {
	this->X = this->S;
	this->flagN(this->X);
	this->flagZ(this->X);
	this->lastOperationCycle();
}

void CPU::TXA() {
	this->A = this->X;
	this->flagN(this->A);
	this->flagZ(this->A);
	this->lastOperationCycle();
}

void CPU::TXS() {
	this->S = this->X;
	this->lastOperationCycle();
}

void CPU::XAA() {
	this->TXA();
	this->AND();
}


//// Memory Functions
//CPU::Memory::Memory(bool& c, int& s) : ppuCanWrite(c), ppuScanlineNum(s) {}
//
//std::reference_wrapper<uint8_t> CPU::Memory::get8(uint16_t addr) {
//	if(addr > 0x7FFF && addr < 0x2000) return this->arr[addr & 0x07FF]; // Memory Mirroring
//	else if(addr > 0x1FFF && addr < 0x4000) { // PPU Register Mirroring
//		addr &= 0xE007;
//		if(addr == 0x2000 || addr == 0x2001 || addr == 0x2003 || addr == 0x2005 || addr == 0x2006) {
//			return this->ppuBus; // PPU Write Only Registers
//		} else if(addr == 0x2002) {
//			this->ppuRegisterRead = 0x2002;
//			return arr[0x2002] = (this->arr[0x2002] & ~0x1F) | (this->ppuBus & 0x1F);
//		} else if(addr == 0x2007) {
//			this->ppuRegisterRead = 0x2007;
//			if(this->ppuDataReadBufferValid) return this->ppuDataReadBuffer;
//			this->ppuDataReadBufferValid = true;
//			return this->arr[0x2007];
//		}
//		this->ppuRegisterRead = addr;
//		return this->arr[addr];
//	}
//	else if(addr == 0x4014) return this->ppuBus; // PPU Write Only Register
//	else if(addr == 0x4016) return this->buttons1[this->buttons1Index++ % 8];
//	else if(addr == 0x4017) return this->buttons2[this->buttons2Index++ % 8];
//	return this->arr[addr];
//}
//
//uint16_t CPU::Memory::get16(uint16_t addr) {
//	return this->get8(addr) | (this->get8(addr + 1) << 8);
//}
//
//void CPU::Memory::set8(uint16_t addr, uint8_t data) {
//	if(addr > 0x7FFF && addr < 0x2000) { // Memory Mirroring
//		this->arr[addr & 0x07FF] = data;
//		return;
//	}
//	else if(addr > 0x1FFF && addr < 0x4000) { // PPU Register Mirroring
//		addr &= 0xE007;
//		if(((addr == 0x2000 || addr == 0x2001 || addr == 0x2005 || addr == 0x2006) && this->ppuCanWrite) ||
//			addr != 0x2002) { // PPU Write Registers
//			if(addr == 0x2004 && (this->ppuScanlineNum < 240 || this->ppuScanlineNum == 261))
//				return; // Ignore writes to OAM Data during rendering
//			this->arr[addr] = this->ppuBus = data;
//			// Set low 5 bits of PPUSTATUS
//			this->arr[0x2002] = (this->arr[0x2002] & ~(0x1F)) | (this->ppuBus & 0x1F);
//
//			this->ppuRegisterWritten = addr;
//		}
//		return;
//	}
//	else if(addr == 0x4014) { // PPU Write Only Register
//		this->arr[0x4014] = this->ppuBus = data;
//		// Set low 5 bits of PPUSTATUS
//		//this->arr[0x2002] = (this->arr[0x2002] & ~(0x1F)) | (this->ppuBus & 0x1F);
//		this->ppuRegisterWritten = 0x4014;
//		return;
//	}
//	this->arr[addr] = data;
//}
