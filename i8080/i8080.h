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
		i8080(const char* filename, uint16_t size = 0xFFFF, uint16_t offset = 0x0);

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

		// debug information for the current step we are on
		uint16_t current_step = 0;

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
		// OUT instruction
		//******************************
		uint8_t out(const uint8_t& arg) noexcept;

		//******************************
		// RRC instruction
		//******************************
		uint8_t rrc(const uint8_t& arg) noexcept;

		//******************************
		// ANI instruction
		//******************************
		uint8_t ani(const uint8_t& arg) noexcept;

		//******************************
		// ADI instruction
		//******************************
		uint8_t adi(const uint8_t& arg) noexcept;

		//******************************
		// LDA instruction
		//******************************
		uint8_t lda(const uint8_t& arg) noexcept;

		//******************************
		// ANA instruction
		//******************************
		uint8_t ana(const uint8_t& arg) noexcept;

		//******************************
		// Conditional RET instruction
		//******************************
		uint8_t rc(const uint8_t& arg) noexcept;

		//******************************
		// ACI instruction
		//******************************
		uint8_t aci(const uint8_t& arg) noexcept;

		//******************************
		// SUI instruction
		//******************************
		uint8_t sui(const uint8_t& arg) noexcept;

		//******************************
		// SBI instruction
		//******************************
		uint8_t sbi(const uint8_t& arg) noexcept;

		//******************************
		// ORI instruction
		//******************************
		uint8_t ori(const uint8_t& arg) noexcept;

		//******************************
		// XRI instruction
		//******************************
		uint8_t xri(const uint8_t& arg) noexcept;

		//******************************
		// Conditional CALL instruction
		//******************************
		uint8_t cc(const uint8_t& arg) noexcept;

		//******************************
		// INR (increment register)
		//******************************
		uint8_t inr(const uint8_t& arg) noexcept;

		//******************************
		// XRA (XOR A with register)
		//******************************
		uint8_t xra(const uint8_t& arg) noexcept;

		//******************************
		// ADD register or memory to A
		//******************************
		uint8_t add(const uint8_t& arg) noexcept;

		//******************************
		// SUB register or memory to A
		//******************************
		uint8_t sub(const uint8_t& arg) noexcept;

		//******************************
		// Add register to A with carry
		//******************************
		uint8_t adc(const uint8_t& arg) noexcept;

		//******************************
		// Sub register from A with borrow
		//******************************
		uint8_t sbb(const uint8_t& arg) noexcept;

		//******************************
		// OR A with register
		//******************************
		uint8_t ora(const uint8_t& arg) noexcept;

		//******************************
		// Compare with accumulator
		//******************************
		uint8_t cmp(const uint8_t& arg) noexcept;

		//******************************
		// Decrement register pair
		//******************************
		uint8_t dcx(const uint8_t& arg) noexcept;

		//******************************
		// Store A in memory location
		//******************************
		uint8_t sta(const uint8_t& arg) noexcept;

		//******************************
		// Load H and L direct
		//******************************
		uint8_t lhld(const uint8_t& arg) noexcept;

		//******************************
		// Store H and L direct
		//******************************
		uint8_t shld(const uint8_t& arg) noexcept;

		//******************************
		// Store accumulator in memory
		//******************************
		uint8_t stax(const uint8_t& arg) noexcept;

		//******************************
		// Set the carry bit
		//******************************
		uint8_t stc(const uint8_t& arg) noexcept;

		//******************************
		// Complement the carry bit
		//******************************
		uint8_t cmc(const uint8_t& arg) noexcept;

		//******************************
		// Complement A
		//******************************
		uint8_t cma(const uint8_t& arg) noexcept;

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