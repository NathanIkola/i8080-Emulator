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
		// Get condition code
		//******************************
		uint8_t get_ccc() noexcept;

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
		// Unimplemented instructions
		//******************************
		inline uint8_t bad(const uint8_t& arg) noexcept { abort(); }

		//******************************
		// Read in 2 bytes
		//******************************
		inline uint16_t read16() noexcept { PC += 2; return ((file.get()) | (file.get() << 8)); }

		//*******************************
		// Read in 1 byte
		//*******************************
		inline uint8_t read8() noexcept { ++PC; return file.get(); }
	};
}