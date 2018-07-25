#include "stdafx.h"
#include "cpu.h"


// Get Appropriate Addressing Mode or Operation
void CPU::unknownOpcode() {
	throw std::runtime_error("Unknown Opcode!");
}

/*std::function<void(uint8_t cycleNum)> CPU::getAddressingMode(uint8_t opcode) {
	switch(opcode) {
	case 0x00:
	case 0xEA:
	case 0x40:
	case 0x60:
	case 0x0A:
	case 0x4A:
	case 0x2A:
	case 0x6A:
	case 0xAA:
	case 0x8A:
	case 0xCA:
	case 0xE8:
	case 0xA8:
	case 0x98:
	case 0x88:
	case 0xC8:
	case 0x18:
	case 0x38:
	case 0x58:
	case 0x78:
	case 0xB8:
	case 0xD8:
	case 0xF8:
	case 0x9A:
	case 0xBA:
	case 0x48:
	case 0x68:
	case 0x08:
	case 0x28:
	case 0x02:
	case 0x12:
	case 0x22:
	case 0x32:
	case 0x42:
	case 0x52:
	case 0x62:
	case 0x72:
	case 0x92:
	case 0xB2:
	case 0xD2:
	case 0xF2:
	case 0x1A:
	case 0x3A:
	case 0x5A:
	case 0x7A:
	case 0xDA:
	case 0xFA:
		return std::bind(&CPU::implied, this, std::placeholders::_1);
		break;
	case 0x69:
	case 0x29:
	case 0xC9:
	case 0xE0:
	case 0xC0:
	case 0x49:
	case 0xA9:
	case 0xA2:
	case 0xA0:
	case 0x09:
	case 0xE9:
	case 0x4B:
	case 0x0B:
	case 0x2B:
	case 0x8B:
	case 0xAB:
	case 0x6B:
	case 0xEB:
	case 0xCB:
	case 0x80:
	case 0x82:
	case 0x89:
	case 0xC2:
	case 0xE2:
		return std::bind(&CPU::immediate, this, std::placeholders::_1);
		break;
	case 0x6D:
	case 0x2D:
	case 0x0E:
	case 0x2C:
	case 0xCD:
	case 0xEC:
	case 0xCC:
	case 0xCE:
	case 0x4D:
	case 0xEE:
	case 0x4C:
	case 0x6C:
	case 0x20:
	case 0xAD:
	case 0xAE:
	case 0xAC:
	case 0x4E:
	case 0x0D:
	case 0x2E:
	case 0x6E:
	case 0xED:
	case 0x8D:
	case 0x8E:
	case 0x8C:
	case 0x0F:
	case 0xCF:
	case 0xEF:
	case 0xAF:
	case 0x4F:
	case 0x2F:
	case 0x6F:
	case 0x8F:
	case 0x0C:
		return std::bind(&CPU::absolute, this, std::placeholders::_1);
		break;
	case 0x65:
	case 0x25:
	case 0x06:
	case 0x24:
	case 0xC5:
	case 0xE4:
	case 0xC4:
	case 0xC6:
	case 0x45:
	case 0xE6:
	case 0xA5:
	case 0xA6:
	case 0xA4:
	case 0x46:
	case 0x05:
	case 0x26:
	case 0x66:
	case 0xE5:
	case 0x85:
	case 0x86:
	case 0x84:
	case 0x07:
	case 0xC7:
	case 0xE7:
	case 0xA7:
	case 0x47:
	case 0x27:
	case 0x67:
	case 0x87:
	case 0x04:
	case 0x44:
	case 0x64:
		return std::bind(&CPU::zeroPage, this, std::placeholders::_1);
		break;
	case 0x10:
	case 0x30:
	case 0x50:
	case 0x70:
	case 0x90:
	case 0xB0:
	case 0xD0:
	case 0xF0:
		return std::bind(&CPU::relative, this, std::placeholders::_1);
		break;
	case 0x7D:
	case 0x3D:
	case 0x1E:
	case 0xDD:
	case 0xDE:
	case 0x5D:
	case 0xFE:
	case 0xBD:
	case 0xBC:
	case 0x5E:
	case 0x1D:
	case 0x3E:
	case 0x7E:
	case 0xFD:
	case 0x9D:
	case 0x1F:
	case 0xDF:
	case 0xFF:
	case 0x5F:
	case 0x3F:
	case 0x7F:
	case 0x9C:
	case 0x1C:
	case 0x3C:
	case 0x5C:
	case 0x7C:
	case 0xDC:
	case 0xFC:
		return std::bind(&CPU::absoluteX, this, std::placeholders::_1);
		break;
	case 0x79:
	case 0x39:
	case 0xD9:
	case 0x59:
	case 0xB9:
	case 0xBE:
	case 0x19:
	case 0xF9:
	case 0x99:
	case 0x1B:
	case 0xDB:
	case 0xFB:
	case 0xBB:
	case 0xBF:
	case 0x5B:
	case 0x3B:
	case 0x7B:
	case 0x9F:
	case 0x9B:
	case 0x9E:
		return std::bind(&CPU::absoluteY, this, std::placeholders::_1);
		break;
	case 0x75:
	case 0x35:
	case 0x16:
	case 0xD5:
	case 0xD6:
	case 0x55:
	case 0xF6:
	case 0xB5:
	case 0xB4:
	case 0x56:
	case 0x15:
	case 0x36:
	case 0x76:
	case 0xF5:
	case 0x95:
	case 0x94:
	case 0x17:
	case 0xD7:
	case 0xF7:
	case 0x57:
	case 0x37:
	case 0x77:
	case 0x14:
	case 0x34:
	case 0x54:
	case 0x74:
	case 0xD4:
	case 0xF4:
		return std::bind(&CPU::zeroPageX, this, std::placeholders::_1);
		break;
	case 0xB6:
	case 0x96:
	case 0xB7:
	case 0x97:
		return std::bind(&CPU::zeroPageY, this, std::placeholders::_1);
		break;
	case 0x61:
	case 0x21:
	case 0xC1:
	case 0x41:
	case 0xA1:
	case 0x01:
	case 0xE1:
	case 0x81:
	case 0x03:
	case 0xC3:
	case 0xE3:
	case 0xA3:
	case 0x43:
	case 0x23:
	case 0x63:
	case 0x83:
		return std::bind(&CPU::indirectX, this, std::placeholders::_1);
		break;
	case 0x71:
	case 0x31:
	case 0xD1:
	case 0x51:
	case 0xB1:
	case 0x11:
	case 0xF1:
	case 0x91:
	case 0x13:
	case 0xD3:
	case 0xF3:
	case 0xB3:
	case 0x53:
	case 0x33:
	case 0x73:
	case 0x93:
		return std::bind(&CPU::indirectY, this, std::placeholders::_1);
		break;
	default:
		this->unknownOpcode();
		return std::bind(&CPU::implied, this, std::placeholders::_1);
		break;
	}
}*/

