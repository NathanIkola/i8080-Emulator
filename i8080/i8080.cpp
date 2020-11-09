//**************************************
// i8080.cpp
//
// Holds the definition of the i8080
// CPU component of my emulator
//
// Author: Nathan Ikola
// nathan.ikola@gmail.com
//**************************************
#include "i8080.h"

#include <assert.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "opcodes.h"

//**************************************
// Get the RP from an instruction
//**************************************
inline uint8_t rp(const uint8_t& in)
{
	return (in >> 4) & 3;
}

//**************************************
// Get the destination from an opcode
//**************************************
inline uint8_t dest(const uint8_t& in)
{
	return (in >> 3) & 7;
}

//**************************************
// Get the source from an opcode
//**************************************
inline uint8_t source(const uint8_t& in)
{
	return in & 7;
}

namespace i8080
{
	namespace flags
	{
		uint8_t C = 1 << 0;
		// 1 is always 1
		uint8_t P = 1 << 2;
		// 3 is always 0
		uint8_t A = 1 << 4;
		// 5 is always 0
		uint8_t Z = 1 << 6;
		uint8_t S = 1 << 7;
	}

	//**********************************
	// Run one cycle
	//**********************************
	bool i8080::step() noexcept
	{
		static uint8_t wait = 0;
		static uint8_t op = 0;

		// implement cycle counting
		if (wait > 0)
		{
			--wait;
			return true;
		}
		else
		{
			op = file.get();

			std::stringstream lineno;
			lineno << "0x" << std::setfill('0') << std::setw(4) << std::hex << PC << "  ";
			lineno << "0x" << std::setfill('0') << std::setw(4) << std::hex << static_cast<uint16_t>(op) << "  ";
			std::cout << lineno.str() << std::endl;

			uint8_t result = (*this.*operations[op])(op);
			++PC;
			// result of 0 means success, and take the dur duration
			if (result == 0)
				wait = opcodes[op].dur;
			// result of 1 means success, and take the alt duration
			else if (result == 1)
				wait = opcodes[op].alt;
			else
				return false;
			return true;
		}
	}

	//******************************
	// Get a register pair
	//******************************
	uint16_t i8080::read_rp(const uint8_t& rp) noexcept
	{
		switch (rp)
		{
		case 0:
			return (B << 8) | C;
		case 1:
			return (D << 8) | E;
		case 2:
			return (H << 8) | L;
		case 3:
			return SP;
		}
		return 0;
	}

	//******************************
	// Get a register pair
	//******************************
	void i8080::write_rp(const uint8_t& rp, const uint16_t val) noexcept
	{
		switch (rp)
		{
		case 0:
			B = val >> 8;
			C = val & 15;
			break;
		case 1:
			D = val >> 8;
			E = val & 15;
			break;
		case 2:
			H = val >> 8;
			L = val & 15;
			break;
		case 3:
			SP = val;
			break;
		}
	}

	//******************************
	// Get a register by number
	//******************************
	uint8_t& i8080::get_reg(const uint8_t& arg) noexcept
	{
		switch (arg)
		{
		case 0:
			return B;
		case 1:
			return C;
		case 2:
			return D;
		case 3:
			return E;
		case 4:
			return H;
		case 5:
			return L;
		case 6:
			// return the memory reference at location H:L
			return memory[read_rp(2)];
		case 7:
			return A;
		}

		// we shouldn't get here
		assert(false);
	}

	//**********************************
	// JMP instruction
	//**********************************
	uint8_t i8080::jmp(const uint8_t& arg) noexcept
	{
		// read in the next two bytes since they are a 16 bit address
		uint16_t addr = read16();
		// subtract 1 from PC because it will be incremented after this executes
		int32_t diff = addr - (PC + 1);
		file.seekg(diff, file.cur);

		PC = addr - 1;
		return 0;
	}

	//******************************
	// LXI instruction
	//******************************
	uint8_t i8080::lxi(const uint8_t& arg) noexcept
	{
		// read in the value we are going to write
		uint16_t val = read16();
		write_rp(rp(arg), val);
		return 0;
	}

	//******************************
	// MVI instruction
	//******************************
	uint8_t i8080::mvi(const uint8_t& arg) noexcept
	{
		// get a reference to the register we want
		uint8_t& reg = get_reg(dest(arg));
		// get the immediate value we are writing
		uint8_t val = read8();
		// assign the value we read in to that register
		reg = val;
		return 0;
	}

	//**********************************
	// Constructor
	//**********************************
	i8080::i8080(const char* filename, uint16_t size) 
		: memory(nullptr), file(filename, std::ios_base::binary), operations()
	{
		if (!file.is_open()) throw -1;

		using namespace i8080;

		// first allocate the amount of memory that we want
		memory = new uint8_t[static_cast<int>(size) + 1];

		// start initializing the array of operations
		// by filling it with operations which crash
		// the system
		operations.fill(&i8080::bad);

		// now start explicitly filling it
		operations[0x00] = &i8080::nop;
		operations[0x06] = &i8080::mvi;
		operations[0x31] = &i8080::lxi;
		operations[0xC3] = &i8080::jmp;
	}
}