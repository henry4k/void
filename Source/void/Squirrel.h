#ifndef __SQUIRREL_H__
#define __SQUIRREL_H__

#include <squirrel.h>
#include <void/Common.h>


class Squirrel
{
	public:
		Squirrel();
		~Squirrel();
		
		static Squirrel* ByHandle( HSQUIRRELVM vm );
		
		HSQUIRRELVM vm() const;
		void registerFunction( const SQChar* name, SQFUNCTION func, SQInteger argCount = 0, const SQChar* typeMask = NULL );
		void setConst( const SQChar* name, int v );
		
		static int AutoRegister( const SQChar* name, SQFUNCTION func, SQInteger argCount = 0, const SQChar* typeMask = NULL );
		
	private:
		HSQUIRRELVM m_Vm;
};

#define RegisterSqFunction( Name, ... ) int Name##_AutoRegister = Squirrel::AutoRegister(#Name, fn_##Name, __VA_ARGS__)



#endif