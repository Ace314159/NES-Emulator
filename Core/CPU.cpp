#include "stdafx.h"
#include "CPU.h"

#include "PPU.h"
#include "APU.h"

CPU::CPU(Memory& m) : mem(m) {}


void CPU::emulateInstr() {
//#ifdef _DEBUG
	/*if(this->mem.mapper->CPUcycleCount == 180402)
		std::cout << "";
	printf("%04X  A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%-3d SL:%-3d  %u\n", PC,
		A, X, Y, P.getByte() & ~0x20, S, this->mem.ppu.cycleNum, this->mem.ppu.scanlineNum,
		this->mem.mapper->CPUcycleCount);
	/*printf("%04X  %02X    A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%3d SL:%1d\n", PC, mem.getRAM8(PC),
		A, X, Y, P.getByte(), S, this->mem.ppu.cycleNum, this->mem.ppu.scanlineNum);*/
//#endif
	this->opcode = this->getMem(this->PC++);
	(this->*(this->addressingModes[this->opcode]))();
	(this->*(this->operations[this->opcode]))();
	this->handleInterrupts();
}


// Other
void CPU::handleInterrupts() {
	// The status of the interrupt lines at the end of the second-to-last cycle matters
	if(this->prevInterruptCalled) {
		this->interrupt();
		this->mem.mapper->IRQCalled = false;
	}
}

void CPU::OAMDMA() {
	if(this->mem.mapper->CPUcycleCount % 2 == 0) this->dummyRead();
	this->dummyRead();

	uint16_t page = this->mem.ppu.OAMDMA << 8;
	for(int i = 0; i < 0x100; i++) {
		uint8_t val = this->getMem(page | i);
		this->setMem(0x2004, val);
	}
}


// Memory Accesses
uint8_t CPU::getMem(uint16_t addr) {
	this->clocked();
	return this->mem.getRAM8(addr);;
}

void CPU::setMem(uint16_t addr, uint8_t data) {
	this->clocked();
	this->mem.setRAM8(addr, data);
}

void CPU::clocked() {
	this->prevInterruptCalled = this->interruptCalled;

	this->mem.mapper->CPUcycleCount++;

	this->mem.ppu.actualCycleNum += 3;
	if(this->mem.ppu.actualCycleNum > 340) {
		this->mem.ppu.actualCycleNum -= 340;
		this->mem.ppu.actualScanlineNum = ((this->mem.ppu.actualScanlineNum + 1 + 1) % (261 + 1)) - 1;
	}
	if(this->mem.ppu.actualScanlineNum == 240 && this->mem.ppu.actualCycleNum <= 3 ||
		this->mem.ppu.actualScanlineNum == 241 && this->mem.ppu.actualCycleNum <= 3)
		this->mem.ppu.catchUp();

	this->mem.apu.emulateCycle();

	this->interruptCalled = this->mem.NMICalled || (this->mem.mapper->IRQCalled && !this->P.I());
}



// Operations
void CPU::interrupt() {
	this->dummyRead(); // fetch opcode (and discard it - $00 (BRK) is forced into the opcode register instead)
	this->dummyRead(); // read next instruction byte
					   // (actually the same as above, since PC increment is suppressed. Also discarded.)
	
	this->stackPush(this->PC);
	this->stackPush(this->P.getByte());
	if(this->mem.NMICalled) {
		this->PC = this->readWord(0xFFFA);
		this->mem.NMICalled = false;
		this->P.I() = true;
	} else {
		this->PC = this->readWord(0xFFFE);
		this->P.I() = true;
	}
}

uint8_t CPU::RMW(uint8_t(CPU::*operation)(uint8_t val)) {
	this->readOperand();
	this->setMem(this->effectiveAddr, this->operand);
	uint8_t result = (this->*operation)(this->operand);
	this->flagN(result);
	this->flagZ(result);
	this->setMem(this->effectiveAddr, result);
	return result;
}

void CPU::branchOp(bool branchResult) {
	int8_t offset = this->readOperand();
	if(!branchResult) return;
	this->dummyRead();
	if(this->pageCrossed(this->PC, offset)) {
		uint16_t currentPage = this->PC & 0xFF00;
		this->getMem(((this->PC + offset) & ~0xFF00) | currentPage);
	}
	this->PC += offset;
}

void CPU::ADD(uint8_t val) {
	uint16_t result = this->A + val + this->P.C();
	this->flagC(result);
	this->P.V() = (~(this->A ^ val) & (this->A ^ result)) & 0x80;
	this->setA((uint8_t)result);
}

void CPU::AHX() { // Undocumented
	this->setMem(this->effectiveAddr, this->A & this->X & ((this->effectiveAddr >> 8) + 1));
}

