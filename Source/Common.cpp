#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "Common.hpp"

// #define LOG_FILE "log.txt"

void LogV( const char* format, va_list args )
{
	vprintf(format, args);
	printf("\n");
	
#if defined(LOG_FILE)
	static FILE* logFile = fopen(LOG_FILE, "w+");
	vfprintf(logFile, format, args);
	fprintf(logFile, "\n");
	fflush(logFile);
#endif
}

void Log( const char* format, ... )
{
	va_list args;
	va_start(args, format);
		LogV(format,args);
	va_end(args);
}

void FatalError_( const char* format, ... )
{
	va_list args;
	va_start(args, format);
		LogV(format,args);
	va_end(args);
	
	abort();
}