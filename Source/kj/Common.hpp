#ifndef COMMON_HPP
#define COMMON_HPP

// -- Common Includes --

#include <stdlib.h>
#include <string.h>


// -- Logging functions --

void Log( const char* format, ... );

#define QUOTEME_(x) #x
#define QUOTEME(x) QUOTEME_(x)

void Error_( const char* format, ... );
#define Error(...) Error_( __FILE__" @ "QUOTEME(__LINE__)" : "__VA_ARGS__ )


// -- Stuff --

const int MaxPathLength = 256;
const int MaxNameLength = 64;

typedef unsigned int Handle;


#endif