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
#include "opcodes.h"

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
			++PC;
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

	//**********************************
	// Constructor
	//**********************************
	i8080::i8080(const char* filename, uint16_t size) : memory(nullptr), file(filename), operations()
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
	}
}