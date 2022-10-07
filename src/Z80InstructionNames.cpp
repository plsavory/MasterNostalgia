//
// Created by Peter Savory on 06/10/2022.
//

#ifndef MASTALGIA_Z80INSTRUCTIONNAMES_CPP
#define MASTALGIA_Z80INSTRUCTIONNAMES_CPP

#include <iostream>
#include "Cartridge.h"
#include "Memory.h"
#include "CPUZ80.h"
#include <vector>

std::vector<std::string> CPUZ80::getStandardInstructionNames() {
    return {
        "NOP", "LD BC,nn", "LD (BC),A", "INC BC", "INC B", "DEC B", "LD B, n", "RLCA", "EX AF,AF'", "ADD HL,BC", "LD A,(BC)", "DEC BC", "INC C", "DEC C", "LD C, n", "RRCA",
        "DJNZ d", "LD DE,nn", "LD (DE),A", "INC DE", "INC D", "DEC D", "LD D,n", "RLA", "JR d", "ADD HL,DE", "LD A,(DE)", "DEC DE", "INC E", "DEC E", "LD E,n", "RRA",
        "JR NZ,d", "LD HL,nn", "LD (nn),HL", "INC HL", "INC H", "DEC H", "LD H,n", "DAA", "JR Z,d", "ADD HL,HL", "LD HL,(nn)", "DEC HL", "INC L", "DEC L", "LD L, n", "CPL",
        "JR NC,d", "LD SP,nn", "LD (nn),A", "INC SP", "INC (HL)", "DEC (HL)", "LD (HL),n", "SCF", "JR C,d", "ADD HL,SP", "LD A,(nn)", "DEC SP", "INC A", "DEC A", "LD A,n", "CCF",
        "LD B,B", "LD B,C", "LDB,D", "LD B,E", "LD B,H", "LD B,L", "LD B,(HL)", "LD B,A", "LD C,B", "LD C,C", "LD C,D", "LD C,E", "LD C,H", "LD C,L", "LD C,(HL)", "LD C,A",
        "LD D,B", "LD D,C", "LD D,D", "LD D,E", "LD D,H", "LD D,L", "LD D,(HL)", "LD D,A", "LD E,B", "LD E,C", "LD E,D", "LD E,E", "LD E,H", "LD E,L", "LD E,(HL)", "LD E,A",
        "LD H,B", "LD H,C", "LD H,D", "LD H,E", "LD H,H", "LD H,L", "LD H,(HL)", "LD H,A", "LD L,B", "LD L,C", "LD L,D", "LD L,E", "LD L,H", "LD L,L", "LD L,(HL)", "LD L,A",
        "LD (HL),B", "LD (HL),C", "LD (HL),D", "LD (HL),E", "LD (HL),H", "LD (HL),L", "HALT", "LD (HL),A", "LD A,B", "LD A,C", "LD A,D", "LD A,E", "LD A,H", "LD A,L", "LD A,(HL)", "ADC A,A",
        "ADD A,B", "ADD A,C", "ADD A,D", "ADD A,E", "ADD A,H", "ADD A,L", "ADD A,(HL)", "ADD A,A", "ADC A,B", "ADC A,C", "ADC A,D", "ADC A,E", "ADC A,H", "ADC A,L", "ADC A,(HL)", "ADC A,A",
        "SUB B", "SUB C", "SUB D", "SUB E", "SUB H", "SUB L", "SUB (HL)", "SUB A", "SBC A,B", "SBC A,C", "SBC A,D", "SBC A,E", "SBC A,H", "SBC A,L", "SBC A,(HL)", "SBC A,A",
        "AND B", "AND C", "AND D", "AND E", "AND H", "AND L", "AND (HL)", "AND A", "XOR B", "XOR C", "XOR D", "XOR E", "XOR H", "XOR L", "XOR (HL)", "XOR A",
        "OR B", "OR C", "OR D", "OR E", "OR H", "OR L", "OR (HL)", "OR A", "CP B", "CP C", "CP D", "CP E", "CP H", "CP L", "CP (HL)", "CP A",
        "RET NZ", "POP BC", "JP NZ,nn", "JP nn", "CALL NZ,nn", "PUSH BC", "ADD A,n", "RST 00H", "RET Z", "RET", "JP Z,nn", "BIT", "CALL Z,nn","CALL nn", "ADC A,n", "RST 08H",
        "RET NC", "POP DE", "JP NC,nn", "OUT (n),A", "CALL NC,nn", "PUSH DE", "SUB n", "RST 10H", "RET C", "EXX", "JP C,nn", "IN A,(n)", "CALL C,nn", "IX", "SBC A,n", "RST 18H",
        "RET PO", "POP HL", "JP PO,nn", "EX (SP),HL", "CALL PO,nn", "PUSH HL", "AND n", "RST 20H", "RET PE", "JP (HL)", "JP PE,nn", "EX DE,HL", "CALL PE,nn", "MISC", "XOR n", "RST 28H",
        "RET P", "POP AF", "JP P,nn", "DI", "CALL P,nn", "PUSH AF", "OR n", "RST 30H", "RET M", "LD SP,HL", "JP M,nn", "EI", "CALL M,nn", "IY", "CP n", "RST 38H"
    };
}