void(CPU::*CPU::addressingModes[256])(uint8_t cycleNum) = {
//  0                1                2                3                4                5                6                7                8              9                A              B                C                D                E                F
	&CPU::implied,   &CPU::indirectX, &CPU::implied,   &CPU::indirectX, &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::implied, &CPU::immediate, &CPU::implied, &CPU::immediate, &CPU::absolute,  &CPU::absolute,  &CPU::absolute,  &CPU::absolute,  // 0
	&CPU::relative,  &CPU::indirectY, &CPU::implied,   &CPU::indirectY, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::implied, &CPU::absoluteY, &CPU::implied, &CPU::absoluteY, &CPU::absoluteX, &CPU::absoluteX, &CPU::absoluteX, &CPU::absoluteX, // 1
	&CPU::absolute,  &CPU::indirectX, &CPU::implied,   &CPU::indirectX, &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::implied, &CPU::immediate, &CPU::implied, &CPU::immediate, &CPU::absolute,  &CPU::absolute,  &CPU::absolute,  &CPU::absolute,  // 2
	&CPU::relative,  &CPU::indirectY, &CPU::implied,   &CPU::indirectY, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::implied, &CPU::absoluteY, &CPU::implied, &CPU::absoluteY, &CPU::absoluteX, &CPU::absoluteX, &CPU::absoluteX, &CPU::absoluteX, // 3
	&CPU::implied,   &CPU::indirectX, &CPU::implied,   &CPU::indirectX, &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::implied, &CPU::immediate, &CPU::implied, &CPU::immediate, &CPU::absolute,  &CPU::absolute,  &CPU::absolute,  &CPU::absolute,  // 4
	&CPU::relative,  &CPU::indirectY, &CPU::implied,   &CPU::indirectY, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::implied, &CPU::absoluteY, &CPU::implied, &CPU::absoluteY, &CPU::absoluteX, &CPU::absoluteX, &CPU::absoluteX, &CPU::absoluteX, // 5
	&CPU::implied,   &CPU::indirectX, &CPU::implied,   &CPU::indirectX, &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::implied, &CPU::immediate, &CPU::implied, &CPU::immediate, &CPU::absolute,  &CPU::absolute,  &CPU::absolute,  &CPU::absolute,  // 6
	&CPU::relative,  &CPU::indirectY, &CPU::implied,   &CPU::indirectY, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::implied, &CPU::absoluteY, &CPU::implied, &CPU::absoluteY, &CPU::absoluteX, &CPU::absoluteX, &CPU::absoluteX, &CPU::absoluteX, // 7
	&CPU::immediate, &CPU::indirectX, &CPU::immediate, &CPU::indirectX, &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::implied, &CPU::immediate, &CPU::implied, &CPU::immediate, &CPU::absolute,  &CPU::absolute,  &CPU::absolute,  &CPU::absolute,  // 8
	&CPU::relative,  &CPU::indirectY, &CPU::implied,   &CPU::indirectY, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageY, &CPU::zeroPageY, &CPU::implied, &CPU::absoluteY, &CPU::implied, &CPU::absoluteY, &CPU::absoluteX, &CPU::absoluteX, &CPU::absoluteY, &CPU::absoluteY, // 9
	&CPU::immediate, &CPU::indirectX, &CPU::immediate, &CPU::indirectX, &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::implied, &CPU::immediate, &CPU::implied, &CPU::immediate, &CPU::absolute,  &CPU::absolute,  &CPU::absolute,  &CPU::absolute,  // A
	&CPU::relative,  &CPU::indirectY, &CPU::implied,   &CPU::indirectY, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageY, &CPU::zeroPageY, &CPU::implied, &CPU::absoluteY, &CPU::implied, &CPU::absoluteY, &CPU::absoluteX, &CPU::absoluteX, &CPU::absoluteY, &CPU::absoluteY, // B
	&CPU::immediate, &CPU::indirectX, &CPU::immediate, &CPU::indirectX, &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::implied, &CPU::immediate, &CPU::implied, &CPU::immediate, &CPU::absolute,  &CPU::absolute,  &CPU::absolute,  &CPU::absolute,  // C
	&CPU::relative,  &CPU::indirectY, &CPU::implied,   &CPU::indirectY, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::implied, &CPU::absoluteY, &CPU::implied, &CPU::absoluteY, &CPU::absoluteX, &CPU::absoluteX, &CPU::absoluteX, &CPU::absoluteX, // D
	&CPU::immediate, &CPU::indirectX, &CPU::immediate, &CPU::indirectX, &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::zeroPage,  &CPU::implied, &CPU::immediate, &CPU::implied, &CPU::immediate, &CPU::absolute,  &CPU::absolute,  &CPU::absolute,  &CPU::absolute,  // E
	&CPU::relative,  &CPU::indirectY, &CPU::implied,   &CPU::indirectY, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::zeroPageX, &CPU::implied, &CPU::absoluteY, &CPU::implied, &CPU::absoluteY, &CPU::absoluteX, &CPU::absoluteX, &CPU::absoluteX, &CPU::absoluteX  // F
};

