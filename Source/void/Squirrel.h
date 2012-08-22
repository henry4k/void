#ifndef __SQUIRREL_H__
#define __SQUIRREL_H__

#include <squirrel.h>

#include <void/SqLoader.h>
#include <void/SquirrelFunction.h>

#include <void/Common.h>
#include <void/Singleton.h>

class Squirrel : public SingletonClass<Squirrel>
{
	public:
		Squirrel();
		~Squirrel();
		
		/**
		 * Returns the Squirrel instance of an vm handle.
		 */
		static Squirrel* ByHandle( HSQUIRRELVM vm );
		
		/**
		 * Returns the vm handle.
		 */
		HSQUIRRELVM vm() const;

		/**
		 * Register a new engine function.
		 * @see sq_setparamscheck()
		 */
		void registerFunction( const SQChar* name, SQFUNCTION func, SQInteger argCount = 0, const SQChar* typeMask = NULL );

		/**
		 * Set a integer constant.
		 */
		void setConst( const SQChar* name, int v );
		
		/**
		 * Helper function for automatic function registering.
		 * @see RegisterSqFunction()
		 */
		static int AutoRegister( const SQChar* name, SQFUNCTION func, SQInteger argCount = 0, const SQChar* typeMask = NULL );
		
	private:
		HSQUIRRELVM m_Vm;
};

/**
 * Macro for automatic function registration.
 */
#define RegisterSqFunction( Name, ... ) int Name##_AutoRegister = Squirrel::AutoRegister(#Name, __VA_ARGS__)



#endif