std::vector<std::string> CPUZ80::getBitInstructionNames() {
    return {
        "RLC B", "RLC C", "RLC D", "RLC E", "RLC H", "RLC L", "RLC (HL)", "RLC A", "RRC B", "RRC C", "RRC D", "RRC E", "RRC H", "RRC L", "RRC (HL)", "RRC A",
        "RL B", "RL C", "RL D", "RL E", "RL H", "RL L", "RL (HL)", "RL A", "RR B", "RR C", "RR D", "RR E", "RR H", "RR L", "RR (HL)", "RR A",
        "SLA B", "SLA C", "SLA D", "SLA E", "SLA H", "SLA L", "SLA (HL)", "SLA A", "SRA B", "SRA C", "SRA D", "SRA E", "SRA H", "SRA L", "SRA (HL)", "SRA A",
        "SLL B", "SLL C", "SLL D", "SLL E", "SLL H", "SLL L", "SLL (HL)", "SLL A", "SRL B", "SRL C", "SRL D", "SRL E", "SRL H", "SRL L", "SRL (HL)", "STL A",
        "BIT 0,B", "BIT 0,C", "BIT 0,D", "BIT 0,E", "BIT 0,H", "BIT 0,L", "BIT 0,(HL)", "BIT 0,A", "BIT 1,B", "BIT 1,C", "BIT 1,D", "BIT 1,E", "BIT 1,H", "BIT 1,L", "BIT 1,(HL)", "BIT 1,A",
        "BIT 2,B", "BIT 2,C", "BIT 2,D", "BIT 2,E", "BIT 2,H", "BIT 2,L", "BIT 2,(HL)", "BIT 2,A", "BIT 3,B", "BIT 3,C", "BIT 3,D", "BIT 3,E", "BIT 3,H", "BIT 3,L", "BIT 3,(HL)", "BIT 3,A",
        "BIT 4,B", "BIT 4,C", "BIT 4,D", "BIT 4,E", "BIT 4,H", "BIT 4,L", "BIT 4,(HL)", "BIT 4,A", "BIT 5,B", "BIT 5,C", "BIT 5,D", "BIT 5,E", "BIT 5,H", "BIT 5,L", "BIT 5,(HL)", "BIT 5,A",
        "BIT 6,B", "BIT 6,C", "BIT 6,D", "BIT 6,E", "BIT 6,H", "BIT 6,L", "BIT 6,(HL)", "BIT 6,A", "BIT 7,B", "BIT 7,C", "BIT 7,D", "BIT 7,E", "BIT 7,H", "BIT 7,L", "BIT 7,(HL)", "BIT 7,A",
        "RES 0,B", "RES 0,C", "RES 0,D", "RES 0,E", "RES 0,H", "RES 0,L", "RES 0,(HL)", "RES 0,A", "RES 1,B", "RES 1,C", "RES 1,D", "RES 1,E", "RES 1,H", "RES 1,L", "RES 1,(HL)", "RES 1,A",
        "RES 2,B", "RES 2,C", "RES 2,D", "RES 2,E", "RES 2,H", "RES 2,L", "RES 2,(HL)", "RES 2,A", "RES 3,B", "RES 3,C", "RES 3,D", "RES 3,E", "RES 3,H", "RES 3,L", "RES 3,(HL)", "RES 3,A",
        "RES 4,B", "RES 4,C", "RES 4,D", "RES 4,E", "RES 4,H", "RES 4,L", "RES 4,(HL)", "RES 4,A", "RES 5,B", "RES 5,C", "RES 5,D", "RES 5,E", "RES 5,H", "RES 5,L", "RES 5,(HL)", "RES 5,A",
        "RES 6,B", "RES 6,C", "RES 6,D", "RES 6,E", "RES 6,H", "RES 6,L", "RES 6,(HL)", "RES 6,A", "RES 7,B", "RES 7,C", "RES 7,D", "RES 7,E", "RES 7,H", "RES 7,L", "RES 7,(HL)", "RES 7,A",
        "SET 0,B", "SET 0,C", "SET 0,D", "SET 0,E", "SET 0,H", "SET 0,L", "SET 0,(HL)", "SET 0,A", "SET 1,B", "SET 1,C", "SET 1,D", "SET 1,E", "SET 1,H", "SET 1,L", "SET 1,(HL)", "SET 1,A",
        "SET 2,B", "SET 2,C", "SET 2,D", "SET 2,E", "SET 2,H", "SET 2,L", "SET 2,(HL)", "SET 2,A", "SET 3,B", "SET 3,C", "SET 3,D", "SET 3,E", "SET 3,H", "SET 3,L", "SET 3,(HL)", "SET 3,A",
        "SET 4,B", "SET 4,C", "SET 4,D", "SET 4,E", "SET 4,H", "SET 4,L", "SET 4,(HL)", "SET 4,A", "SET 5,B", "SET 5,C", "SET 5,D", "SET 5,E", "SET 5,H", "SET 5,L", "SET 5,(HL)", "SET 5,A",
        "SET 6,B", "SET 6,C", "SET 6,D", "SET 6,E", "SET 6,H", "SET 6,L", "SET 6,(HL)", "SET 6,A", "SET 7,B", "SET 7,C", "SET 7,D", "SET 7,E", "SET 7,H", "SET 7,L", "SET 7,(HL)", "SET 7,A"
    };
}

