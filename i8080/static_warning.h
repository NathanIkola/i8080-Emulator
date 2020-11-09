//**************************************
// static_warning.h
//
// Defines a static warning for passing
// information to the user at compile
// time
//
// Author: Nathan Ikola
// nathan.ikola@gmail.com
//**************************************
#pragma once

#define _STR(M) #M
#define STR(M) _STR(M)

#define WARN(FILE, LINE, MSG) __pragma(message("static warning triggered at "FILE":"STR(LINE)" - "MSG))

#define static_warning(MSG) WARN(__FILE__, __LINE__, MSG)