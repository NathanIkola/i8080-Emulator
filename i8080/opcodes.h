//**************************************
// opcodes.h
//
// This file holds the list of opcodes
// that are used in the Intel 8080
//
// Author: Nathan Ikola
// nathan.ikola@gmail.com
//**************************************
#pragma once
#include <functional>

// each opcode has the following attributes
//     5 flag bits
//	   5 duration bits (max of 17)
//	   4 alternate duration bits (max of 11)
//	   2 instruction length bits (max of 3)

namespace i8080
{
	// an opcode structure
	struct opcode
	{
		// default initializes to NOP
		opcode() : len(1), alt(0), dur(4) {}
		opcode(unsigned char len, unsigned char dur, unsigned char alt) : len(len), alt(alt), dur(dur) {}
		opcode(unsigned char len, unsigned char dur) : len(len), alt(0), dur(dur) {}
		unsigned char len : 2;
		unsigned char alt : 4;
		unsigned char dur : 5;
	};

	// define the opcode list
	const opcode opcodes[256] =
	{
		// 0x
		// NOP
		opcode{},
		// LXI B,d16
		opcode{3, 10},
		// STAX B
		opcode{1, 7},
		// INX B
		opcode{1, 5},
		// INR B
		opcode{1, 5},
		// DCR B
		opcode{1, 5},
		// MVI B,d8
		opcode{2, 7},
		// RLC
		opcode{1, 4},
		// NOP
		opcode{1, 4},
		// DAD B
		opcode{1, 10},
		// LDAX B
		opcode{1, 7},
		// DCX B
		opcode{1, 5},
		// INR C
		opcode{1, 5},
		// DCR C
		opcode{1, 5},
		// MVI C,d8
		opcode{2, 7},
		// RRC
		opcode{1, 4},

		// 1x
		// *NOP
		opcode{1, 4},
		// LXI D,d16
		opcode{3, 10},
		// STAX D
		opcode{1, 7},
		// INX D
		opcode{1, 5},
		// INR D
		opcode{1, 5},
		// DCR D
		opcode{1, 5},
		// MVI D,d8
		opcode{2, 7},
		// RAL
		opcode{1, 4},
		// *NOP
		opcode{1, 4},
		// DAD D
		opcode{1, 10},
		// LDAX D
		opcode{1, 7},
		// DCX D
		opcode{1, 5},
		// INR E
		opcode{1, 5},
		// DCR E
		opcode{1, 5},
		// MVI E,d8
		opcode{2, 7},
		// RAR
		opcode{1, 4},
		
		// 2x
		// *NOP
		opcode{1, 4},
		// LXI H,d16
		opcode{3, 10},
		// SHLD a16
		opcode{3, 16},
		// INX H
		opcode{1, 5},
		// INR H
		opcode{1, 5},
		// DCR H
		opcode{1, 5},
		// MVI H,d7
		opcode{2, 7},
		// DAA
		opcode{1, 4},
		// *NOP
		opcode{1, 4},
		// DAD H
		opcode{1, 10},
		// LHLD a16
		opcode{3, 16},
		// DCX H
		opcode{1, 5},
		// INR L
		opcode{1, 5},
		// DCR L
		opcode{1, 5},
		// MVI L,d8
		opcode{2, 7},
		// CMA
		opcode{1, 4},
		
		// 3x
		// *NOP
		opcode{1, 4},
		// LCI SP,d16
		opcode{3, 10},
		// STA a16
		opcode{3, 13},
		// INX SP
		opcode{1, 5},
		// INR M
		opcode{1, 10},
		// DCR M
		opcode{1, 10},
		// MVI M,d8
		opcode{2, 10},
		// STC
		opcode{1, 4},
		// *NOP
		opcode{1, 4},
		// DAD SP
		opcode{1, 10},
		// LDA a16
		opcode{3, 13},
		// DCX SP
		opcode{1, 5},
		// INR A
		opcode{1, 5},
		// DCR A
		opcode{1, 5},
		// MVI A,d8
		opcode{2, 7},
		// CMC
		opcode{1, 4},
		
		// 4x
		// MOV B,B
		opcode{ 1, 5},
		// MOV B,C
		opcode{ 1, 5},
		// MOV B,D
		opcode{ 1, 5},
		// MOV B,E
		opcode{ 1, 5},
		// MOV B,H
		opcode{ 1, 5},
		// MOV B,L
		opcode{ 1, 5},
		// MOV B,M
		opcode{ 1, 7},
		// MOV B,A
		opcode{ 1, 5},
		// MOV C,B
		opcode{ 1, 5},
		// MOV C,C
		opcode{ 1, 5},
		// MOV C,D
		opcode{ 1, 5},
		// MOV C,E
		opcode{ 1, 5},
		// MOV C,H
		opcode{ 1, 5},
		// MOV C,L
		opcode{ 1, 5},
		// MOV C,M
		opcode{ 1, 7},
		// MOV C,A
		opcode{ 1, 5},
		
		// 5x
		// MOV D,B
		opcode{ 1, 5 },
		// MOV D,C
		opcode{ 1, 5 },
		// MOV D,D
		opcode{ 1, 5 },
		// MOV D,E
		opcode{ 1, 5 },
		// MOV D,H
		opcode{ 1, 5 },
		// MOV D,L
		opcode{ 1, 5 },
		// MOV D,M
		opcode{ 1, 7 },
		// MOV D,A
		opcode{ 1, 5 },
		// MOV E,B
		opcode{ 1, 5 },
		// MOV E,C
		opcode{ 1, 5 },
		// MOV E,D
		opcode{ 1, 5 },
		// MOV E,E
		opcode{ 1, 5 },
		// MOV E,H
		opcode{ 1, 5 },
		// MOV E,L
		opcode{ 1, 5 },
		// MOV E,M
		opcode{ 1, 7 },
		// MOV E,A
		opcode{ 1, 5 },
		
		// 6x
		// MOV H,B
		opcode{ 1, 5 },
		// MOV H,C
		opcode{ 1, 5 },
		// MOV H,D
		opcode{ 1, 5 },
		// MOV H,E
		opcode{ 1, 5 },
		// MOV H,H
		opcode{ 1, 5 },
		// MOV H,L
		opcode{ 1, 5 },
		// MOV H,M
		opcode{ 1, 7 },
		// MOV H,A
		opcode{ 1, 5 },
		// MOV L,B
		opcode{ 1, 5 },
		// MOV L,C
		opcode{ 1, 5 },
		// MOV L,D
		opcode{ 1, 5 },
		// MOV L,E
		opcode{ 1, 5 },
		// MOV L,H
		opcode{ 1, 5 },
		// MOV L,L
		opcode{ 1, 5 },
		// MOV L,M
		opcode{ 1, 7 },
		// MOV L,A
		opcode{ 1, 5 },
		
		// 7x
		// MOV M,B
		opcode{ 1, 7},
		// MOV M,C
		opcode{ 1, 7},
		// MOV M,D
		opcode{ 1, 7},
		// MOV M,E
		opcode{ 1, 7},
		// MOV M,H
		opcode{ 1, 7},
		// MOV M,L
		opcode{ 1, 7},
		// HLT
		opcode{ 1, 7},
		// MOV M,A
		opcode{ 1, 7},
		// MOV A,B
		opcode{ 1, 5},
		// MOV A,C
		opcode{ 1, 5},
		// MOV A,D
		opcode{ 1, 5},
		// MOV A,E
		opcode{ 1, 5},
		// MOV A,H
		opcode{ 1, 5},
		// MOV A,L
		opcode{ 1, 5},
		// MOV A,M
		opcode{ 1, 7},
		// MOV A,A
		opcode{ 1, 5},
		
		// 8x
		// ADD B
		opcode{ 1, 4},
		// ADD C
		opcode{ 1, 4},
		// ADD D
		opcode{ 1, 4},
		// ADD E
		opcode{ 1, 4},
		// ADD H
		opcode{ 1, 4},
		// ADD L
		opcode{ 1, 4},
		// ADD M
		opcode{ 1, 7},
		// ADD A
		opcode{ 1, 4},
		// ADC B
		opcode{ 1, 4},
		// ADC C
		opcode{ 1, 4},
		// ADC D
		opcode{ 1, 4},
		// ADC E
		opcode{ 1, 4},
		// ADC H
		opcode{ 1, 4},
		// ADC L
		opcode{ 1, 4},
		// ADC M
		opcode{ 1, 7},
		// ADC A
		opcode{ 1, 4},
		
		// 9x
		// SUB B
		opcode{ 1, 4 },
		// SUB C
		opcode{ 1, 4 },
		// SUB D
		opcode{ 1, 4 },
		// SUB E
		opcode{ 1, 4 },
		// SUB H
		opcode{ 1, 4 },
		// SUB L
		opcode{ 1, 4 },
		// SUB M
		opcode{ 1, 7 },
		// SUB A
		opcode{ 1, 4 },
		// SBB B
		opcode{ 1, 4 },
		// SBB C
		opcode{ 1, 4 },
		// SBB D
		opcode{ 1, 4 },
		// SBB E
		opcode{ 1, 4 },
		// SBB H
		opcode{ 1, 4 },
		// SBB L
		opcode{ 1, 4 },
		// SBB M
		opcode{ 1, 7 },
		// SBB A
		opcode{ 1, 4 },
		
		// Ax
		// ANA B
		opcode{ 1, 4 },
		// ANA C
		opcode{ 1, 4 },
		// ANA D
		opcode{ 1, 4 },
		// ANA E
		opcode{ 1, 4 },
		// ANA H
		opcode{ 1, 4 },
		// ANA L
		opcode{ 1, 4 },
		// ANA M
		opcode{ 1, 7 },
		// ANA A
		opcode{ 1, 4 },
		// XRA B
		opcode{ 1, 4 },
		// XRA C
		opcode{ 1, 4 },
		// XRA D
		opcode{ 1, 4 },
		// XRA E
		opcode{ 1, 4 },
		// XRA H
		opcode{ 1, 4 },
		// XRA L
		opcode{ 1, 4 },
		// XRA M
		opcode{ 1, 7 },
		// XRA A
		opcode{ 1, 4 },
		
		// Bx
		// ORA B
		opcode{ 1, 4 },
		// ORA C
		opcode{ 1, 4 },
		// ORA D
		opcode{ 1, 4 },
		// ORA E
		opcode{ 1, 4 },
		// ORA H
		opcode{ 1, 4 },
		// ORA L
		opcode{ 1, 4 },
		// ORA M
		opcode{ 1, 7 },
		// ORA A
		opcode{ 1, 4 },
		// CMP B
		opcode{ 1, 4 },
		// CMP C
		opcode{ 1, 4 },
		// CMP D
		opcode{ 1, 4 },
		// CMP E
		opcode{ 1, 4 },
		// CMP H
		opcode{ 1, 4 },
		// CMP L
		opcode{ 1, 4 },
		// CMP M
		opcode{ 1, 7 },
		// CMP A
		opcode{ 1, 4 },
		
		// Cx
		// RNZ
		opcode{1, 11, 5},
		// POP B
		opcode{1, 10},
		// JNZ a16
		opcode{3, 10},
		// JMP a16
		opcode{3, 10},
		// CNZ a16
		opcode{3, 17, 11},
		// PUSH B
		opcode{1, 11},
		// ADI d8
		opcode{2, 7},
		// RST 0
		opcode{1, 11},
		// RZ
		opcode{1, 11, 5},
		// RET
		opcode{1, 10},
		// JZ a16
		opcode{3, 10},
		// *JMP a16
		opcode{3, 10},
		// CZ a16
		opcode{3, 17, 11},
		// CALL a16
		opcode{3, 17},
		// ACI d8
		opcode{2, 7},
		// RST 1
		opcode{1, 11},
		
		// Dx
		// RNC
		opcode{ 1, 11, 5 },
		// POP D
		opcode{ 1, 10 },
		// JNC a16
		opcode{ 3, 10 },
		// OUT d8
		opcode{ 2, 10 },
		// CNC a16
		opcode{ 3, 17, 11 },
		// PUSH D
		opcode{ 1, 11 },
		// SUI d8
		opcode{ 2, 7 },
		// RST 2
		opcode{ 1, 11 },
		// RC
		opcode{ 1, 11, 5 },
		// *RET
		opcode{ 1, 10 },
		// JC a16
		opcode{ 3, 10 },
		// IN d8
		opcode{ 2, 10 },
		// CC a16
		opcode{ 3, 17, 11 },
		// *CALL a16
		opcode{ 3, 17 },
		// SBI d8
		opcode{ 2, 7 },
		// RST 3
		opcode{ 1, 11 },
		
		// Ex
		// RPO
		opcode{ 1, 11, 5 },
		// POP H
		opcode{ 1, 10 },
		// JPO a16
		opcode{ 3, 10 },
		// XTHL
		opcode{ 1, 18 },
		// CPO a16
		opcode{ 3, 17, 11 },
		// PUSH H
		opcode{ 1, 11 },
		// ANI d8
		opcode{ 2, 7 },
		// RST 4
		opcode{ 1, 11 },
		// RPE
		opcode{ 1, 11, 5 },
		// PCHL
		opcode{ 1, 5},
		// JPE a16
		opcode{ 3, 10 },
		// XCHG
		opcode{ 1, 5 },
		// CPE a16
		opcode{ 3, 17, 11 },
		// *CALL a16
		opcode{ 3, 17 },
		// XRI d8
		opcode{ 2, 7 },
		// RST 5
		opcode{ 1, 11 },
		
		// Fx
		// RP
		opcode{ 1, 11, 5 },
		// POP PSW
		opcode{ 1, 10 },
		// JP a16
		opcode{ 3, 10 },
		// DI
		opcode{ 1, 4 },
		// CP a16
		opcode{ 3, 17, 11 },
		// PUSH PSW
		opcode{ 1, 11 },
		// ORI d8
		opcode{ 2, 7 },
		// RST 6
		opcode{ 1, 11 },
		// RM
		opcode{ 1, 11, 5 },
		// SPHL
		opcode{ 1, 5 },
		// JM a16
		opcode{ 3, 10 },
		// EI d8
		opcode{ 1, 4 },
		// CM a16
		opcode{ 3, 17, 11 },
		// *CALL a16
		opcode{ 3, 17 },
		// CPI d8
		opcode{ 2, 7},
		// RST 7
		opcode{ 1, 11 },
	};
}