void CPU::ALR() { // Undocumented
	this->AND();
	this->LSR_A();
}

void CPU::ANC() { // Undocumented
	this->AND();
	this->P.C() = this->P.N();
}

void CPU::AND() {
	this->setA(this->A & this->readOperand());
}

void CPU::ARR() { // Undocumented
	this->AND();
	this->ROR_A();
	this->P.C() = this->A & 0x40;
	this->P.V() = ((this->A >> 6) ^ (this->A >> 5)) & 0x01;
}

void CPU::ASL_A() {
	uint8_t result = this->ASL(this->A);
	this->P.C() = this->A & 0x80;
	this->setA(result);
}

void CPU::ASL_M() {
	this->RMW(&CPU::ASL);
	this->P.C() = this->operand & 0x80; // Operand is before operation
}

void CPU::AXS() { // Undocumented
	uint16_t result = (this->A & this->X) - this->readOperand();
	this->setX((uint8_t)result);
	this->flagC(result);
	this->P.C() = !this->P.C();
}

void CPU::BCC() {
	this->branchOp(!this->P.C());
}

void CPU::BCS() {
	this->branchOp(this->P.C());
}

void CPU::BEQ() {
	this->branchOp(this->P.Z());
}

void CPU::BIT() {
	this->readOperand();
	uint8_t result = this->A & this->operand;
	this->flagZ(result);
	this->flagN(this->operand);
	this->P.V() = (this->operand >> 6) & 0x01;
}

void CPU::BMI() {
	this->branchOp(this->P.N());
}

void CPU::BNE() {
	this->branchOp(!this->P.Z());
}

void CPU::BPL() {
	this->branchOp(!this->P.N());
}

void CPU::BRK() {
	this->PC++; // Read without increment during dummyRead of implied
	this->stackPush(this->PC);

	uint8_t flags = this->P.getByte() | 0x10; // Sets B Flag
	uint16_t vectorLocation = this->mem.NMICalled ? 0xFFFA : 0xFFFE;

	this->stackPush(flags);
	this->P.I() = true;
	this->PC = this->readWord(vectorLocation);

	this->prevInterruptCalled = false;
}

void CPU::BVC() {
	this->branchOp(!this->P.V());
}

void CPU::BVS() {
	this->branchOp(this->P.V());
}

void CPU::CLC() {
	this->P.C() = 0;
}

void CPU::CLD() {
	this->P.D() = 0;
}

void CPU::CLI() {
	this->P.I() = 0;
	//this->mem.mapper->IRQCalled = false; - Commented for MMC3 IRQ to work
}

void CPU::CLV() {
	this->P.V() = 0;
}

void CPU::CMP(uint8_t reg, uint8_t val) {
	uint16_t result = reg - val;
	this->flagN((uint8_t)result);
	this->flagZ((uint8_t)result);
	this->flagC(result);
	this->P.C() = !this->P.C();
}

void CPU::DCP() { // Undocumented
	this->DEC();
	this->CMP(this->A, this->operand - 1); // Operand - 1 occurs in DEC
}

void CPU::DEC() {
	this->RMW(&CPU::dec);
}

void CPU::DEX() {
	this->setX(this->X - 1);
}

void CPU::DEY() {
	this->setY(this->Y - 1);
}

void CPU::EOR() {
	this->setA(this->A ^ this->readOperand());
}

void CPU::IGN() {
	this->NOP();
}

void CPU::INC() {
	this->RMW(&CPU::inc);
}

void CPU::INX() {
	this->setX(this->X + 1);
}

void CPU::INY() {
	this->setY(this->Y + 1);
}

void CPU::ISC() { // Undocumented
	this->INC();
	this->ADD((this->operand + 1) ^ 0xFF); // Operand + 1 occurs in INC
}

void CPU::JMP_Abs() {
	this->PC = this->effectiveAddr;
}

void CPU::JMP_Ind() {
	uint16_t highAddr = (this->effectiveAddr & 0xFF00) | ((this->effectiveAddr + 1) & 0xFF);
	this->PC = this->getMem(this->effectiveAddr) | (this->getMem(highAddr) << 8);
}

void CPU::JSR() { // Order of getting bytes is not accurate, but it shouldn't matter
	this->dummyRead();
	this->stackPush((uint16_t)(this->PC - 1));
	this->PC = this->effectiveAddr;
}

void CPU::KIL() { // Undocumented
	throw std::runtime_error("KIL Called!");
}

void CPU::LAS() { // Undocumented
	this->S &= this->readOperand();
	this->A = this->S;
	this->setX(this->S);
}