std::vector<std::string> CPUZ80::getIXInstructionNames() {
    return {
        "", "", "", "", "INC B", "DEC B", "LD B,n", "", "", "ADD IX,BC", "", "", "INC C", "DEC C", "LD C,n", "",
        "", "", "", "", "INC D", "DEC D", "LD D,n", "", "", "ADD IX,DE", "", "", "INC E", "DEC E", "LD E,n", "",
        "", "LD IX,nn", "ld (NN),IX", "INC IX", "INC IXH", "DEC IXH", "LD IX,n", "", "", "ADD IX,IX", "LD IX,(nn)", "DEC IX", "INC IXL", "DEC IXL", "LD IXL,n", "",
        "", "", "", "", "INC (IX+d)", "DEC (IX+d)", "LD (IX+d),n", "", "", "ADD IX,SP", "", "", "INC A", "DEC A", "LD A,N", "",
        "LD B,B", "LD B,C", "LD B,D", "LD B,E", "LD B,IXH", "LD B,IXL", "LD B,(IX+d)", "LD B,A", "LD C,B", "LD C,C", "LD C,D", "LD C,E", "LD C,IXH", "LD C,IXL", "LD E,(IX+d)", "LD C,A",
        "LD D,B", "LD D,C", "LD D,D", "LD D,E", "LD D,IXH", "LD D,IXL", "LD D,(IX+d)", "LD D,A", "LD E,B", "LD E,C", "LD E,D", "LD E,E", "LD E,IXH", "LD E,IXL", "LD E,(IX+d)", "LD E,A",
        "LD IXH,B", "LD IXH,C", "LD IXH,D", "LD IXH,E", "LD IXH,IXH", "LX IDH,IXL", "LD H,(IX+d)", "LD IXH,A", "LD IXL,B", "LD IXL,C", "LD IXL,D", "LD IXL,E", "LD IXL,IXH", "LD IXL,IXL", "LD L,(IX+d)", "LD E,A",
        "LD (IX+d),B", "LD (IX+d),C", "LD (IX+d),D", "LD (IX+d),E", "LD (IX+d),H", "LD (IX+d),L", "", "LD (IX+d),A", "LD A,B", "LD A,C", "LD A,D", "LD A,E", "LD A,IXH", "LD A,IXL", "LD A,(IX+d)", "LD A,A",
        "ADD A,B", "ADD A,C", "ADD A,D", "ADD A,E", "ADD A,IXH", "ADD A,IXL", "ADD A,(IX+d)", "ADD A,A", "ADC A,B", "ADC A,C", "ADC A,D", "ADC A,E", "ADC A,IXH", "ADC A,IXL", "ADC A,(IX+d)", "ADC A,A",
        "SUB B", "SUB C", "SUB D", "SUB E", "SUX IXH", "SUX IXL", "SUB (IX+d)", "SUB A", "SBC A,B", "SBC A,C", "SBC A,D", "SBC A,E", "SBC A,IXH", "SBC A,IXL", "SBC A,(IX+d)", "SBC A,A",
        "AND B", "AND C", "AND D", "AND E", "AND IXH", "AND IXL", "AND (IX+d)", "AND A", "XOR B", "XOR C", "XOR D", "XOR E", "XOR IXH", "XOR IXL", "XOR (IX+d)", "XOR A",
        "OR B", "OR C", "OR D", "OR E", "OR IXH", "OR IXL", "OR (IX+d)", "OR A", "CP B", "CP C", "CP D", "CP E", "CP IXH", "CP IXL", "CP (IX+d)", "CP A",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "", "", "IX BIT", "", "", "", "",
        "", "POP IX", "", "EX (SP),IX", "", "PUSH IX", "", "", "", "JP (IX)", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "LD SP,IX", "", "", "", "", "", ""
    };
}

