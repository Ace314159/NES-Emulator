#include "stdafx.h"
#include "CPU.h"


// Get Appropriate Addressing Mode or Operation
void(CPU::*CPU::addressingModes[256])() = {
//  0                1                 2                3                 4                5                6                7                8              9                 A              B                 C                 D                 E                 F
	&CPU::implied,   &CPU::indirectX,  &CPU::implied,   &CPU::indirectX,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::implied, &CPU::immediate,  &CPU::implied, &CPU::immediate,  &CPU::absolute,   &CPU::absolute,   &CPU::absolute,   &CPU::absolute,  // 0
	&CPU::relative,  &CPU::indirectYR, &CPU::implied,   &CPU::indirectY,  &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::implied, &CPU::absoluteYR, &CPU::implied, &CPU::absoluteY,  &CPU::absoluteXR, &CPU::absoluteXR, &CPU::absoluteX,  &CPU::absoluteX, // 1
	&CPU::absolute,  &CPU::indirectX,  &CPU::implied,   &CPU::indirectX,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::implied, &CPU::immediate,  &CPU::implied, &CPU::immediate,  &CPU::absolute,   &CPU::absolute,   &CPU::absolute,   &CPU::absolute,  // 2
	&CPU::relative,  &CPU::indirectYR, &CPU::implied,   &CPU::indirectY,  &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::implied, &CPU::absoluteYR, &CPU::implied, &CPU::absoluteY,  &CPU::absoluteXR, &CPU::absoluteXR, &CPU::absoluteX,  &CPU::absoluteX, // 3
	&CPU::implied,   &CPU::indirectX,  &CPU::implied,   &CPU::indirectX,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::implied, &CPU::immediate,  &CPU::implied, &CPU::immediate,  &CPU::absolute,   &CPU::absolute,   &CPU::absolute,   &CPU::absolute,  // 4
	&CPU::relative,  &CPU::indirectYR, &CPU::implied,   &CPU::indirectY,  &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::implied, &CPU::absoluteYR, &CPU::implied, &CPU::absoluteY,  &CPU::absoluteXR, &CPU::absoluteXR, &CPU::absoluteX,  &CPU::absoluteX, // 5
	&CPU::implied,   &CPU::indirectX,  &CPU::implied,   &CPU::indirectX,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::implied, &CPU::immediate,  &CPU::implied, &CPU::immediate,  &CPU::absolute,   &CPU::absolute,   &CPU::absolute,   &CPU::absolute,  // 6
	&CPU::relative,  &CPU::indirectYR, &CPU::implied,   &CPU::indirectY,  &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::implied, &CPU::absoluteYR, &CPU::implied, &CPU::absoluteY,  &CPU::absoluteXR, &CPU::absoluteXR, &CPU::absoluteX,  &CPU::absoluteX, // 7
	&CPU::immediate, &CPU::indirectX,  &CPU::immediate, &CPU::indirectX,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::implied, &CPU::immediate,  &CPU::implied, &CPU::immediate,  &CPU::absolute,   &CPU::absolute,   &CPU::absolute,   &CPU::absolute,  // 8
	&CPU::relative,  &CPU::indirectY,  &CPU::implied,   &CPU::indirectY,  &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageY, &CPU::zeroPageY, &CPU::implied, &CPU::absoluteY,  &CPU::implied, &CPU::absoluteY,  &CPU::absoluteX,  &CPU::absoluteX,  &CPU::absoluteY,  &CPU::absoluteY, // 9
	&CPU::immediate, &CPU::indirectX,  &CPU::immediate, &CPU::indirectX,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::implied, &CPU::immediate,  &CPU::implied, &CPU::immediate,  &CPU::absolute,   &CPU::absolute,   &CPU::absolute,   &CPU::absolute,  // A
	&CPU::relative,  &CPU::indirectYR, &CPU::implied,   &CPU::indirectYR, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageY, &CPU::zeroPageY, &CPU::implied, &CPU::absoluteYR, &CPU::implied, &CPU::absoluteYR, &CPU::absoluteXR, &CPU::absoluteXR, &CPU::absoluteYR, &CPU::absoluteYR, // B
	&CPU::immediate, &CPU::indirectX,  &CPU::immediate, &CPU::indirectX,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::implied, &CPU::immediate,  &CPU::implied, &CPU::immediate,  &CPU::absolute,   &CPU::absolute,   &CPU::absolute,   &CPU::absolute,  // C
	&CPU::relative,  &CPU::indirectYR, &CPU::implied,   &CPU::indirectY,  &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::implied, &CPU::absoluteYR, &CPU::implied, &CPU::absoluteY,  &CPU::absoluteXR, &CPU::absoluteXR, &CPU::absoluteX,  &CPU::absoluteX, // D
	&CPU::immediate, &CPU::indirectX,  &CPU::immediate, &CPU::indirectX,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::implied, &CPU::immediate,  &CPU::implied, &CPU::immediate,  &CPU::absolute,   &CPU::absolute,   &CPU::absolute,   &CPU::absolute,  // E
	&CPU::relative,  &CPU::indirectYR, &CPU::implied,   &CPU::indirectY,  &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::implied, &CPU::absoluteYR, &CPU::implied, &CPU::absoluteY,  &CPU::absoluteXR, &CPU::absoluteXR, &CPU::absoluteX,  &CPU::absoluteX  // F
};

