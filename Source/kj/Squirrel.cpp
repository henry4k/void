#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <tools4k/Crc32.h>
#include <kj/Common.h>
#include <kj/SqLoader.h>
#include <kj/Squirrel.h>

#ifdef SQUNICODE
#define scfprintf fwprintf
#define scfopen _wfopen
#define scvprintf vfwprintf
#define scfputs _wfputs
#else
#define scfprintf fprintf
#define scfopen fopen
#define scvprintf vfprintf
#define scfputs fputs
#endif


HSQUIRRELVM ScriptVm;


void PrintStackObject(HSQUIRRELVM vm, SQPRINTFUNCTION pf, SQInteger num)
{
	union
	{
		SQBool b;
		SQInteger i;
		SQFloat f;
		const SQChar* s;
	} data;

	switch( sq_gettype(vm, num) )
	{
		case OT_NULL:
		{
			pf(vm,"NULL\n");
		} break;

		case OT_BOOL:
		{
			sq_getbool(vm,num,&data.b);
			pf(vm,"BOOL (%d)\n",int(data.b));
		} break;

		case OT_INTEGER:
		{
			sq_getinteger(vm,num,&data.i);
			pf(vm,"INTEGER (%d)\n",data.i);
		} break;

		case OT_FLOAT:
		{
			sq_getfloat(vm,num,&data.f);
			pf(vm,"FLOAT (%.14g)\n",data.f);
		} break;

		case OT_STRING:
		{
			sq_getstring(vm,num,&data.s);
			pf(vm,"STRING (\"%s\")\n",data.s);
		} break;

		case OT_TABLE:
		{
			pf(vm,"TABLE\n");
		} break;

		case OT_ARRAY:
		{
			pf(vm,"ARRAY\n");
		} break;

		case OT_USERDATA:
		{
			SQUserPointer p, t;
			sq_getuserdata(vm,num,&p,&t);
			pf(vm,"USERDATA (%p , typetag: %p)\n",p,t);
		} break;

		case OT_CLOSURE:
		{
			SQUnsignedInteger p, f;
			sq_getclosureinfo(vm, num, &p, &f);
			// SQFunctionInfo fi;
			// sq_getfunctioninfo(vm, num, &fi);
			pf(vm,"CLOSURE (%d paramerters, %d free variables)\n", p, f);
		} break;

		case OT_NATIVECLOSURE:
		{
			pf(vm,"NATIVE CLOSURE\n");
		} break;

		case OT_GENERATOR:
		{
			pf(vm,"GENERATOR\n");
		} break;

		case OT_USERPOINTER:
		{
			SQUserPointer p;
			sq_getuserpointer(vm,num,&p);
			pf(vm,"USERPOINTER (%p)\n",p);
		} break;

		case OT_CLASS:
		{
			SQUserPointer t;
			sq_gettypetag(vm,num,&t);
			pf(vm,"CLASS (typetag: %p)\n",t);
		} break;

		case OT_INSTANCE:
		{
			sq_getclass(vm,num);
			SQUserPointer t;
			sq_gettypetag(vm,-1,&t);
			sq_pop(vm,1);
			SQUserPointer u;
			sq_getinstanceup(vm,num,&u,0);
			pf(vm,"INSTANCE (%p , class typetag: %p)\n",u,t);
		} break;

		case OT_THREAD:
		{
			HSQUIRRELVM t;
			sq_getthread(vm,num,&t);
			SQInteger s = sq_getvmstate(t);
			pf(vm,"THREAD (%p",t);
			switch(s)
			{
				case SQ_VMSTATE_IDLE: pf(vm," , idle"); break;
				case SQ_VMSTATE_RUNNING: pf(vm," , running"); break;
				case SQ_VMSTATE_SUSPENDED: pf(vm," , suspended"); break;
			}
			pf(vm,")\n");
		} break;

		case OT_WEAKREF:
		{
			pf(vm,"WEAKREF\n");
		} break;

		default:
		{
			pf(vm,"INVALID\n");
		} return;
	}
}

void PrintCallStack(HSQUIRRELVM vm, SQPRINTFUNCTION pf)
{
	SQStackInfos si;
	SQInteger level = 1; // 1 is to skip this function that is level 0
	const SQChar* name = 0;
	SQInteger seq = 0;
	pf(vm, _SC("\nCALLSTACK\n"));
	while(SQ_SUCCEEDED( sq_stackinfos(vm, level, &si) ))
	{
		const SQChar* fn=_SC("unknown");
		const SQChar* src=_SC("unknown");

		if(si.funcname)
			fn = si.funcname;

		if(si.source)
			src = si.source;

		pf(vm, _SC("*FUNCTION [%s()] %s line [%d]\n"), fn, src, si.line);
		level++;
	}
	level=0;
	pf(vm,_SC("\nLOCALS\n"));

	for(level=0;level<10;level++)
	{
		seq=0;
		while((name = sq_getlocal(vm,level,seq)))
		{
			seq++;
			
			pf(vm,"[%s] ",name);
			PrintStackObject(vm, pf,-1);

			sq_pop(vm,1);
		}
	}
}

void PrintStack(HSQUIRRELVM vm, SQPRINTFUNCTION pf, SQInteger begin, SQInteger end)
{
	if(end < begin)
	{
		SQInteger s = end;
		end = begin;
		begin = s;
	}

    for(; begin <= end; begin++)
    {
        pf(vm, "[%d] ", begin);
		PrintStackObject(vm, pf, begin);
    }
}

void PrintArgs(HSQUIRRELVM vm, SQPRINTFUNCTION pf)
{
    SQInteger nargs = sq_gettop(vm);

    for(SQInteger n = 1; n <= nargs; n++)
    {
        pf(vm,"[%d] ", n);
		PrintStackObject(vm, pf, n);
    }
}