std::vector<std::string> CPUZ80::getIXBitInstructionNames() {
    return {
        "RLC (IX+d),B", "RLC (IX+d),C", "RLC (IX+d),D", "RLC (IX+d),E", "RLC (IX+d),H", "RLC (IX+d), L", "RLC (IX+d)", "RLC (X+d),A", "RRC (IX+d),B", "RRC (IX+d),C", "RRC (IX+d),D", "RRC (IX+d),E", "RRC (IX+d),H", "RRC (IX+d),L", "RRC (IX+d)", "RRC (IX+d),A",
        "RL (IX+d),B", "RL (IX+d),C", "RL (IX+d),D", "RL (IX+d),E", "RL (IX+d),H", "RL (IX+d),L", "RL (IX+d)", "RL, (IX+d),A", "RR (IX+d),B", "RR (IX+d),C", "RR (IX+d),D", "RR (IX+d),E", "RR (IX+d),H", "RR (IX+d),L", "RR (IX+d)", "RR (IX+d),A",
        "SLA (IX+d),B", "SLA (IX+d),C", "SLA (IX+d),D", "SLA (IX+d),E", "SLA (IX+d),H", "SLA (IX+d),L", "SLA (IX+d)", "SLA (IX+d),A", "SRA (IX+d),B", "SRA (IX+d),C", "SRA (IX+d),D", "SRA (IX+d),E", "SRA (IX+d),H", "SRA (IX+d),L", "SRA (IX+d)", "SRA (IX+d),A",
        "SLL (IX+d),B", "SLL (IX+d),C", "SLL (IX+d),D", "SLL (IX+d),E", "SLL (IX+d),H", "SLL (IX+d),L", "SLL (IX+d)", "SLL (IX+d),A", "SRL (IX+d),B", "SRL (IX+d),C", "SRL (IX+d),D", "SRL (IX+d),E", "SRL (IX+d),H", "SRL (IX+d),L", "SRL (IX+d)", "SRL (IX+d),A",
        "BIT 0,(IX+d),B", "BIT 0,(IX+d),C", "BIT 0,(IX+d),D", "BIT 0,(IX+d),E", "BIT 0,(IX+d),H", "BIT 0,(IX+d),L", "BIT 0,(IX+d)", "BIT 0,(IX+d),A", "BIT 1,(IX+d),B", "BIT 1,(IX+d),C", "BIT 1,(IX+d),D", "BIT 1,(IX+d),E", "BIT 1,(IX+d),H", "BIT 1,(IX+d),L", "BIT 1,(IX+d)", "BIT 1,(IX+d),A",
        "BIT 2,(IX+d),B", "BIT 2,(IX+d),C", "BIT 2,(IX+d),D", "BIT 2,(IX+d),E", "BIT 2,(IX+d),H", "BIT 2,(IX+d),L", "BIT 2,(IX+d)", "BIT 2,(IX+d),A", "BIT 3,(IX+d),B", "BIT 3,(IX+d),C", "BIT 3,(IX+d),D", "BIT 3,(IX+d),E", "BIT 3,(IX+d),H", "BIT 3,(IX+d),L", "BIT 3,(IX+d)", "BIT 3,(IX+d),A",
        "BIT 4,(IX+d),B", "BIT 4,(IX+d),C", "BIT 4,(IX+d),D", "BIT 4,(IX+d),E", "BIT 4,(IX+d),H", "BIT 4,(IX+d),L", "BIT 4,(IX+d)", "BIT 4,(IX+d),A", "BIT 5,(IX+d),B", "BIT 5,(IX+d),C", "BIT 5,(IX+d),D", "BIT 5,(IX+d),E", "BIT 5,(IX+d),H", "BIT 5,(IX+d),L", "BIT 5,(IX+d)", "BIT 5,(IX+d),A",
        "BIT 6,(IX+d),B", "BIT 6,(IX+d),C", "BIT 6,(IX+d),D", "BIT 6,(IX+d),E", "BIT 6,(IX+d),H", "BIT 6,(IX+d),L", "BIT 6,(IX+d)", "BIT 6,(IX+d),A", "BIT 7,(IX+d),B", "BIT 7,(IX+d),C", "BIT 7,(IX+d),D", "BIT 7,(IX+d),E", "BIT 7,(IX+d),H", "BIT 7,(IX+d),L", "BIT 7,(IX+d)", "BIT 7,(IX+d),A",
        "RES 0,(IX+d),B", "RES 0,(IX+d),C", "RES 0,(IX+d),D", "RES 0,(IX+d),E", "RES 0,(IX+d),H", "RES 0,(IX+d),L", "RES 0,(IX+d)", "RES 0,(IX+d),A", "RES 1,(IX+d),B", "RES 1,(IX+d),C", "RES 1,(IX+d),D", "RES 1,(IX+d),E", "RES 1,(IX+d),H", "RES 1,(IX+d),L", "RES 1,(IX+d)", "RES 1,(IX+d),A",
        "RES 2,(IX+d),B", "RES 2,(IX+d),C", "RES 2,(IX+d),D", "RES 2,(IX+d),E", "RES 2,(IX+d),H", "RES 2,(IX+d),L", "RES 2,(IX+d)", "RES 2,(IX+d),A", "RES 3,(IX+d),B", "RES 3,(IX+d),C", "RES 3,(IX+d),D", "RES 3,(IX+d),E", "RES 3,(IX+d),H", "RES 3,(IX+d),L", "RES 3,(IX+d)", "RES 3,(IX+d),A",
        "RES 4,(IX+d),B", "RES 4,(IX+d),C", "RES 4,(IX+d),D", "RES 4,(IX+d),E", "RES 4,(IX+d),H", "RES 4,(IX+d),L", "RES 4,(IX+d)", "RES 4,(IX+d),A", "RES 5,(IX+d),B", "RES 5,(IX+d),C", "RES 5,(IX+d),D", "RES 5,(IX+d),E", "RES 5,(IX+d),H", "RES 5,(IX+d),L", "RES 5,(IX+d)", "RES 5,(IX+d),A",
        "RES 6,(IX+d),B", "RES 6,(IX+d),C", "RES 6,(IX+d),D", "RES 6,(IX+d),E", "RES 6,(IX+d),H", "RES 6,(IX+d),L", "RES 6,(IX+d)", "RES 6,(IX+d),A", "RES 7,(IX+d),B", "RES 7,(IX+d),C", "RES 7,(IX+d),D", "RES 7,(IX+d),E", "RES 7,(IX+d),H", "RES 7,(IX+d),L", "RES 7,(IX+d)", "RES 7,(IX+d),A",
        "SET 0,(IX+d),B", "SET 0,(IX+d),C", "SET 0,(IX+d),D", "SET 0,(IX+d),E", "SET 0,(IX+d),H", "SET 0,(IX+d),L", "SET 0,(IX+d)", "SET 0,(IX+d),A", "SET 1,(IX+d),B", "SET 1,(IX+d),C", "SET 1,(IX+d),D", "SET 1,(IX+d),E", "SET 1,(IX+d),H", "SET 1,(IX+d),L", "SET 1,(IX+d)", "SET 1,(IX+d),A",
        "SET 2,(IX+d),B", "SET 2,(IX+d),C", "SET 2,(IX+d),D", "SET 2,(IX+d),E", "SET 2,(IX+d),H", "SET 2,(IX+d),L", "SET 2,(IX+d)", "SET 2,(IX+d),A", "SET 3,(IX+d),B", "SET 3,(IX+d),C", "SET 3,(IX+d),D", "SET 3,(IX+d),E", "SET 3,(IX+d),H", "SET 3,(IX+d),L", "SET 3,(IX+d)", "SET 3,(IX+d),A",
        "SET 4,(IX+d),B", "SET 4,(IX+d),C", "SET 4,(IX+d),D", "SET 4,(IX+d),E", "SET 4,(IX+d),H", "SET 4,(IX+d),L", "SET 4,(IX+d)", "SET 4,(IX+d),A", "SET 5,(IX+d),B", "SET 5,(IX+d),C", "SET 5,(IX+d),D", "SET 5,(IX+d),E", "SET 5,(IX+d),H", "SET 5,(IX+d),L", "SET 5,(IX+d)", "SET 5,(IX+d),A",
        "SET 6,(IX+d),B", "SET 6,(IX+d),C", "SET 6,(IX+d),D", "SET 6,(IX+d),E", "SET 6,(IX+d),H", "SET 6,(IX+d),L", "SET 6,(IX+d)", "SET 6,(IX+d),A", "SET 7,(IX+d),B", "SET 7,(IX+d),C", "SET 7,(IX+d),D", "SET 7,(IX+d),E", "SET 7,(IX+d),H", "SET 7,(IX+d),L", "SET 7,(IX+d)", "SET 7,(IX+d),A"
    };
}

