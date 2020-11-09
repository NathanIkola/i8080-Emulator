//**************************************
// disassembler.cpp
//
// Holds the definition for the
// disassembler used to read in the
// opcodes from a file
//
// Author: Nathan Ikola
// nathan.ikola@gmail.com
//**************************************
#include "disassembler.h"

#include <iomanip>
#include <sstream>
#include "mnemonics.h"
#include "opcodes.h"

namespace i8080
{
	Disassembler::Disassembler(string filename)
		: m_file(filename, std::ios_base::binary), m_line(0)
	{
		if (!m_file.is_open()) throw -1;
	}

	Disassembler::~Disassembler() noexcept { m_file.close(); m_line = 0; }

	string Disassembler::GetLine()
	{
		string ret = "";
		// read in the instruction
		unsigned char _op = m_file.get();
		opcode op = opcodes[_op];

		// add our base mnemonic into our return string
		std::stringstream lineno;
		lineno << "0x" << std::setfill('0') << std::setw(4) << std::hex << m_line << "  ";
		lineno << std::setfill(' ') << std::setw(8) << std::left << mnemonics[_op];
		ret = lineno.str();
		m_line += op.len;

		// see what size the arguments are, if any
		switch (op.len)
		{
		// just the mnemonic
		case 1:
			return ret;
		case 2:
		{
			// 8 bit address
			if (m_file.eof()) throw -1;
			std::stringstream str;
			str << "0x" << std::setfill('0') << std::setw(2) << std::hex << m_file.get();
			return ret + str.str();
		}
		case 3:
		{
			if (m_file.eof()) throw -1;
			uint16_t addr = m_file.get();
			if (m_file.eof()) throw - 1;
			addr |= m_file.get() << 8;
			std::stringstream str;
			str << "0x" << std::setfill('0') << std::setw(4) << std::hex << addr;
			return ret + str.str();
		}
		default:
			throw -1;
		}

		if (op.len > 1)
		{
			ret += " ";
			// loop through and add all of the remaining arguments
			for (int byte = 1; byte < op.len; ++byte)
			{
				if (m_file.eof()) throw -1;
				std::stringstream str;
				if (byte != 1) str << ",";
				str << std::hex << m_file.get();
				ret += str.str();
			}
		}

		return ret;
	}
}