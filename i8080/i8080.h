//**************************************
// i8080.h
//
// Holds the declaration of the Intel
// 8080 CPU emulator
//
// Author: Nathan Ikola
// nathan.ikola@gmail.com
//**************************************
#pragma once

#include <fstream>
#include <array>
#include <cstdint>

#include <iomanip>
#include <iostream>
#include <sstream>

namespace i8080
{
	class i8080 final
	{
	public:
		//******************************
		// Constructor
		//******************************
		i8080(const char* filename, uint16_t size = 0xFFFF);

		//******************************
		// Destructor
		//******************************
		inline ~i8080() noexcept { delete[] memory; }

		//******************************
		// Run the emulation
		//******************************
		void run() noexcept
		{
			// simply step forever until the CPU halts 
			while (step());
		}
	private:
		// define the registers
		// accumulator
		uint8_t A = 0;
		uint8_t B = 0;
		uint8_t C = 0;
		uint8_t D = 0;
		uint8_t E = 0;
		// flags register
		uint8_t F = 2;
		uint8_t H = 0;
		uint8_t L = 0;
		uint16_t PC = 0;
		uint16_t SP = PC;

		uint16_t address_bus = 0;
		uint8_t data_bus = 0;

		uint8_t* memory;

		std::ifstream file;

		// map functions to opcodes
		std::array<uint8_t(i8080::i8080::*)(const uint8_t&), 256> operations;

		//******************************
		// Run one cycle
		//******************************
		bool step() noexcept;

		//******************************
		// Get a register pair
		//******************************
		uint16_t read_rp(const uint8_t& rp) noexcept;

		//******************************
		// Write a register pair
		//******************************
		void write_rp(const uint8_t& rp, const uint16_t val) noexcept;

		//******************************
		// Get a register by number
		//******************************
		uint8_t& get_reg(const uint8_t& arg) noexcept;

		//******************************
		// NOP instruction
		//******************************
		inline uint8_t nop(const uint8_t& arg) noexcept { return 0; };

		//******************************
		// JMP instruction
		//******************************
		uint8_t jmp(const uint8_t& arg) noexcept;

		//******************************
		// LXI instruction
		//******************************
		uint8_t lxi(const uint8_t& arg) noexcept;

		//******************************
		// MVI instruction
		//******************************
		uint8_t mvi(const uint8_t& arg) noexcept;

		//******************************
		// CALL instruction
		//******************************
		uint8_t call(const uint8_t& arg) noexcept;

		//******************************
		// LDAX instruction
		//******************************
		uint8_t ldax(const uint8_t& arg) noexcept;

		//******************************
		// MOV instruction
		//******************************
		uint8_t mov(const uint8_t& arg) noexcept;

		//******************************
		// INX instruction
		//******************************
		uint8_t inx(const uint8_t& arg) noexcept;

		//******************************
		// DCR instruction
		//******************************
		uint8_t dcr(const uint8_t& arg) noexcept;

		//******************************
		// Conditional JMP instruction
		//******************************
		uint8_t jc(const uint8_t& arg) noexcept;

		//******************************
		// RET instruction
		//******************************
		uint8_t ret(const uint8_t& arg) noexcept;

		//******************************
		// CPI instruction
		//******************************
		uint8_t cpi(const uint8_t& arg) noexcept;

		//******************************
		// PUSH instruction
		//******************************
		uint8_t push(const uint8_t& arg) noexcept;

		//******************************
		// POP instruction
		//******************************
		uint8_t pop(const uint8_t& arg) noexcept;

		//******************************
		// HLT instruction
		//******************************
		inline uint8_t hlt(const uint8_t& arg) noexcept { return 2; }

		//******************************
		// DAD instruction
		//******************************
		uint8_t dad(const uint8_t& arg) noexcept;

		//******************************
		// EXCHG instruction
		//******************************
		uint8_t exchg(const uint8_t& arg) noexcept;

		//******************************
		// Unimplemented instructions
		//******************************
		inline uint8_t bad(const uint8_t& arg) noexcept 
		{
			std::stringstream lineno;
			lineno << "0x" << std::setfill('0') << std::setw(4) << std::hex << PC - 1 << "  ";
			lineno << "0x" << std::setfill('0') << std::setw(4) << std::hex << static_cast<uint16_t>(arg) << "  ";
			std::cout << lineno.str() << std::endl;
			abort(); 
		}

		//******************************
		// Read in 2 bytes
		//******************************
		inline uint16_t read16() noexcept { PC += 2; return ((memory[PC-2]) | (memory[PC-1] << 8)); }

		//*******************************
		// Read in 1 byte
		//*******************************
		inline uint8_t read8() noexcept { return memory[PC++]; }

		//*******************************
		// Load the program
		//*******************************
		void load_program(uint16_t offset = 0) noexcept;
	};
}