void CPU::LAX() { // Undocumented
	this->readOperand();
	this->A = this->operand;
	this->setX(this->operand);
}

void CPU::LDA() {
	this->setA(this->readOperand());
}

void CPU::LDX() {
	this->setX(this->readOperand());
}

void CPU::LDY() {
	this->setY(this->readOperand());
}

void CPU::LSR_A() {
	uint8_t result = this->LSR(this->A);
	this->P.C() = this->A & 0x01;
	this->setA(result);
}

void CPU::LSR_M() {
	this->RMW(&CPU::LSR);
	this->P.C() = this->operand & 0x01; // Operand is beefore operation
}


void CPU::NOP() {
	return;
}

void CPU::ORA() {
	this->setA(this->A | this->readOperand());
}

void CPU::PHA() {
	this->stackPush(this->A);
}

void CPU::PHP() {
	this->stackPush((uint8_t)(this->P.getByte() | 0x10)); // Bit 4 Set
}

void CPU::PLA() {
	this->dummyRead();
	this->setA(this->stackPull());
}

void CPU::PLP() {
	this->dummyRead();
	this->P.setByte(this->stackPull()); // Bits 4(clear) and 5(set) are ignored
}

void CPU::RLA() { // Undocumented
	uint8_t result = this->RMW(&CPU::ROL);
	this->P.C() = this->operand & 0x80; // Operand is before operation
	this->setA(this->A & result);
}

void CPU::ROL_A() {
	uint8_t result = this->ROL(this->A);
	this->P.C() = this->A & 0x80;
	this->setA(result);
}

void CPU::ROL_M() {
	this->RMW(&CPU::ROL);
	this->P.C() = this->operand & 0x80; // Operand is before operation
}

void CPU::ROR_A() {
	uint8_t result = this->ROR(this->A);
	this->P.C() = this->A & 0x01;
	this->setA(result);
}

void CPU::ROR_M() {
	this->RMW(&CPU::ROR);
	this->P.C() = this->operand & 0x01; // Operand is before operation
}

void CPU::RRA() { // Undocumented
	uint8_t result = this->RMW(&CPU::ROR);
	this->P.C() = this->operand & 0x01; // Operand is before operation
	this->ADD(result);
}

void CPU::RTI() {
	this->dummyRead(); // Wrong Address, shouldn't matter
	this->P.setByte(this->stackPull()); // Bits 4(clear) and 5(set) are ignored
	this->PC = this->stackPullWord();
}

void CPU::RTS() {
	this->dummyRead(); // Wrong Address, shouldn't matter
	this->PC = this->stackPullWord();
	this->dummyRead();
	this->PC++;
}

void CPU::SAX() { // Undocumented
	this->setMem(this->effectiveAddr, (this->A & this->X));
}

void CPU::SEC() {
	this->P.C() = 1;
}

void CPU::SED() {
	this->P.D() = 1;
}

void CPU::SEI() {
	this->P.I() = 1;
}

void CPU::SHX() { // Undocumented
	uint8_t high = this->effectiveAddr >> 8;
	uint8_t low = this->effectiveAddr & 0xFF;
	this->setMem(((this->X & (high + 1)) << 8) | low, this->X & (high + 1));
}

void CPU::SHY() { // Undocumented
	uint8_t high = this->effectiveAddr >> 8;
	uint8_t low = this->effectiveAddr & 0xFF;
	this->setMem( ((this->Y & (high + 1)) << 8) | low, this->Y & (high + 1));
}

void CPU::SKB() { // Undocumented
	this->NOP();
}

void CPU::SLO() { // Undocumented
	this->ASL_M();
	this->setA(this->A | this->ASL(this->operand));
}

void CPU::SRE() { // Undocumented
	this->LSR_M();
	this->setA(this->A ^ this->LSR(this->operand));
}

void CPU::STA() {
	this->setMem(this->effectiveAddr, this->A);
}

void CPU::STX() {
	this->setMem(this->effectiveAddr, this->X);
}

void CPU::STY() {
	this->setMem(this->effectiveAddr, this->Y);
}

void CPU::TAS() {
	this->S = this->A & this->X;
	this->setMem(this->effectiveAddr, this->S & ((this->effectiveAddr >> 8) + 1));
}

void CPU::TAX() {
	this->setX(this->A);
}

void CPU::TAY() {
	this->setY(this->A);
}

void CPU::TYA() {
	this->setA(this->Y);
}

void CPU::TSX() {
	this->setX(this->S);
}

void CPU::TXA() {
	this->setA(this->X);
}

void CPU::TXS() {
	this->S = this->X;
}

void CPU::XAA() {
	this->TXA();
	this->AND();
}