void(CPU::*CPU::operations[256])() = {
//  0          1          2          3          4          5          6          7          8         9           A          B          C          D          E          F
	&CPU::BRK, &CPU::ORA, &CPU::KIL, &CPU::SLO, &CPU::SKB, &CPU::ORA, &CPU::ASL, &CPU::SLO, &CPU::PHP, &CPU::ORA, &CPU::ASL, &CPU::ANC, &CPU::IGN, &CPU::ORA, &CPU::ASL, &CPU::SLO, // 0
	&CPU::BPL, &CPU::ORA, &CPU::KIL, &CPU::SLO, &CPU::SKB, &CPU::ORA, &CPU::ASL, &CPU::SLO, &CPU::CLC, &CPU::ORA, &CPU::NOP, &CPU::SLO, &CPU::IGN, &CPU::ORA, &CPU::ASL, &CPU::SLO, // 1
	&CPU::JSR, &CPU::AND, &CPU::KIL, &CPU::RLA, &CPU::BIT, &CPU::AND, &CPU::ROL, &CPU::RLA, &CPU::PLP, &CPU::AND, &CPU::ROL, &CPU::ANC, &CPU::BIT, &CPU::AND, &CPU::ROL, &CPU::RLA, // 2
	&CPU::BMI, &CPU::AND, &CPU::KIL, &CPU::RLA, &CPU::SKB, &CPU::AND, &CPU::ROL, &CPU::RLA, &CPU::SEC, &CPU::AND, &CPU::NOP, &CPU::RLA, &CPU::IGN, &CPU::AND, &CPU::ROL, &CPU::RLA, // 3
	&CPU::RTI, &CPU::EOR, &CPU::KIL, &CPU::SRE, &CPU::SKB, &CPU::EOR, &CPU::LSR, &CPU::SRE, &CPU::PHA, &CPU::EOR, &CPU::LSR, &CPU::ALR, &CPU::JMP, &CPU::EOR, &CPU::LSR, &CPU::SRE, // 4
	&CPU::BVC, &CPU::EOR, &CPU::KIL, &CPU::SRE, &CPU::SKB, &CPU::EOR, &CPU::LSR, &CPU::SRE, &CPU::CLI, &CPU::EOR, &CPU::NOP, &CPU::SRE, &CPU::IGN, &CPU::EOR, &CPU::LSR, &CPU::SRE, // 5
	&CPU::RTS, &CPU::ADC, &CPU::KIL, &CPU::RRA, &CPU::SKB, &CPU::ADC, &CPU::ROR, &CPU::RRA, &CPU::PLA, &CPU::ADC, &CPU::ROR, &CPU::ARR, &CPU::JMP, &CPU::ADC, &CPU::ROR, &CPU::RRA, // 6
	&CPU::BVS, &CPU::ADC, &CPU::KIL, &CPU::RRA, &CPU::SKB, &CPU::ADC, &CPU::ROR, &CPU::RRA, &CPU::SEI, &CPU::ADC, &CPU::NOP, &CPU::RRA, &CPU::IGN, &CPU::ADC, &CPU::ROR, &CPU::RRA, // 7
	&CPU::SKB, &CPU::STA, &CPU::SKB, &CPU::SAX, &CPU::STY, &CPU::STA, &CPU::STX, &CPU::SAX, &CPU::DEY, &CPU::SKB, &CPU::TXA, &CPU::XAA, &CPU::STY, &CPU::STA, &CPU::STX, &CPU::SAX, // 8
	&CPU::BCC, &CPU::STA, &CPU::KIL, &CPU::AHX, &CPU::STY, &CPU::STA, &CPU::STX, &CPU::SAX, &CPU::TYA, &CPU::STA, &CPU::TXS, &CPU::TAS, &CPU::SHY, &CPU::STA, &CPU::SHX, &CPU::AHX, // 9
	&CPU::LDY, &CPU::LDA, &CPU::LDX, &CPU::LAX, &CPU::LDY, &CPU::LDA, &CPU::LDX, &CPU::LAX, &CPU::TAY, &CPU::LDA, &CPU::TAX, &CPU::LAX, &CPU::LDY, &CPU::LDA, &CPU::LDX, &CPU::LAX, // A
	&CPU::BCS, &CPU::LDA, &CPU::KIL, &CPU::LAX, &CPU::LDY, &CPU::LDA, &CPU::LDX, &CPU::LAX, &CPU::CLV, &CPU::LDA, &CPU::TSX, &CPU::LAS, &CPU::LDY, &CPU::LDA, &CPU::LDX, &CPU::LAX, // B
	&CPU::CPY, &CPU::CMP, &CPU::SKB, &CPU::DCP, &CPU::CPY, &CPU::CMP, &CPU::DEC, &CPU::DCP, &CPU::INY, &CPU::CMP, &CPU::DEX, &CPU::AXS, &CPU::CPY, &CPU::CMP, &CPU::DEC, &CPU::DCP, // C
	&CPU::BNE, &CPU::CMP, &CPU::KIL, &CPU::DCP, &CPU::SKB, &CPU::CMP, &CPU::DEC, &CPU::DCP, &CPU::CLD, &CPU::CMP, &CPU::NOP, &CPU::DCP, &CPU::IGN, &CPU::CMP, &CPU::DEC, &CPU::DCP, // D
	&CPU::CPX, &CPU::SBC, &CPU::SKB, &CPU::ISC, &CPU::CPX, &CPU::SBC, &CPU::INC, &CPU::ISC, &CPU::INX, &CPU::SBC, &CPU::NOP, &CPU::SBC, &CPU::CPX, &CPU::SBC, &CPU::INC, &CPU::ISC, // E
	&CPU::BEQ, &CPU::SBC, &CPU::KIL, &CPU::ISC, &CPU::SKB, &CPU::SBC, &CPU::INC, &CPU::ISC, &CPU::SED, &CPU::SBC, &CPU::NOP, &CPU::ISC, &CPU::IGN, &CPU::SBC, &CPU::INC, &CPU::ISC  // F
};

