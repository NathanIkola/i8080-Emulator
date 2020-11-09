//**************************************
// main.cpp
//
// Driver program for testing my i8080
// features
//
// Author: Nathan Ikola
// nathan.ikola@gmail.com
//**************************************

#include <iostream>

#include "static_warning.h"
#include "disassembler.h"

int main()
{
	i8080::Disassembler disassembler("cpudiag.bin");

	while (disassembler.HasContent())
		std::cout << disassembler.GetLine() << std::endl;

	return 0;
}