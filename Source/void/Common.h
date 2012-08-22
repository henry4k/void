#ifndef COMMON_HPP
#define COMMON_HPP

// -- Common Includes --

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <tools4k/Common.h>
#include <tools4k/Logic.h>


using tools4k::BoundBy;
using tools4k::Min;
using tools4k::Max;
using tools4k::Abs;
using tools4k::Inside;
using tools4k::InsideArray;
using tools4k::Wrap;


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

// ^- Don't use these anymore!
// TODO: Remove that crap!

#endif