std::vector<std::string> CPUZ80::getMiscInstructionNames() {
    return {
        "IN0 B,(n)", "OUT0 (n),B", "", "", "TST B", "", "", "", "IN0 C,(n)", "OUT0 (n),C", "", "", "TST C", "", "", "",
        "IN0 D,(n)", "OUT0 (n),D", "", "", "TST D", "", "", "", "IN0 E,(n)", "OUT0 (n),E", "", "", "TST E", "", "", "",
        "IN0 H,(n)", "OUT0 (n),H", "", "", "TST H", "", "", "", "IN0 L,(n)", "OUT0 (n),L", "", "", "TST L", "", "", "",
        "", "", "", "", "TST (hl)", "", "", "", "IN0 A,(n)", "OUT0 (n),A", "", "", "TST A", "", "", "",
        "IN B,(c)", "OUT (c),B", "SBC HL,BC", "LD (nn),BC", "NEG", "RETN", "IM 0", "LD I,A", "IN C,(c)", "OUT (c),C", "ADC HL,BC", "LD BC,(nn)", "MLT BC", "RETI", "", "LD R,A",
        "IN D,(c)", "OUT (c),D", "SBC HL,DE", "LD (nn),DE", "", "", "IM 1", "LD, A,I", "IN E,(c)", "OUT (c),E", "ADC HL,DE", "LD DE,(nn)", "MLT DE", "", "IM 2", "LD A,R",
        "IN H,(c)", "OUT (c),H", "SBC HL,HL", "LD (nn),HL", "TST n", "", "", "RRD", "IN L,(c)", "OUT (c),L", "ADC HL,HL", "LD HL,(nn)", "MLT HL", "", "", "RLD",
        "IN (c)", "OUT (c),0", "SBC HL,SP", "LD (nn),SP", "TSTIO n", "", "SLP", "", "IN A,(c)", "OUT (c),A", "ADC HL,SP", "LD SP,(nn)", "MLT SP", "", "", "",
        "", "", "", "OTIM", "", "", "", "", "", "", "", "OTDM", "", "", "", "",
        "", "", "", "OTIMR", "", "", "", "", "", "", "", "OTDMR", "", "", "", "",
        "LDI", "CPI", "INI", "OUTI", "", "", "", "", "LDD", "CPD", "IND", "OUTD", "", "", "", "",
        "LDIR", "CPIR", "INIR", "OTIR", "", "", "", "", "LDDR", "CPDR", "INDR", "OTDR", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
    };
}

