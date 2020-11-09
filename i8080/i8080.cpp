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

//**************************************
// Get the condition code from an opcode
//**************************************
inline uint8_t ccc(const uint8_t& in)
{
	return ((in >> 3) & 7);
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
			op = read8();

			std::stringstream lineno;
			lineno << "0x" << std::setfill('0') << std::setw(4) << std::hex << PC - 1 << "  ";
			lineno << "0x" << std::setfill('0') << std::setw(4) << std::hex << static_cast<uint16_t>(op) << "  ";
			std::cout << lineno.str() << std::endl;

			uint8_t result = (*this.*operations[op])(op);
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
			C = val & 0xFF;
			break;
		case 1:
			D = val >> 8;
			E = val & 0xFF;
			break;
		case 2:
			H = val >> 8;
			L = val & 0xFF;
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
		abort();
	}

	//**********************************
	// JMP instruction
	//**********************************
	uint8_t i8080::jmp(const uint8_t& arg) noexcept
	{
		// read in the next two bytes since they are a 16 bit address
		uint16_t addr = read16();

		PC = addr;
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
	// CALL instruction
	//**********************************
	uint8_t i8080::call(const uint8_t& arg) noexcept
	{
		// get the address we are jumping to
		uint16_t target = read16();
		// save our return address
		uint16_t ret = PC;
		PC = target;
		// store our return address on the stack
		assert(SP > 1);
		SP -= 2;
		memory[SP+2] = (ret >> 8) & 0xFF;
		memory[SP+1] = (ret & 0xFF);
		return 0;
	}

	//******************************
	// LDAX instruction
	//******************************
	uint8_t i8080::ldax(const uint8_t& arg) noexcept
	{
		// get the address we are loading from
		uint16_t addr = read_rp(rp(arg));
		// put it in the accumulator
		A = memory[addr];
		return 0;
	}

	//******************************
	// MOV instruction
	//******************************
	uint8_t i8080::mov(const uint8_t& arg) noexcept
	{
		uint8_t val = get_reg(source(arg));
		uint8_t& reg = get_reg(dest(arg));
		reg = val;
		return 0;
	}

	//******************************
	// INX instruction
	//******************************
	uint8_t i8080::inx(const uint8_t& arg) noexcept
	{
		// get our value
		uint8_t reg = rp(arg);
		uint16_t val = read_rp(reg) + 1;
		write_rp(reg, val);
		return 0;
	}

	//******************************
	// DCR instruction
	//******************************
	uint8_t i8080::dcr(const uint8_t& arg) noexcept
	{
		// reset the flags
		F &= C;

		// get our value
		uint8_t& val = get_reg(dest(arg));
		// if subtracting will make it negative, then set the sign bit
		if (val == 0)
		{
			F |= flags::S;
			// set it to -1
			val = 0xFF;
		}
		else val -= 1;

		// if the value was less than 0, set the sign bit
		// reset all bits
		F &= C;

		if (val == 0) F |= flags::Z;
		// didn't implement aux. carry
		else if (!(val & 1)) F |= flags::P;

		return 0;
	}

	//******************************
	// CPI instruction
	//******************************
	uint8_t i8080::cpi(const uint8_t& arg) noexcept
	{
		// get the 2's complement of the value
		uint8_t val = read8();
		val = ~val + 1;
		// compare the two
		int16_t result = A + val;
		// clear the flags
		F &= C;

		// lower byte is equal
		if (!(result & 0xFF)) 
			F |= flags::Z;
		// they have different signs (because val is actually -val)
		else if (!(result >> 8))
		{
			F |= flags::C;
		}
		return 0;
	}

	//******************************
	// Conditional JMP instruction
	//******************************
	uint8_t i8080::jc(const uint8_t& arg) noexcept
	{
		uint8_t con = ccc(arg);
		// get the address we want to jump to
		uint16_t addr = read16();
		if ((con == 0 && !(F & flags::Z)) || (con == 1 && (F & flags::Z))
			|| (con == 2 && !(F & flags::C)) || (con == 3 && (F & flags::C))
			|| (con == 4 && !(F & flags::P)) || (con == 5 && (F & flags::P))
			|| (con == 6 && !(F & flags::S)) || (con == 7 && (F & flags::S)))
		{
			PC = addr;
			return 0;
		}
		return 0;
	}

	//******************************
	// RET instruction
	//******************************
	uint8_t i8080::ret(const uint8_t& arg) noexcept
	{
		// get the last two bytes from the stack
		SP += 2;
		PC = memory[SP-1];
		PC |= memory[SP] << 8;
		return 0;
	}

	//**********************************
	// Load the program
	//**********************************
	void i8080::load_program(uint16_t offset) noexcept
	{
		// start loading
		while (!file.eof()) memory[offset++] = file.get();
		// set the stack to where the program finished
		SP = offset;
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
		load_program();

		// read in the file to memory

		// start initializing the array of operations
		// by filling it with operations which crash
		// the system
		operations.fill(&i8080::bad);

		// now start explicitly filling it
		operations[0x00] = &i8080::nop;
		operations[0x01] = &i8080::lxi;
		operations[0x03] = &i8080::inx;
		operations[0x05] = &i8080::dcr;
		operations[0x06] = &i8080::mvi;
		operations[0x0A] = &i8080::ldax;
		operations[0x0E] = &i8080::mvi;

		operations[0x11] = &i8080::lxi;
		operations[0x13] = &i8080::inx;
		operations[0x15] = &i8080::dcr;
		operations[0x16] = &i8080::mvi;
		operations[0x1A] = &i8080::ldax;
		operations[0x1E] = &i8080::mvi;

		operations[0x23] = &i8080::inx;
		operations[0x25] = &i8080::dcr;
		operations[0x21] = &i8080::lxi;
		operations[0x26] = &i8080::mvi;
		operations[0x2E] = &i8080::mvi;

		operations[0x33] = &i8080::inx;
		operations[0x35] = &i8080::dcr;
		operations[0x31] = &i8080::lxi;
		operations[0x36] = &i8080::mvi;
		operations[0x3E] = &i8080::mvi;

		operations[0x40] = &i8080::mov;
		operations[0x41] = &i8080::mov;
		operations[0x42] = &i8080::mov;
		operations[0x43] = &i8080::mov;
		operations[0x44] = &i8080::mov;
		operations[0x45] = &i8080::mov;
		operations[0x46] = &i8080::mov;
		operations[0x47] = &i8080::mov;
		operations[0x48] = &i8080::mov;
		operations[0x49] = &i8080::mov;
		operations[0x4A] = &i8080::mov;
		operations[0x4B] = &i8080::mov;
		operations[0x4C] = &i8080::mov;
		operations[0x4D] = &i8080::mov;
		operations[0x4E] = &i8080::mov;
		operations[0x4F] = &i8080::mov;

		operations[0x50] = &i8080::mov;
		operations[0x51] = &i8080::mov;
		operations[0x52] = &i8080::mov;
		operations[0x53] = &i8080::mov;
		operations[0x54] = &i8080::mov;
		operations[0x55] = &i8080::mov;
		operations[0x56] = &i8080::mov;
		operations[0x57] = &i8080::mov;
		operations[0x58] = &i8080::mov;
		operations[0x59] = &i8080::mov;
		operations[0x5A] = &i8080::mov;
		operations[0x5B] = &i8080::mov;
		operations[0x5C] = &i8080::mov;
		operations[0x5D] = &i8080::mov;
		operations[0x5E] = &i8080::mov;
		operations[0x5F] = &i8080::mov;

		operations[0x60] = &i8080::mov;
		operations[0x61] = &i8080::mov;
		operations[0x62] = &i8080::mov;
		operations[0x63] = &i8080::mov;
		operations[0x64] = &i8080::mov;
		operations[0x65] = &i8080::mov;
		operations[0x66] = &i8080::mov;
		operations[0x67] = &i8080::mov;
		operations[0x68] = &i8080::mov;
		operations[0x69] = &i8080::mov;
		operations[0x6A] = &i8080::mov;
		operations[0x6B] = &i8080::mov;
		operations[0x6C] = &i8080::mov;
		operations[0x6D] = &i8080::mov;
		operations[0x6E] = &i8080::mov;
		operations[0x6F] = &i8080::mov;

		operations[0x70] = &i8080::mov;
		operations[0x71] = &i8080::mov;
		operations[0x72] = &i8080::mov;
		operations[0x73] = &i8080::mov;
		operations[0x74] = &i8080::mov;
		operations[0x75] = &i8080::mov;
		operations[0x77] = &i8080::mov;
		operations[0x78] = &i8080::mov;
		operations[0x79] = &i8080::mov;
		operations[0x7A] = &i8080::mov;
		operations[0x7B] = &i8080::mov;
		operations[0x7C] = &i8080::mov;
		operations[0x7D] = &i8080::mov;
		operations[0x7E] = &i8080::mov;
		operations[0x7F] = &i8080::mov;

		operations[0xC2] = &i8080::jc;
		operations[0xC3] = &i8080::jmp;
		operations[0xC9] = &i8080::ret;
		operations[0xCA] = &i8080::jc;
		operations[0xCD] = &i8080::call;

		operations[0xD2] = &i8080::jc;
		operations[0xDA] = &i8080::jc;

		operations[0xE2] = &i8080::jc;
		operations[0xEA] = &i8080::jc;

		operations[0xF2] = &i8080::jc;
		operations[0xFA] = &i8080::jc;
		operations[0xFE] = &i8080::cpi;
	}
}