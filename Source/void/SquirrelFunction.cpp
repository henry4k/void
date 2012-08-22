#include <void/Common.h>
#include <void/Squirrel.h>
#include <void/SquirrelFunction.h>


SquirrelFunction::SquirrelFunction() :
	m_Vm(NULL)
{
	sq_resetobject(&m_Function);
}

SquirrelFunction::SquirrelFunction( const SquirrelFunction& fn ) :
	m_Vm(fn.m_Vm),
	m_Function(fn.m_Function)
{
	sq_addref(m_Vm, &m_Function);
}

SquirrelFunction& SquirrelFunction::operator=( const SquirrelFunction& fn )
{
	m_Vm = fn.m_Vm;
	m_Function = fn.m_Function;
	sq_addref(m_Vm, &m_Function);
	return *this;
}

SquirrelFunction::SquirrelFunction( HSQUIRRELVM vm, HSQOBJECT env, HSQOBJECT fun ) :
	m_Vm(vm)
{
	sq_pushobject(vm, fun);
	sq_pushobject(vm, env);
	
	sq_bindenv(vm, -2);
	sq_getstackobj(vm, -1, &m_Function);
	sq_addref(m_Vm, &m_Function);
	
	sq_pop(vm, 3); // pop fun, env and new fun
}

SquirrelFunction::~SquirrelFunction()
{
	sq_release(m_Vm, &m_Function);
}

void SquirrelFunction::prepareCall() const
{
	sq_pushobject(m_Vm, m_Function);
	sq_pushnull(m_Vm); // Cause we use an bound environment
}

void SquirrelFunction::executeCall( int argCount, bool pushReturnValue, bool raiseError ) const
{
	sq_call(m_Vm, argCount+1, pushReturnValue, raiseError);
	// argCount+1 because of the fake environment
	
	int offset = pushReturnValue ? -1 : 0;
	sq_remove(m_Vm, -1+offset); // pop function
	// and leave the return value in the stack!
}