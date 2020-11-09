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
#include "i8080.h"

int main()
{
	i8080::i8080 cpu("invaders.h");

	cpu.run();

	return 0;
}