/**** Callbacks ****/

//const size_t cPrintBufferSize = 512;
//char PrintBuffer[cPrintBufferSize];

void fn_print(HSQUIRRELVM vm, const SQChar* s,...)
{
	va_list vl;
	va_start(vl, s);
	scvprintf(stdout, s, vl);
	//LogInfo().write(&PrintBuffer[0], scvsprintf(&PrintBuffer[0], s, vl));
	va_end(vl);
}

void fn_error(HSQUIRRELVM vm, const SQChar* s,...)
{
	va_list vl;
	va_start(vl, s);
	scvprintf(stderr, s, vl);
	//LogError().write(&PrintBuffer[0], scvsprintf(&PrintBuffer[0], s, vl));
	va_end(vl);
}

void fn_compiler_error(HSQUIRRELVM vm, const SQChar* error, const SQChar* source, SQInteger line, SQInteger column)
{
	SQPRINTFUNCTION pf = sq_geterrorfunc(vm);
	assert(pf);
	pf(vm, _SC("%s line = (%d) column = (%d) : error %s\n"), source, line, column, error);
}

SQInteger fn_printerror(HSQUIRRELVM vm)
{
	SQPRINTFUNCTION pf = sq_geterrorfunc(vm);
	assert(pf);

	const SQChar *sErr = 0;
	if(sq_gettop(vm)>=1)
	{
		if(SQ_SUCCEEDED(sq_getstring(vm,2,&sErr)))
			pf(vm,_SC("\nAN ERROR HAS OCCURED [%s]\n"),sErr);
		else
			pf(vm,_SC("\nAN ERROR HAS OCCURED [unknown]\n"));

		PrintCallStack(vm, pf);
	}
	return 0;
}

SQInteger fn_crc32(HSQUIRRELVM vm)
{
	const SQChar* text;
	sq_getstring(vm, 2, &text);
	sq_pushinteger(vm, tools4k::Crc32String(text));
	return 1;
}

/*
char* FileName( HSQUIRRELVM vm )
{
	SQStackInfos si;
	if( SQ_SUCCEEDED(sq_stackinfos(vm, 1, &si)) )
		return Clone(si.source,char,strlen(si.source)+1);
	else
		return NULL;
}

SQInteger fn_filename(HSQUIRRELVM vm)
{
	String file = FileName(vm);
	sq_pushstring(vm, file.data(), file.length());
	return 1;
}

char* DirName( HSQUIRRELVM vm )
{
	SQStackInfos si;
	if( SQ_SUCCEEDED(sq_stackinfos(vm, 1, &si)) )
		return ParentDir(si.source);
	else
		return NULL;
}

SQInteger fn_dirname(HSQUIRRELVM vm)
{
	String dir = DirName(vm);
	sq_pushstring(vm, dir.data(), dir.length());
	return 1;
}

SQInteger fn_include(HSQUIRRELVM vm)
{
	const SQChar* file;
	sq_getstring(vm, 2, &file);
	
	char* dir = DirName(vm);
	char* filePath = Concat2Strings(dir,file);
	Free(dir);
	
	char* key = Concat2Strings("includes_",filePath);
	
	// Check if that file was already included
	sq_pushstring(vm, key, -1); // push file path
	if( SQ_SUCCEEDED(sq_get(vm, 1)) ) // env[file]
	{
		sq_pop(vm, 2); // pop result and environment
		sq_pushbool(vm, true);
	}
	else
	{
		sq_push(vm, 1); // push this
		bool r = SQ_SUCCEEDED(DoFile(vm, filePath, false, true));
		sq_pop(vm, 1);
		
		if(r)
		{
			sq_pushstring(vm, key, -1); // push file path
			sq_pushbool(vm, true);
			sq_newslot(vm, 1, false);
		}
		else
		{
			Log("Can't include %s\n", filePath.data());
		}

		sq_pushbool(vm, r);
	}
	Free(filePath);
	Free(key);
	return 1;
}
*/


/// ---- Squirrel ----

Squirrel::Squirrel()
{
	m_Vm = sq_open(1024);
	
	sq_setprintfunc(m_Vm, fn_print, fn_error);
	sq_setcompilererrorhandler(m_Vm, fn_compiler_error);
	
	sq_newclosure(m_Vm, fn_printerror, 0);
	sq_seterrorhandler(m_Vm);
	
	registerFunction("loadfile", fn_loadfile, -2, ".sb");
	registerFunction("dofile", fn_dofile, -2, ".sb");
	registerFunction("crc32", fn_crc32, 2, ".s");
}

Squirrel::~Squirrel()
{
	sq_close(m_Vm);
}

HSQUIRRELVM Squirrel::vm() const
{
	return m_Vm;
}

void Squirrel::registerFunction( const SQChar* name, SQFUNCTION func, SQInteger argCount, const SQChar* typeMask )
{
	sq_pushroottable(m_Vm);
	
	sq_pushstring(m_Vm, name, -1);
	sq_newclosure(m_Vm, func, 0);
	sq_setparamscheck(m_Vm, argCount, typeMask);
	sq_setnativeclosurename(m_Vm, -1, name);
	sq_newslot(m_Vm, -3, SQFalse);
	
	sq_pop(m_Vm, 1); // pop root table
}

void Squirrel::setConst( const SQChar* name, int v )
{
	sq_pushconsttable(m_Vm);
	
	sq_pushstring(m_Vm, name, -1);
	sq_pushinteger(m_Vm, v);
	sq_newslot(m_Vm, -3, false);
	
	sq_pop(m_Vm, 1);
}
