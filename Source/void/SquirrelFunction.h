#ifndef __SQUIRREL_FUNCTION_H__
#define __SQUIRREL_FUNCTION_H__

#include <squirrel.h>

class SquirrelFunction
{
	public:
		SquirrelFunction();
		SquirrelFunction( const SquirrelFunction& fn );
		SquirrelFunction& operator=( const SquirrelFunction& fn );

		/**
		 * @param env Environment object that will be bound to the function.
		 */
		SquirrelFunction( HSQUIRRELVM vm, HSQOBJECT env, HSQOBJECT fun );
		~SquirrelFunction();
		
		/**
		 * Must be called before pushing any function arguments
		 * and executing the function call!
		 */
		void prepareCall() const;
		
		/**
		 * After preparing and pushing the parameters,
		 * this function actually executes the call.
		 * @param argCount How many arguments you pushed
		 * @param pushReturnValue Leaves the return value on top of the stack
		 * @param raiseError Script errors will invoke the VMs error handler
		 */
		void executeCall( int argCount, bool pushReturnValue, bool raiseError ) const;
		
	private:
		HSQUIRRELVM m_Vm;
		HSQOBJECT m_Function;
};

#endif
