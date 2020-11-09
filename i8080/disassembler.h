//**************************************
// disassembler.h
//
// Holds the declaration for the
// disassembler used to read in the
// opcodes from a file
//
// Author: Nathan Ikola
// nathan.ikola@gmail.com
//**************************************
#pragma once

#include <string>
#include <fstream>
using std::string;

namespace i8080
{
	class Disassembler final
	{
	public:
		//******************************
		// Constructor, takes in the
		// path to a file to read from
		//
		// Throws -1 when file could
		// not be opened properly
		//******************************
		Disassembler(string filename);

		//******************************
		// Destructor, clean up the file
		//******************************
		~Disassembler() noexcept;

		//******************************
		// Get a single line of
		// assembly mnemonic from the
		// file
		//
		// Throws -1 when a file read
		// error occurs
		//******************************
		string GetLine();

		//******************************
		// Gets whether the file has
		// any contents left in it
		//******************************
		inline bool HasContent() noexcept { return !m_file.eof(); }
	private:
		std::ifstream m_file;
		uint16_t m_line;
	};
}