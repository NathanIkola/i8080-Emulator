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
#include "opcodes.h"

#define DEBUG_MESSAGES

#ifdef DEBUG_MESSAGES
#define MSG std::cout << "0x" << std::setfill('0') << std::setw(4) << std::hex << PC << "\n"
#else
#define MSG void()
#endif

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

//**************************************
// Get the parity of a number
//**************************************
inline uint8_t parity(uint8_t in)
{
	bool even_parity = true;
	while (in != 0)
	{
		if (in & 0x01) even_parity = !even_parity;
		in >>= 1;
	}
	return even_parity;
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
			MSG;
			op = read8();
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
	// Write a register pair
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
		memory[SP + 2] = (ret >> 8) & 0xFF;
		memory[SP + 1] = (ret & 0xFF);
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
		// reset all flags but carry
		F &= C;

		// get our value (by reference!)
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
		if (val == 0) F |= flags::Z;
		// didn't implement aux. carry
		else if (parity(val)) F |= flags::P;

		return 0;
	}

	//******************************
	// CPI instruction
	//******************************
	uint8_t i8080::cpi(const uint8_t& arg) noexcept
	{
		// get the 2's complement of the value
		uint8_t val = read8();
		// put it in 2's complement negative
		val = ~val + 1;
		// perform A + -val
		int16_t result = A + val;
		// clear the flags
		F &= 0;

		// lower byte is equal
		if ((result & 0xFF) == 0)
			F |= flags::Z;
		// set the carry bit as required
		else if (!(result & (1 << 8))) F |= flags::C;
		// flip the carry bit if the numbers were of a different sign
		// note that the sign of val is inverted now, so if they are
		// equal then the signs were different initially
		if ((val >> 7 == A >> 7) || ((val == 0) && A >> 7) || ((val >> 7 == 0) && A == 0)) F ^= flags::C;
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
		PC = memory[SP - 1];
		PC |= memory[SP] << 8;
		return 0;
	}

	//**********************************
	// PUSH instruction
	//**********************************
	inline uint8_t i8080::push(const uint8_t& arg) noexcept
	{
		uint16_t val = read_rp(rp(arg));
		assert(SP > 1);
		SP -= 2;
		memory[SP + 2] = (val >> 8) & 0xFF;
		memory[SP + 1] = (val & 0xFF);
		return 0;
	}

	//******************************
	// POP instruction
	//******************************
	uint8_t i8080::pop(const uint8_t& arg) noexcept
	{
		// get the last two bytes from the stack
		SP += 2;
		uint16_t val = memory[SP - 1];
		val |= memory[SP] << 8;
		write_rp(rp(arg), val);
		return 0;
	}

	//**********************************
	// DAD instruction
	//**********************************
	uint8_t i8080::dad(const uint8_t& arg) noexcept
	{
		uint16_t hl = read_rp(2);
		uint16_t hl_pre = hl;
		hl += read_rp(rp(arg));
		write_rp(2, hl);
		if (hl_pre > hl) F |= flags::C;
		else F &= ~flags::C;
		return 0;
	}

	//**********************************
	// EXCHG instruction
	//**********************************
	uint8_t i8080::exchg(const uint8_t& arg) noexcept
	{
		uint16_t de = read_rp(1);
		uint16_t hl = read_rp(2);
		// swap HL and DE
		write_rp(2, de);
		write_rp(1, hl);
		return 0;
	}

	//**********************************
	// OUT instruction
	//**********************************
	uint8_t i8080::out(const uint8_t& arg) noexcept
	{
		// not a lot we can do here yet
		// so ignore the command
		// but we do need to read the port address in
		uint8_t addr = read8();
		return 0;
	}

	//**********************************
	// RRC instruction
	//**********************************
	uint8_t i8080::rrc(const uint8_t& arg) noexcept
	{
		// rotate right, so capture the lowest order bit
		// (evaluates to 0 or 1)
		uint8_t _ar = A & 0x01;
		// rotate the bit over
		A = (A >> 1) | (_ar << 7);
		// set the carry if _ar == 1
		if (_ar) F |= flags::C;
		// otherwise clear it
		else F &= ~flags::C;
		return 0;
	}

	//**********************************
	// ANI instruction
	//**********************************
	uint8_t i8080::ani(const uint8_t& arg) noexcept
	{
		// read in the value we are &'ing with A
		uint8_t val = read8();
		A &= val;
		// clear all flags
		F = 0;
		// set our flags as necessary
		if (A == 0) F |= flags::Z;
		else if (A >> 7) F |= flags::S;
		if (parity(A)) F |= flags::P;

		return 0;
	}

	//**********************************
	// ADI instruction
	//**********************************
	uint8_t i8080::adi(const uint8_t& arg) noexcept
	{
		// read in the value we are adding to A
		uint8_t val = read8();
		// clear all flags
		F = 0;
		// if we overflow, this will trip the carry flag
		if (static_cast<uint8_t>(A + val) < A) F |= flags::C;
		A += val;

		if (A == 0) F |= flags::Z;
		else if (A & 0x80) F |= flags::S;
		if (parity(A)) F |= flags::P;

		return 0;
	}

	//******************************
	// LDA instruction
	//******************************
	uint8_t i8080::lda(const uint8_t& arg) noexcept
	{
		// load into A from the address
		// next in the program
		uint16_t addr = read16();
		// read in this address
		// exception risk, can read out of bounds
		// if memory size < addr
		A = memory[addr];
		return 0;
	}

	//******************************
	// ANA instruction
	//******************************
	uint8_t i8080::ana(const uint8_t& arg) noexcept
	{
		// & A with the register specified
		A &= get_reg(source(arg));
		// clear all flags
		F = 0;
		// set our flags as necessary
		if (A == 0) F |= flags::Z;
		else if (A & 0x80) F |= flags::S;
		if (parity(A)) F |= flags::P;
		return 0;
	}

	//**********************************
	// Conditional RET instruction
	//**********************************
	uint8_t i8080::rc(const uint8_t& arg) noexcept
	{
		uint8_t con = ccc(arg);
		if ((con == 0 && !(F & flags::Z)) || (con == 1 && (F & flags::Z))
			|| (con == 2 && !(F & flags::C)) || (con == 3 && (F & flags::C))
			|| (con == 4 && !(F & flags::P)) || (con == 5 && (F & flags::P))
			|| (con == 6 && !(F & flags::S)) || (con == 7 && (F & flags::S)))
		{
			SP += 2;
			PC = memory[SP - 1];
			PC |= memory[SP] << 8;
			return 0;
		}
		return 1;
	}

	//******************************
	// ACI instruction
	//******************************
	uint8_t i8080::aci(const uint8_t& arg) noexcept
	{
		// read in the value we are adding to A
		uint8_t val = read8();
		// capture our carry flag so we can add it in later
		uint8_t c = (F & flags::C) == 1;
		// clear all flags
		F = 0;
		// if we overflow, this will trip the carry flag
		if (static_cast<uint8_t>(A + val + c) < A) F |= flags::C;
		A += val + c;

		if (A == 0) F |= flags::Z;
		else if (A & 0x80) F |= flags::S;
		if (parity(A)) F |= flags::P;

		return 0;
	}

	//******************************
	// SUI instruction
	//******************************
	uint8_t i8080::sui(const uint8_t& arg) noexcept
	{
		// load the immediate value
		uint8_t val = read8();
		// put number into 2's complement
		val = ~val + 1;

		// reset our flags
		F = 0;

		// see if we are going to underflow
		if (static_cast<uint8_t>(A + val) > A) F |= flags::C;
		A += val;
		if (A == 0) F |= flags::Z;
		else if (A >> 7) F |= flags::S;
		if (parity(A)) F |= flags::P;

		return 0;
	}

	//******************************
	// SBI instruction
	//******************************
	uint8_t i8080::sbi(const uint8_t& arg) noexcept
	{
		// load the immediate value
		uint8_t val = read8();
		// add the carry bit
		if (F & flags::C) ++val;
		// calculate the 2's complement
		val = ~val + 1;
		// clear the flags
		F = 0;

		// see if we will underflow
		if (static_cast<uint8_t>(A + val) > A) F |= flags::C;
		A += val;
		if (A == 0) F |= flags::Z;
		else if (A >> 7) F |= flags::S;
		if (parity(A)) F |= flags::P;

		return 0;
	}

	//******************************
	// ORI instruction
	//******************************
	uint8_t i8080::ori(const uint8_t& arg) noexcept
	{
		// load the immediate value
		uint8_t val = read8();
		// use bitwise OR on A
		A |= val;
		// clear the flags
		F = 0;
		if (A == 0) F |= flags::Z;
		else if (A >> 7) F |= flags::S;
		if (parity(A)) F |= flags::P;

		return 0;
	}

	//******************************
	// XRI instruction
	//******************************
	uint8_t i8080::xri(const uint8_t& arg) noexcept
	{
		// get the immediate value
		uint8_t val = read8();
		// use bitwise XOR
		A ^= val;
		// clear the flags
		F = 0;

		if (A == 0) F |= flags::Z;
		else if (A >> 7) F |= flags::S;
		if (parity(A)) F |= flags::P;

		return 0;
	}

	//******************************
	// Conditional CALL instruction
	//******************************
	uint8_t i8080::cc(const uint8_t& arg) noexcept
	{
		uint8_t con = ccc(arg);
		// get the address we want to jump to
		uint16_t target = read16();
		if ((con == 0 && !(F & flags::Z)) || (con == 1 && (F & flags::Z))
			|| (con == 2 && !(F & flags::C)) || (con == 3 && (F & flags::C))
			|| (con == 4 && !(F & flags::P)) || (con == 5 && (F & flags::P))
			|| (con == 6 && !(F & flags::S)) || (con == 7 && (F & flags::S)))
		{
			// save our return address
			uint16_t ret = PC;
			PC = target;
			// store our return address on the stack
			assert(SP > 1);
			SP -= 2;
			memory[SP + 2] = (ret >> 8) & 0xFF;
			memory[SP + 1] = (ret & 0xFF);
			return 0;
		}

		return 1;
	}

	//**********************************
	// INR (increment register)
	//**********************************
	uint8_t i8080::inr(const uint8_t& arg) noexcept
	{
		uint8_t& reg = get_reg(dest(arg));
		++reg;

		return 0;
	}

	//******************************
	// XRA (XOR A with register)
	//******************************
	uint8_t i8080::xra(const uint8_t& arg) noexcept
	{
		uint8_t src = get_reg(source(arg));
		A = A ^ src;

		// check for flags
		F = 0;

		if (A == 0) F |= flags::Z;
		else if ((A >> 7) == 1) F |= flags::S;
		if (parity(A)) F |= flags::P;

		return 0;
	}

	//******************************
	// ADD register or memory to A
	//******************************
	uint8_t i8080::add(const uint8_t& arg) noexcept
	{
		uint8_t src = get_reg(source(arg));
		uint16_t a = A;
		a += src;
		A = a;

		// check for flags
		F = 0;

		if (A == 0) F |= flags::Z;
		else if ((A >> 7) == 1) F |= flags::S;
		if (parity(A)) F |= flags::P;
		if (a & ~0xFF) F |= flags::C;

		return 0;
	}

	//******************************
	// SUB register or memory to A
	//******************************
	uint8_t i8080::sub(const uint8_t& arg) noexcept
	{
		uint8_t src = get_reg(source(arg));
		uint16_t a = A;
		src = ~src + 1;
		a += src;
		A = a;

		// check for flags
		F = 0;

		if (A == 0) F |= flags::Z;
		else if ((A >> 7) == 1) F |= flags::S;
		if (parity(A)) F |= flags::P;
		if (a & ~0xFF) F |= flags::C;

		return 0;
	}

	//******************************
	// Add register to A with carry
	//******************************
	uint8_t i8080::adc(const uint8_t& arg) noexcept
	{
		uint8_t src = get_reg(source(arg));
		uint16_t a = A;
		if (F & flags::C) ++a;
		a += src;
		A = a;

		// check for flags
		F = 0;

		if (A == 0) F |= flags::Z;
		else if ((A >> 7) == 1) F |= flags::S;
		if (parity(A)) F |= flags::P;
		if (a & ~0xFF) F |= flags::C;

		return 0;
	}

	//******************************
	// Sub register from A with borrow
	//******************************
	uint8_t i8080::sbb(const uint8_t& arg) noexcept
	{
		uint8_t src = get_reg(source(arg));
		uint8_t a = A;
		if (F & flags::C) ++src;
		src = ~src + 1;
		a += src;
		A = a;

		// check for flags
		F = 0;

		if (A == 0) F |= flags::Z;
		else if ((A >> 7) == 1) F |= flags::S;
		if (parity(A)) F |= flags::P;
		if (a & ~0xFF) F |= flags::C;

		return 0;
	}

	//******************************
	// OR A with register
	//******************************
	uint8_t i8080::ora(const uint8_t& arg) noexcept
	{
		uint8_t src = get_reg(source(arg));
		A |= src;

		// check for flags
		F = 0;

		if (A == 0) F |= flags::Z;
		else if ((A >> 7) == 1) F |= flags::S;
		if (parity(A)) F |= flags::P;

		return 0;
	}

	//******************************
	// Compare with accumulator
	//******************************
	uint8_t i8080::cmp(const uint8_t& arg) noexcept
	{
		uint8_t val = get_reg(source(arg));
		// put it in 2's complement negative
		val = ~val + 1;
		// perform A + -val
		int16_t result = A + val;
		// clear the flags
		F &= 0;

		// lower byte is equal
		if ((result & 0xFF) == 0)
			F |= flags::Z;
		// set the carry bit as required
		else if (!(result & (1 << 8))) F |= flags::C;
		// flip the carry bit if the numbers were of a different sign
		// note that the sign of val is inverted now, so if they are
		// equal then the signs were different initially
		if ((val >> 7 == A >> 7) || ((val == 0) && A >> 7) || ((val >> 7 == 0) && A == 0)) F ^= flags::C;
		return 0;
	}

	//******************************
	// Decrement register pair
	//******************************
	uint8_t i8080::dcx(const uint8_t& arg) noexcept
	{
		uint8_t reg = rp(arg);
		uint16_t rp = read_rp(reg);
		rp += 0xFFFF;
		write_rp(reg, rp);

		return 0;
	}

	//******************************
	// Store A in memory location
	//******************************
	uint8_t i8080::sta(const uint8_t& arg) noexcept
	{
		uint16_t address = read16();
		memory[address] = A;

		return 0;
	}

	//**********************************
	// Load H and L direct
	//**********************************
	uint8_t i8080::lhld(const uint8_t& arg) noexcept
	{
		uint16_t address = read16();
		L = memory[address];
		H = memory[address + 1];
		return 0;
	}

	//**********************************
	// Store H and L direct
	//**********************************
	uint8_t i8080::shld(const uint8_t& arg) noexcept
	{
		uint16_t address = read16();
		memory[address] = L;
		memory[address + 1] = H;
		return 0;
	}

	//******************************
	// Store accumulator in memory
	//******************************
	uint8_t i8080::stax(const uint8_t& arg) noexcept
	{
		uint8_t reg = rp(arg);
		uint16_t address = read_rp(reg);
		memory[address] = A;
		return 0;
	}

	//**********************************
	// Set the carry bit
	//**********************************
	uint8_t i8080::stc(const uint8_t& arg) noexcept
	{
		F |= flags::C;
		return 0;
	}

	//******************************
	// Complement the carry bit
	//******************************
	uint8_t i8080::cmc(const uint8_t& arg) noexcept
	{
		F ^= flags::C;
		return 0;
	}

	//******************************
	// Complement A
	//******************************
	uint8_t i8080::cma(const uint8_t& arg) noexcept
	{
		A = ~A;
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
	i8080::i8080(const char* filename, uint16_t size, uint16_t offset) 
		: memory(nullptr), file(filename, std::ios_base::binary), operations()
	{
		if (!file.is_open()) throw -1;

		using namespace i8080;

		// first allocate the amount of memory that we want
		memory = new uint8_t[static_cast<int>(size) + 1];

		// read in the file to memory
		load_program(offset);

		// set our offset
		PC = offset;

		// start initializing the array of operations
		// by filling it with operations which crash
		// the system
		operations.fill(&i8080::bad);

		// now start explicitly filling it
		operations[0x00] = &i8080::nop;
		operations[0x01] = &i8080::lxi;
		operations[0x02] = &i8080::stax;
		operations[0x03] = &i8080::inx;
		operations[0x04] = &i8080::inr;
		operations[0x05] = &i8080::dcr;
		operations[0x06] = &i8080::mvi;
		operations[0x09] = &i8080::dad;
		operations[0x0A] = &i8080::ldax;
		operations[0x0B] = &i8080::dcx;
		operations[0x0C] = &i8080::inr;
		operations[0x0D] = &i8080::dcr;
		operations[0x0E] = &i8080::mvi;
		operations[0x0F] = &i8080::rrc;

		operations[0x11] = &i8080::lxi;
		operations[0x12] = &i8080::stax;
		operations[0x13] = &i8080::inx;
		operations[0x14] = &i8080::inr;
		operations[0x15] = &i8080::dcr;
		operations[0x16] = &i8080::mvi;
		operations[0x19] = &i8080::dad;
		operations[0x1A] = &i8080::ldax;
		operations[0x1B] = &i8080::dcx;
		operations[0x1C] = &i8080::inr;
		operations[0x1D] = &i8080::dcr;
		operations[0x1E] = &i8080::mvi;

		operations[0x22] = &i8080::shld;
		operations[0x23] = &i8080::inx;
		operations[0x24] = &i8080::inr;
		operations[0x25] = &i8080::dcr;
		operations[0x21] = &i8080::lxi;
		operations[0x26] = &i8080::mvi;
		operations[0x29] = &i8080::dad;
		operations[0x2A] = &i8080::lhld;
		operations[0x2B] = &i8080::dcx;
		operations[0x2C] = &i8080::inr;
		operations[0x2D] = &i8080::dcr;
		operations[0x2E] = &i8080::mvi;
		operations[0x2F] = &i8080::cma;

		operations[0x32] = &i8080::sta;
		operations[0x33] = &i8080::inx;
		operations[0x34] = &i8080::inr;
		operations[0x35] = &i8080::dcr;
		operations[0x31] = &i8080::lxi;
		operations[0x36] = &i8080::mvi;
		operations[0x37] = &i8080::stc;
		operations[0x39] = &i8080::dad;
		operations[0x3A] = &i8080::lda;
		operations[0x3B] = &i8080::dcx;
		operations[0x3C] = &i8080::inr;
		operations[0x3D] = &i8080::dcr;
		operations[0x3E] = &i8080::mvi;
		operations[0x3F] = &i8080::cmc;

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
		operations[0x76] = &i8080::hlt;
		operations[0x77] = &i8080::mov;
		operations[0x78] = &i8080::mov;
		operations[0x79] = &i8080::mov;
		operations[0x7A] = &i8080::mov;
		operations[0x7B] = &i8080::mov;
		operations[0x7C] = &i8080::mov;
		operations[0x7D] = &i8080::mov;
		operations[0x7E] = &i8080::mov;
		operations[0x7F] = &i8080::mov;

		operations[0x80] = &i8080::add;
		operations[0x81] = &i8080::add;
		operations[0x82] = &i8080::add;
		operations[0x83] = &i8080::add;
		operations[0x84] = &i8080::add;
		operations[0x85] = &i8080::add;
		operations[0x86] = &i8080::add;
		operations[0x87] = &i8080::add;
		operations[0x88] = &i8080::adc;
		operations[0x89] = &i8080::adc;
		operations[0x8A] = &i8080::adc;
		operations[0x8B] = &i8080::adc;
		operations[0x8C] = &i8080::adc;
		operations[0x8D] = &i8080::adc;
		operations[0x8E] = &i8080::adc;
		operations[0x8F] = &i8080::adc;

		operations[0x90] = &i8080::sub;
		operations[0x91] = &i8080::sub;
		operations[0x92] = &i8080::sub;
		operations[0x93] = &i8080::sub;
		operations[0x94] = &i8080::sub;
		operations[0x95] = &i8080::sub;
		operations[0x96] = &i8080::sub;
		operations[0x97] = &i8080::sub;
		operations[0x98] = &i8080::sbb;
		operations[0x99] = &i8080::sbb;
		operations[0x9A] = &i8080::sbb;
		operations[0x9B] = &i8080::sbb;
		operations[0x9C] = &i8080::sbb;
		operations[0x9D] = &i8080::sbb;
		operations[0x9E] = &i8080::sbb;
		operations[0x9F] = &i8080::sbb;

		operations[0xA0] = &i8080::ana;
		operations[0xA1] = &i8080::ana;
		operations[0xA2] = &i8080::ana;
		operations[0xA3] = &i8080::ana;
		operations[0xA4] = &i8080::ana;
		operations[0xA5] = &i8080::ana;
		operations[0xA6] = &i8080::ana;
		operations[0xA7] = &i8080::ana;
		operations[0xA8] = &i8080::xra;
		operations[0xA9] = &i8080::xra;
		operations[0xAA] = &i8080::xra;
		operations[0xAB] = &i8080::xra;
		operations[0xAC] = &i8080::xra;
		operations[0xAD] = &i8080::xra;
		operations[0xAE] = &i8080::xra;
		operations[0xAF] = &i8080::xra;

		operations[0xB0] = &i8080::ora;
		operations[0xB1] = &i8080::ora;
		operations[0xB2] = &i8080::ora;
		operations[0xB3] = &i8080::ora;
		operations[0xB4] = &i8080::ora;
		operations[0xB5] = &i8080::ora;
		operations[0xB6] = &i8080::ora;
		operations[0xB7] = &i8080::ora;
		operations[0xB8] = &i8080::cmp;
		operations[0xB9] = &i8080::cmp;
		operations[0xBA] = &i8080::cmp;
		operations[0xBB] = &i8080::cmp;
		operations[0xBC] = &i8080::cmp;
		operations[0xBD] = &i8080::cmp;
		operations[0xBE] = &i8080::cmp;
		operations[0xBF] = &i8080::cmp;

		operations[0xC0] = &i8080::rc;
		operations[0xC1] = &i8080::pop;
		operations[0xC2] = &i8080::jc;
		operations[0xC3] = &i8080::jmp;
		operations[0xC4] = &i8080::cc;
		operations[0xC5] = &i8080::push;
		operations[0xC6] = &i8080::adi;
		operations[0xC8] = &i8080::rc;
		operations[0xC9] = &i8080::ret;
		operations[0xCA] = &i8080::jc;
		operations[0xCC] = &i8080::cc;
		operations[0xCD] = &i8080::call;
		operations[0xCE] = &i8080::aci;

		operations[0xD0] = &i8080::rc;
		operations[0xD1] = &i8080::pop;
		operations[0xD2] = &i8080::jc;
		operations[0xD3] = &i8080::out;
		operations[0xD4] = &i8080::cc;
		operations[0xD5] = &i8080::push;
		operations[0xD6] = &i8080::sui;
		operations[0xD8] = &i8080::rc;
		operations[0xDA] = &i8080::jc;
		operations[0xDC] = &i8080::cc;
		operations[0xDE] = &i8080::sbi;

		operations[0xE0] = &i8080::rc;
		operations[0xE1] = &i8080::pop;
		operations[0xE2] = &i8080::jc;
		operations[0xE4] = &i8080::cc;
		operations[0xE5] = &i8080::push;
		operations[0xE6] = &i8080::ani;
		operations[0xE8] = &i8080::rc;
		operations[0xEA] = &i8080::jc;
		operations[0xEB] = &i8080::exchg;
		operations[0xEC] = &i8080::cc;
		operations[0xEE] = &i8080::xri;

		operations[0xF0] = &i8080::rc;
		operations[0xF1] = &i8080::pop;
		operations[0xF2] = &i8080::jc;
		operations[0xF4] = &i8080::cc;
		operations[0xF5] = &i8080::push;
		operations[0xF6] = &i8080::ori;
		operations[0xF8] = &i8080::rc;
		operations[0xFA] = &i8080::jc;
		operations[0xFC] = &i8080::cc;
		operations[0xFE] = &i8080::cpi;
	}
}