/*std::function<void()> CPU::getOperation(uint8_t opcode) {
	switch(opcode) {
	case 0x69:
	case 0x65:
	case 0x75:
	case 0x6D:
	case 0x7D:
	case 0x79:
	case 0x61:
	case 0x71:
		return std::bind(&CPU::ADC, this);
		break;
	case 0x93:
	case 0x9F:
		return std::bind(&CPU::AHX, this);
		break;
	case 0x4B:
		return std::bind(&CPU::ALR, this);
		break;
	case 0x0B:
	case 0x2B:
		return std::bind(&CPU::ANC, this);
		break;
	case 0x29:
	case 0x25:
	case 0x35:
	case 0x2D:
	case 0x3D:
	case 0x39:
	case 0x21:
	case 0x31:
		return std::bind(&CPU::AND, this);
		break;
	case 0x6B:
		return std::bind(&CPU::ARR, this);
		break;
	case 0xCB:
		return std::bind(&CPU::AXS, this);
		break;
	case 0x0A:
	case 0x06:
	case 0x16:
	case 0x0E:
	case 0x1E:
		return std::bind(&CPU::ASL, this);
		break;
	case 0x90:
		return std::bind(&CPU::BCC, this);
		break;
	case 0xB0:
		return std::bind(&CPU::BCS, this);
		break;
	case 0xF0:
		return std::bind(&CPU::BEQ, this);
		break;
	case 0x24:
	case 0x2C:
		return std::bind(&CPU::BIT, this);
		break;
	case 0x30:
		return std::bind(&CPU::BMI, this);
		break;
	case 0xD0:
		return std::bind(&CPU::BNE, this);
		break;
	case 0x10:
		return std::bind(&CPU::BPL, this);
		break;
	case 0x00:
		return std::bind(&CPU::BRK, this);
		break;
	case 0x50:
		return std::bind(&CPU::BVC, this);
		break;
	case 0x70:
		return std::bind(&CPU::BVS, this);
		break;
	case 0x18:
		return std::bind(&CPU::CLC, this);
		break;
	case 0xD8:
		return std::bind(&CPU::CLD, this);
		break;
	case 0x58:
		return std::bind(&CPU::CLI, this);
		break;
	case 0xB8:
		return std::bind(&CPU::CLV, this);
		break;
	case 0xC9:
	case 0xC5:
	case 0xD5:
	case 0xCD:
	case 0xDD:
	case 0xD9:
	case 0xC1:
	case 0xD1:
		return std::bind(&CPU::CMP, this);
		break;
	case 0xE0:
	case 0xE4:
	case 0xEC:
		return std::bind(&CPU::CPX, this);
		break;
	case 0xC0:
	case 0xC4:
	case 0xCC:
		return std::bind(&CPU::CPY, this);
		break;
	case 0xC3:
	case 0xC7:
	case 0xCF:
	case 0xD3:
	case 0xD7:
	case 0xDB:
	case 0xDF:
		return std::bind(&CPU::DCP, this);
		break;
	case 0xC6:
	case 0xD6:
	case 0xCE:
	case 0xDE:
		return std::bind(&CPU::DEC, this);
		break;
	case 0xCA:
		return std::bind(&CPU::DEX, this);
		break;
	case 0x88:
		return std::bind(&CPU::DEY, this);
		break;
	case 0x49:
	case 0x45:
	case 0x55:
	case 0x4D:
	case 0x5D:
	case 0x59:
	case 0x41:
	case 0x51:
		return std::bind(&CPU::EOR, this);
		break;
	case 0x0C:
	case 0x1C:
	case 0x3C:
	case 0x5C:
	case 0x7C:
	case 0xDC:
	case 0xFC:
		return std::bind(&CPU::IGN, this);
		break;
	case 0xE6:
	case 0xF6:
	case 0xEE:
	case 0xFE:
		return std::bind(&CPU::INC, this);
		break;
	case 0xE8:
		return std::bind(&CPU::INX, this);
		break;
	case 0xC8:
		return std::bind(&CPU::INY, this);
		break;
	case 0xE3:
	case 0xE7:
	case 0xEF:
	case 0xF3:
	case 0xF7:
	case 0xFB:
	case 0xFF:
		return std::bind(&CPU::ISC, this);
		break;
	case 0x4C:
	case 0x6C:
		return std::bind(&CPU::JMP, this);
		break;
	case 0x20:
		return std::bind(&CPU::JSR, this);
		break;
	case 0x02:
	case 0x12:
	case 0x22:
	case 0x32:
	case 0x42:
	case 0x52:
	case 0x62:
	case 0x72:
	case 0x92:
	case 0xB2:
	case 0xD2:
	case 0xF2:
		return std::bind(&CPU::KIL, this);
		break;
	case 0xBB:
		return std::bind(&CPU::LAS, this);
		break;
	case 0xAB:
	case 0xA3:
	case 0xA7:
	case 0xAF:
	case 0xB3:
	case 0xB7:
	case 0xBF:
		return std::bind(&CPU::LAX, this);
		break;
	case 0xA9:
	case 0xA5:
	case 0xB5:
	case 0xAD:
	case 0xBD:
	case 0xB9:
	case 0xA1:
	case 0xB1:
		return std::bind(&CPU::LDA, this);
		break;
	case 0xA2:
	case 0xA6:
	case 0xB6:
	case 0xAE:
	case 0xBE:
		return std::bind(&CPU::LDX, this);
		break;
	case 0xA0:
	case 0xA4:
	case 0xB4:
	case 0xAC:
	case 0xBC:
		return std::bind(&CPU::LDY, this);
		break;
	case 0x4A:
	case 0x46:
	case 0x56:
	case 0x4E:
	case 0x5E:
		return std::bind(&CPU::LSR, this);
		break;
	case 0x1A:
	case 0x3A:
	case 0x5A:
	case 0x7A:
	case 0xDA:
	case 0xEA:
	case 0xFA:
		return std::bind(&CPU::NOP, this);
		break;
	case 0x09:
	case 0x05:
	case 0x15:
	case 0x0D:
	case 0x1D:
	case 0x19:
	case 0x01:
	case 0x11:
		return std::bind(&CPU::ORA, this);
		break;
	case 0x48:
		return std::bind(&CPU::PHA, this);
		break;
	case 0x08:
		return std::bind(&CPU::PHP, this);
		break;
	case 0x68:
		return std::bind(&CPU::PLA, this);
		break;
	case 0x28:
		return std::bind(&CPU::PLP, this);
		break;
	case 0x23:
	case 0x27:
	case 0x2F:
	case 0x33:
	case 0x37:
	case 0x3B:
	case 0x3F:
		return std::bind(&CPU::RLA, this);
		break;
	case 0x2A:
	case 0x26:
	case 0x36:
	case 0x2E:
	case 0x3E:
		return std::bind(&CPU::ROL, this);
		break;
	case 0x6A:
	case 0x66:
	case 0x76:
	case 0x6E:
	case 0x7E:
		return std::bind(&CPU::ROR, this);
		break;
	case 0x63:
	case 0x67:
	case 0x6F:
	case 0x73:
	case 0x77:
	case 0x7B:
	case 0x7F:
		return std::bind(&CPU::RRA, this);
		break;
	case 0x40:
		return std::bind(&CPU::RTI, this);
		break;
	case 0x60:
		return std::bind(&CPU::RTS, this);
		break;
	case 0x83:
	case 0x87:
	case 0x8F:
	case 0x97:
		return std::bind(&CPU::SAX, this);
		break;
	case 0xE9:
	case 0xE5:
	case 0xF5:
	case 0xEB:
	case 0xED:
	case 0xFD:
	case 0xF9:
	case 0xE1:
	case 0xF1:
		return std::bind(&CPU::SBC, this);
		break;
	case 0x38:
		return std::bind(&CPU::SEC, this);
		break;
	case 0xF8:
		return std::bind(&CPU::SED, this);
		break;
	case 0x78:
		return std::bind(&CPU::SEI, this);
		break;
	case 0x9E:
		return std::bind(&CPU::SHX, this);
		break;
	case 0x9C:
		return std::bind(&CPU::SHY, this);
		break;
	case 0x04:
	case 0x14:
	case 0x34:
	case 0x44:
	case 0x54:
	case 0x64:
	case 0x74:
	case 0x80:
	case 0x82:
	case 0x89:
	case 0xC2:
	case 0xD4:
	case 0xE2:
	case 0xF4:
		return std::bind(&CPU::SKB, this);
		break;
	case 0x03:
	case 0x07:
	case 0x0F:
	case 0x13:
	case 0x17:
	case 0x1B:
	case 0x1F:
		return std::bind(&CPU::SLO, this);
		break;
	case 0x43:
	case 0x47:
	case 0x4F:
	case 0x53:
	case 0x57:
	case 0x5B:
	case 0x5F:
		return std::bind(&CPU::SRE, this);
		break;
	case 0x85:
	case 0x95:
	case 0x8D:
	case 0x9D:
	case 0x99:
	case 0x81:
	case 0x91:
		return std::bind(&CPU::STA, this);
		break;
	case 0x86:
	case 0x96:
	case 0x8E:
		return std::bind(&CPU::STX, this);
		break;
	case 0x84:
	case 0x94:
	case 0x8C:
		return std::bind(&CPU::STY, this);
		break;
	case 0x9B:
		return std::bind(&CPU::TAS, this);
		break;
	case 0xAA:
		return std::bind(&CPU::TAX, this);
		break;
	case 0xA8:
		return std::bind(&CPU::TAY, this);
		break;
	case 0xBA:
		return std::bind(&CPU::TSX, this);
		break;
	case 0x8A:
		return std::bind(&CPU::TXA, this);
		break;
	case 0x9A:
		return std::bind(&CPU::TXS, this);
		break;
	case 0x98:
		return std::bind(&CPU::TYA, this);
		break;
	case 0x8B:
		return std::bind(&CPU::XAA, this);
		break;
	default:
		this->unknownOpcode();
		return std::bind(&CPU::NOP, this);
		break;
	}
}*/
