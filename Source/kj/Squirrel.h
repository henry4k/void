#ifndef __SQUIRREL_H__
#define __SQUIRREL_H__

#include <squirrel.h>
#include <kj/Common.h>


class Squirrel
{
	public:
		Squirrel();
		~Squirrel();
		
		HSQUIRRELVM vm() const;
		void registerFunction( const SQChar* name, SQFUNCTION func, SQInteger argCount = 0, const SQChar* typeMask = NULL );
		void setConst( const SQChar* name, int v );
		
	private:
		HSQUIRRELVM m_Vm;
};



#endif