void(CPU::*CPU::operations[256])() = {
//  0          1          2          3          4          5          6            7          8         9           A            B          C              D          E            F
	&CPU::BRK, &CPU::ORA, &CPU::KIL, &CPU::SLO, &CPU::SKB, &CPU::ORA, &CPU::ASL_M, &CPU::SLO, &CPU::PHP, &CPU::ORA, &CPU::ASL_A, &CPU::ANC, &CPU::IGN,     &CPU::ORA, &CPU::ASL_M, &CPU::SLO, // 0
	&CPU::BPL, &CPU::ORA, &CPU::KIL, &CPU::SLO, &CPU::SKB, &CPU::ORA, &CPU::ASL_M, &CPU::SLO, &CPU::CLC, &CPU::ORA, &CPU::NOP,   &CPU::SLO, &CPU::IGN,     &CPU::ORA, &CPU::ASL_M, &CPU::SLO, // 1
	&CPU::JSR, &CPU::AND, &CPU::KIL, &CPU::RLA, &CPU::BIT, &CPU::AND, &CPU::ROL_M, &CPU::RLA, &CPU::PLP, &CPU::AND, &CPU::ROL_A, &CPU::ANC, &CPU::BIT,     &CPU::AND, &CPU::ROL_M, &CPU::RLA, // 2
	&CPU::BMI, &CPU::AND, &CPU::KIL, &CPU::RLA, &CPU::SKB, &CPU::AND, &CPU::ROL_M, &CPU::RLA, &CPU::SEC, &CPU::AND, &CPU::NOP,   &CPU::RLA, &CPU::IGN,     &CPU::AND, &CPU::ROL_M, &CPU::RLA, // 3
	&CPU::RTI, &CPU::EOR, &CPU::KIL, &CPU::SRE, &CPU::SKB, &CPU::EOR, &CPU::LSR_M, &CPU::SRE, &CPU::PHA, &CPU::EOR, &CPU::LSR_A, &CPU::ALR, &CPU::JMP_Abs, &CPU::EOR, &CPU::LSR_M, &CPU::SRE, // 4
	&CPU::BVC, &CPU::EOR, &CPU::KIL, &CPU::SRE, &CPU::SKB, &CPU::EOR, &CPU::LSR_M, &CPU::SRE, &CPU::CLI, &CPU::EOR, &CPU::NOP,   &CPU::SRE, &CPU::IGN,     &CPU::EOR, &CPU::LSR_M, &CPU::SRE, // 5
	&CPU::RTS, &CPU::ADC, &CPU::KIL, &CPU::RRA, &CPU::SKB, &CPU::ADC, &CPU::ROR_M, &CPU::RRA, &CPU::PLA, &CPU::ADC, &CPU::ROR_A, &CPU::ARR, &CPU::JMP_Ind, &CPU::ADC, &CPU::ROR_M, &CPU::RRA, // 6
	&CPU::BVS, &CPU::ADC, &CPU::KIL, &CPU::RRA, &CPU::SKB, &CPU::ADC, &CPU::ROR_M, &CPU::RRA, &CPU::SEI, &CPU::ADC, &CPU::NOP,   &CPU::RRA, &CPU::IGN,     &CPU::ADC, &CPU::ROR_M, &CPU::RRA, // 7
	&CPU::SKB, &CPU::STA, &CPU::SKB, &CPU::SAX, &CPU::STY, &CPU::STA, &CPU::STX,   &CPU::SAX, &CPU::DEY, &CPU::SKB, &CPU::TXA,   &CPU::XAA, &CPU::STY,     &CPU::STA, &CPU::STX,   &CPU::SAX, // 8
	&CPU::BCC, &CPU::STA, &CPU::KIL, &CPU::AHX, &CPU::STY, &CPU::STA, &CPU::STX,   &CPU::SAX, &CPU::TYA, &CPU::STA, &CPU::TXS,   &CPU::TAS, &CPU::SHY,     &CPU::STA, &CPU::SHX,   &CPU::AHX, // 9
	&CPU::LDY, &CPU::LDA, &CPU::LDX, &CPU::LAX, &CPU::LDY, &CPU::LDA, &CPU::LDX,   &CPU::LAX, &CPU::TAY, &CPU::LDA, &CPU::TAX,   &CPU::LAX, &CPU::LDY,     &CPU::LDA, &CPU::LDX,   &CPU::LAX, // A
	&CPU::BCS, &CPU::LDA, &CPU::KIL, &CPU::LAX, &CPU::LDY, &CPU::LDA, &CPU::LDX,   &CPU::LAX, &CPU::CLV, &CPU::LDA, &CPU::TSX,   &CPU::LAS, &CPU::LDY,     &CPU::LDA, &CPU::LDX,   &CPU::LAX, // B
	&CPU::CPY, &CPU::CPA, &CPU::SKB, &CPU::DCP, &CPU::CPY, &CPU::CPA, &CPU::DEC,   &CPU::DCP, &CPU::INY, &CPU::CPA, &CPU::DEX,   &CPU::AXS, &CPU::CPY,     &CPU::CPA, &CPU::DEC,   &CPU::DCP, // C
	&CPU::BNE, &CPU::CPA, &CPU::KIL, &CPU::DCP, &CPU::SKB, &CPU::CPA, &CPU::DEC,   &CPU::DCP, &CPU::CLD, &CPU::CPA, &CPU::NOP,   &CPU::DCP, &CPU::IGN,     &CPU::CPA, &CPU::DEC,   &CPU::DCP, // D
	&CPU::CPX, &CPU::SBC, &CPU::SKB, &CPU::ISC, &CPU::CPX, &CPU::SBC, &CPU::INC,   &CPU::ISC, &CPU::INX, &CPU::SBC, &CPU::NOP,   &CPU::SBC, &CPU::CPX,     &CPU::SBC, &CPU::INC,   &CPU::ISC, // E
	&CPU::BEQ, &CPU::SBC, &CPU::KIL, &CPU::ISC, &CPU::SKB, &CPU::SBC, &CPU::INC,   &CPU::ISC, &CPU::SED, &CPU::SBC, &CPU::NOP,   &CPU::ISC, &CPU::IGN,     &CPU::SBC, &CPU::INC,   &CPU::ISC  // F
};
