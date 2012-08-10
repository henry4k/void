#ifndef __SQLOADER_H__
#define __SQLOADER_H__

#include <squirrel.h>


SQRESULT LoadFile( HSQUIRRELVM vm, const SQChar* fileName, SQBool printError );

/**
	...
	The function expects a table on top of the stack that will be used as 'this' for the execution of the script.
	The 'this' parameter is left untouched in the stack.
	...
	
	sq_pushobject(vm, rootTable);
	sq_pushobject(vm, thisTable);
	Sq::DoFile(...);
	sq_pop(vm, 2); // pop rootTable and thisTable
**/
SQRESULT DoFile( HSQUIRRELVM vm, const SQChar* fileName, SQBool retVal, SQBool printError, SQInteger closureRoot = 0 );

SQInteger fn_loadfile( HSQUIRRELVM vm );
SQInteger fn_dofile( HSQUIRRELVM vm );
SQInteger WriteFile( HSQUIRRELVM vm, const SQChar* fileName );

#endif