std::vector<std::string> CPUZ80::getIYInstructionNames() {
    return {
        "", "", "", "", "INC B", "DEC B", "LD B,n", "", "", "ADD IY,BC", "", "", "INC C", "DEC C", "LD C,n", "",
        "", "", "", "", "INC D", "DEC D", "LD D,n", "", "", "ADD IY,DE", "", "", "INC E", "DEC E", "LD E,n", "",
        "", "LD IY,nn", "ld (NN),IY", "INC IY", "INC IYH", "DEC IYH", "LD IY,n", "", "", "ADD IY,IY", "LD IY,(nn)", "DEC IY", "INC IYL", "DEC IYL", "LD IYL,n", "",
        "", "", "", "", "INC (IY+d)", "DEC (IY+d)", "LD (IY+d),n", "", "", "ADD IY,SP", "", "", "INC A", "DEC A", "LD A,N", "",
        "LD B,B", "LD B,C", "LD B,D", "LD B,E", "LD B,IYH", "LD B,IYL", "LD B,(IY+d)", "LD B,A", "LD C,B", "LD C,C", "LD C,D", "LD C,E", "LD C,IYH", "LD C,IYL", "LD E,(IY+d)", "LD C,A",
        "LD D,B", "LD D,C", "LD D,D", "LD D,E", "LD D,IYH", "LD D,IYL", "LD D,(IY+d)", "LD D,A", "LD E,B", "LD E,C", "LD E,D", "LD E,E", "LD E,IYH", "LD E,IYL", "LD E,(IY+d)", "LD E,A",
        "LD IYH,B", "LD IYH,C", "LD IYH,D", "LD IYH,E", "LD IYH,IYH", "LX IDH,IYL", "LD H,(IY+d)", "LD IYH,A", "LD IYL,B", "LD IYL,C", "LD IYL,D", "LD IYL,E", "LD IYL,IYH", "LD IYL,IYL", "LD L,(IY+d)", "LD E,A",
        "LD (IY+d),B", "LD (IY+d),C", "LD (IY+d),D", "LD (IY+d),E", "LD (IY+d),H", "LD (IY+d),L", "", "LD (IY+d),A", "LD A,B", "LD A,C", "LD A,D", "LD A,E", "LD A,IYH", "LD A,IYL", "LD A,(IY+d)", "LD A,A",
        "ADD A,B", "ADD A,C", "ADD A,D", "ADD A,E", "ADD A,IYH", "ADD A,IYL", "ADD A,(IY+d)", "ADD A,A", "ADC A,B", "ADC A,C", "ADC A,D", "ADC A,E", "ADC A,IYH", "ADC A,IYL", "ADC A,(IY+d)", "ADC A,A",
        "SUB B", "SUB C", "SUB D", "SUB E", "SUX IYH", "SUX IYL", "SUB (IY+d)", "SUB A", "SBC A,B", "SBC A,C", "SBC A,D", "SBC A,E", "SBC A,IYH", "SBC A,IYL", "SBC A,(IY+d)", "SBC A,A",
        "AND B", "AND C", "AND D", "AND E", "AND IYH", "AND IYL", "AND (IY+d)", "AND A", "XOR B", "XOR C", "XOR D", "XOR E", "XOR IYH", "XOR IYL", "XOR (IY+d)", "XOR A",
        "OR B", "OR C", "OR D", "OR E", "OR IYH", "OR IYL", "OR (IY+d)", "OR A", "CP B", "CP C", "CP D", "CP E", "CP IYH", "CP IYL", "CP (IY+d)", "CP A",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "", "", "IY BIT", "", "", "", "",
        "", "POP IY", "", "EX (SP),IY", "", "PUSH IY", "", "", "", "JP (IY)", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "LD SP,IY", "", "", "", "", "", ""
    };
}

