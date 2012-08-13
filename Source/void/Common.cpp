#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <void/Common.h>



// -- Logging functions --

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

void Error_( const char* format, ... )
{
	va_list args;
	va_start(args, format);
		LogV(format,args);
	va_end(args);
	
	abort();
}




// -- Memory --
/*
void* Alloc( int elementSize, int count )
{
	return malloc(elementSize*count);
}

void* _Realloc( void* data, int oldSize, int newSize )
{
	return realloc();
}

void _Free( void* data )
{
	free(data);
}
*/