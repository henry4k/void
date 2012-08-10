#include <assert.h>
#include <stdio.h>
#include "SqLoader.h"


#ifndef SQUNICODE
#define sqstd_fopen fopen
#else
#define sqstd_fopen _wfopen
#endif
#define sqstd_fread fread
#define sqstd_fwrite fwrite
#define sqstd_fseek fseek
#define sqstd_ftell ftell
#define sqstd_fflush fflush
#define sqstd_fclose fclose
#define sqstd_feof feof

static SQInteger _io_file_lexfeed_ASCII( SQUserPointer file )
{
	SQInteger ret;
	char c;
	if( (ret = sqstd_fread(&c, sizeof(c), 1, (FILE*)file)) > 0 )
		return c;
	return 0;
}

static SQInteger _io_file_lexfeed_UTF8( SQUserPointer file )
{
#define READ() \
	if(sqstd_fread(&inchar,sizeof(inchar),1,(FILE*)file) != 1) \
		return 0;

	static const SQInteger utf8_lengths[16] =
	{
		1,1,1,1,1,1,1,1,        /* 0000 to 0111 : 1 byte (plain ASCII) */
		0,0,0,0,                /* 1000 to 1011 : not valid */
		2,2,                    /* 1100, 1101 : 2 bytes */
		3,                      /* 1110 : 3 bytes */
		4                       /* 1111 :4 bytes */
	};

	static unsigned char byte_masks[5] = { 0, 0, 0x1f, 0x0f, 0x07 };
	unsigned char inchar;
	SQInteger c = 0;
	READ();
	c = inchar;

	if(c >= 0x80)
	{
		SQInteger tmp;
		SQInteger codelen = utf8_lengths[ c >> 4 ];
		if(codelen == 0) 
			return 0; // "invalid UTF-8 stream";

		tmp = c & byte_masks[codelen];
		for(SQInteger n = 0; n < codelen-1; ++n)
		{
			tmp <<= 6;
			READ();
			tmp |= inchar & 0x3F;
		}
		c = tmp;
	}
	return c;
}

static SQInteger _io_file_lexfeed_UCS2_LE( SQUserPointer file )
{
	SQInteger ret;
	wchar_t c;
	if( (ret = sqstd_fread(&c, sizeof(c), 1, (FILE*)file)) > 0 )
		return (SQChar)c;
	return 0;
}

static SQInteger _io_file_lexfeed_UCS2_BE( SQUserPointer file )
{
	SQInteger ret;
	unsigned short c;
	if( (ret = sqstd_fread(&c, sizeof(c), 1, (FILE*)file)) > 0 )
	{
		c = ( (c >> 8) & 0x00FF) | ((c << 8) & 0xFF00);
		return (SQChar)c;
	}
	return 0;
}

SQInteger file_read( SQUserPointer file, SQUserPointer buf, SQInteger size )
{
	SQInteger ret;
	if( (ret = sqstd_fread(buf, 1, size, (FILE*)file)) != 0 )
		return ret;
	return -1;
}

SQInteger file_write( SQUserPointer file, SQUserPointer p, SQInteger size )
{
	return sqstd_fwrite(p, 1, size, (FILE*)file);
}

SQRESULT LoadFile( HSQUIRRELVM vm, const SQChar* fileName, SQBool printError )
{
	FILE* file = sqstd_fopen(fileName, _SC("rb"));
	SQInteger ret;
	unsigned short us;
	unsigned char uc;
	SQLEXREADFUNC func = _io_file_lexfeed_ASCII;
	
	if(!file)
		return sq_throwerror(vm, _SC("cannot open the file"));
	
	
	ret = sqstd_fread(&us, 1, 2, file);
	if(ret != 2) // probably an empty file
		us = 0;
	
	if(us == SQ_BYTECODE_STREAM_TAG) // BYTECODE
	{
		sqstd_fseek(file, 0, SEEK_SET);
		if(SQ_SUCCEEDED(sq_readclosure(vm, file_read, file)))
		{
			sqstd_fclose(file);
			return SQ_OK;
		}
	}
	else // SCRIPT
	{
		switch(us)
		{
			// TODO: gotta swap the next 2 lines on BIG endian machines
			
			case 0xFFFE: func = _io_file_lexfeed_UCS2_BE; break; // UTF-16 little endian;
			
			case 0xFEFF: func = _io_file_lexfeed_UCS2_LE; break; // UTF-16 big endian;
			
			case 0xBBEF: // UTF-8
			{
				if(sqstd_fread(&uc, 1, sizeof(uc), file) == 0)
				{ 
					sqstd_fclose(file); 
					return sq_throwerror(vm, _SC("io error")); 
				}
				
				if(uc != 0xBF)
				{ 
					sqstd_fclose(file); 
					return sq_throwerror(vm, _SC("Unrecognozed encoding")); 
				}
				
				func = _io_file_lexfeed_UTF8;
			} break;
			
			default: sqstd_fseek(file, 0, SEEK_SET); break; // ascii
		}
		
		if(SQ_SUCCEEDED(sq_compile(vm, func, file, fileName, printError)))
		{
			sqstd_fclose(file);
			return SQ_OK;
		}
	}
	
	sqstd_fclose(file);
	return SQ_ERROR;
}

SQRESULT DoFile( HSQUIRRELVM vm, const SQChar* fileName, SQBool retVal, SQBool printError, SQInteger closureRoot )
{
	if(SQ_SUCCEEDED( LoadFile(vm, fileName, printError) ))
	{
		if(closureRoot != 0)
		{
			if(closureRoot < 0) // relative position?
				closureRoot -= 1;
			sq_push(vm, closureRoot);
			sq_setclosureroot(vm, -1);
		}
		
		sq_push(vm, -2);
		if(SQ_SUCCEEDED( sq_call(vm, 1, retVal, printError) ))
		{
			sq_remove( vm, (retVal) ? (-2) : (-1) ); //removes the closure
			return 1;
		}
		printf("call failed\n");
		sq_pop(vm, 1); //removes the closure
	}
	return SQ_ERROR;
}

SQInteger fn_loadfile( HSQUIRRELVM vm )
{
	const SQChar* filename;
	SQBool printerror = SQFalse;

	sq_getstring(vm, 2, &filename);

	if(sq_gettop(vm) >= 3)
		sq_getbool(vm, 3, &printerror);

	if(SQ_SUCCEEDED(LoadFile(vm, filename, printerror)))
		return 1;

	return SQ_ERROR; //propagates the error
}

SQInteger fn_dofile( HSQUIRRELVM vm )
{
	const SQChar* filename;
	SQBool printerror = SQFalse;

	sq_getstring(vm, 2, &filename);

	if(sq_gettop(vm) >= 3)
		sq_getbool(vm, 3, &printerror);

	sq_push(vm, 1); //repush the this

	if(SQ_SUCCEEDED(DoFile(vm, filename, SQTrue, printerror)))
		return 1;

	return SQ_ERROR; //propagates the error
}

SQInteger WriteFile( HSQUIRRELVM vm, const SQChar* fileName )
{
	FILE* file = sqstd_fopen(fileName, _SC("wb+"));
	if(!file)
		return sq_throwerror(vm,_SC("cannot open the file"));

	if(SQ_SUCCEEDED(sq_writeclosure(vm, file_write, file)))
	{
		sqstd_fclose(file);
		return SQ_OK;
	}

	sqstd_fclose(file);
	return SQ_ERROR;
}