std::vector<std::string> CPUZ80::getIYBitInstructionNames() {
    return {
        "RLC (IY+d),B", "RLC (IY+d),C", "RLC (IY+d),D", "RLC (IY+d),E", "RLC (IY+d),H", "RLC (IY+d), L", "RLC (IY+d)", "RLC (X+d),A", "RRC (IY+d),B", "RRC (IY+d),C", "RRC (IY+d),D", "RRC (IY+d),E", "RRC (IY+d),H", "RRC (IY+d),L", "RRC (IY+d)", "RRC (IY+d),A",
        "RL (IY+d),B", "RL (IY+d),C", "RL (IY+d),D", "RL (IY+d),E", "RL (IY+d),H", "RL (IY+d),L", "RL (IY+d)", "RL, (IY+d),A", "RR (IY+d),B", "RR (IY+d),C", "RR (IY+d),D", "RR (IY+d),E", "RR (IY+d),H", "RR (IY+d),L", "RR (IY+d)", "RR (IY+d),A",
        "SLA (IY+d),B", "SLA (IY+d),C", "SLA (IY+d),D", "SLA (IY+d),E", "SLA (IY+d),H", "SLA (IY+d),L", "SLA (IY+d)", "SLA (IY+d),A", "SRA (IY+d),B", "SRA (IY+d),C", "SRA (IY+d),D", "SRA (IY+d),E", "SRA (IY+d),H", "SRA (IY+d),L", "SRA (IY+d)", "SRA (IY+d),A",
        "SLL (IY+d),B", "SLL (IY+d),C", "SLL (IY+d),D", "SLL (IY+d),E", "SLL (IY+d),H", "SLL (IY+d),L", "SLL (IY+d)", "SLL (IY+d),A", "SRL (IY+d),B", "SRL (IY+d),C", "SRL (IY+d),D", "SRL (IY+d),E", "SRL (IY+d),H", "SRL (IY+d),L", "SRL (IY+d)", "SRL (IY+d),A",
        "BIT 0,(IY+d),B", "BIT 0,(IY+d),C", "BIT 0,(IY+d),D", "BIT 0,(IY+d),E", "BIT 0,(IY+d),H", "BIT 0,(IY+d),L", "BIT 0,(IY+d)", "BIT 0,(IY+d),A", "BIT 1,(IY+d),B", "BIT 1,(IY+d),C", "BIT 1,(IY+d),D", "BIT 1,(IY+d),E", "BIT 1,(IY+d),H", "BIT 1,(IY+d),L", "BIT 1,(IY+d)", "BIT 1,(IY+d),A",
        "BIT 2,(IY+d),B", "BIT 2,(IY+d),C", "BIT 2,(IY+d),D", "BIT 2,(IY+d),E", "BIT 2,(IY+d),H", "BIT 2,(IY+d),L", "BIT 2,(IY+d)", "BIT 2,(IY+d),A", "BIT 3,(IY+d),B", "BIT 3,(IY+d),C", "BIT 3,(IY+d),D", "BIT 3,(IY+d),E", "BIT 3,(IY+d),H", "BIT 3,(IY+d),L", "BIT 3,(IY+d)", "BIT 3,(IY+d),A",
        "BIT 4,(IY+d),B", "BIT 4,(IY+d),C", "BIT 4,(IY+d),D", "BIT 4,(IY+d),E", "BIT 4,(IY+d),H", "BIT 4,(IY+d),L", "BIT 4,(IY+d)", "BIT 4,(IY+d),A", "BIT 5,(IY+d),B", "BIT 5,(IY+d),C", "BIT 5,(IY+d),D", "BIT 5,(IY+d),E", "BIT 5,(IY+d),H", "BIT 5,(IY+d),L", "BIT 5,(IY+d)", "BIT 5,(IY+d),A",
        "BIT 6,(IY+d),B", "BIT 6,(IY+d),C", "BIT 6,(IY+d),D", "BIT 6,(IY+d),E", "BIT 6,(IY+d),H", "BIT 6,(IY+d),L", "BIT 6,(IY+d)", "BIT 6,(IY+d),A", "BIT 7,(IY+d),B", "BIT 7,(IY+d),C", "BIT 7,(IY+d),D", "BIT 7,(IY+d),E", "BIT 7,(IY+d),H", "BIT 7,(IY+d),L", "BIT 7,(IY+d)", "BIT 7,(IY+d),A",
        "RES 0,(IY+d),B", "RES 0,(IY+d),C", "RES 0,(IY+d),D", "RES 0,(IY+d),E", "RES 0,(IY+d),H", "RES 0,(IY+d),L", "RES 0,(IY+d)", "RES 0,(IY+d),A", "RES 1,(IY+d),B", "RES 1,(IY+d),C", "RES 1,(IY+d),D", "RES 1,(IY+d),E", "RES 1,(IY+d),H", "RES 1,(IY+d),L", "RES 1,(IY+d)", "RES 1,(IY+d),A",
        "RES 2,(IY+d),B", "RES 2,(IY+d),C", "RES 2,(IY+d),D", "RES 2,(IY+d),E", "RES 2,(IY+d),H", "RES 2,(IY+d),L", "RES 2,(IY+d)", "RES 2,(IY+d),A", "RES 3,(IY+d),B", "RES 3,(IY+d),C", "RES 3,(IY+d),D", "RES 3,(IY+d),E", "RES 3,(IY+d),H", "RES 3,(IY+d),L", "RES 3,(IY+d)", "RES 3,(IY+d),A",
        "RES 4,(IY+d),B", "RES 4,(IY+d),C", "RES 4,(IY+d),D", "RES 4,(IY+d),E", "RES 4,(IY+d),H", "RES 4,(IY+d),L", "RES 4,(IY+d)", "RES 4,(IY+d),A", "RES 5,(IY+d),B", "RES 5,(IY+d),C", "RES 5,(IY+d),D", "RES 5,(IY+d),E", "RES 5,(IY+d),H", "RES 5,(IY+d),L", "RES 5,(IY+d)", "RES 5,(IY+d),A",
        "RES 6,(IY+d),B", "RES 6,(IY+d),C", "RES 6,(IY+d),D", "RES 6,(IY+d),E", "RES 6,(IY+d),H", "RES 6,(IY+d),L", "RES 6,(IY+d)", "RES 6,(IY+d),A", "RES 7,(IY+d),B", "RES 7,(IY+d),C", "RES 7,(IY+d),D", "RES 7,(IY+d),E", "RES 7,(IY+d),H", "RES 7,(IY+d),L", "RES 7,(IY+d)", "RES 7,(IY+d),A",
        "SET 0,(IY+d),B", "SET 0,(IY+d),C", "SET 0,(IY+d),D", "SET 0,(IY+d),E", "SET 0,(IY+d),H", "SET 0,(IY+d),L", "SET 0,(IY+d)", "SET 0,(IY+d),A", "SET 1,(IY+d),B", "SET 1,(IY+d),C", "SET 1,(IY+d),D", "SET 1,(IY+d),E", "SET 1,(IY+d),H", "SET 1,(IY+d),L", "SET 1,(IY+d)", "SET 1,(IY+d),A",
        "SET 2,(IY+d),B", "SET 2,(IY+d),C", "SET 2,(IY+d),D", "SET 2,(IY+d),E", "SET 2,(IY+d),H", "SET 2,(IY+d),L", "SET 2,(IY+d)", "SET 2,(IY+d),A", "SET 3,(IY+d),B", "SET 3,(IY+d),C", "SET 3,(IY+d),D", "SET 3,(IY+d),E", "SET 3,(IY+d),H", "SET 3,(IY+d),L", "SET 3,(IY+d)", "SET 3,(IY+d),A",
        "SET 4,(IY+d),B", "SET 4,(IY+d),C", "SET 4,(IY+d),D", "SET 4,(IY+d),E", "SET 4,(IY+d),H", "SET 4,(IY+d),L", "SET 4,(IY+d)", "SET 4,(IY+d),A", "SET 5,(IY+d),B", "SET 5,(IY+d),C", "SET 5,(IY+d),D", "SET 5,(IY+d),E", "SET 5,(IY+d),H", "SET 5,(IY+d),L", "SET 5,(IY+d)", "SET 5,(IY+d),A",
        "SET 6,(IY+d),B", "SET 6,(IY+d),C", "SET 6,(IY+d),D", "SET 6,(IY+d),E", "SET 6,(IY+d),H", "SET 6,(IY+d),L", "SET 6,(IY+d)", "SET 6,(IY+d),A", "SET 7,(IY+d),B", "SET 7,(IY+d),C", "SET 7,(IY+d),D", "SET 7,(IY+d),E", "SET 7,(IY+d),H", "SET 7,(IY+d),L", "SET 7,(IY+d)", "SET 7,(IY+d),A"
    };
}

std::string CPUZ80::getInstructionName(unsigned short opcode, unsigned short middleOpcode, unsigned short lastOpcode) {
    if (opcode == 0xCB) {
        return getBitInstructionNames()[middleOpcode];
    }

    if (opcode == 0xDD) {
        if (middleOpcode == 0xCB) {
            return getIXBitInstructionNames()[lastOpcode];
        }
        return getIXInstructionNames()[middleOpcode];
    }

    if (opcode == 0xED) {
        return getMiscInstructionNames()[middleOpcode];
    }

    if (opcode == 0xFD) {
        if (middleOpcode == 0xCB) {
            return getIYBitInstructionNames()[lastOpcode];
        }
        return getIYInstructionNames()[middleOpcode];
    }

    return getStandardInstructionNames()[opcode];
}

#endif //MASTALGIA_Z80INSTRUCTIONNAMES_CPP
