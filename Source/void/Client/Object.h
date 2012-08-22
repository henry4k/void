#ifndef __CLIENT_OBJECT_H__
#define __CLIENT_OBJECT_H__

#include <void/Engine.h>
#include <void/Squirrel.h>
#include <void/Object.h>
#include <void/Singleton.h>


class ClientObject : public Object
{
	public:
		ClientObject( ObjectId id, ObjectTypeId typeId, HSQOBJECT scriptObject );
		virtual ~ClientObject();
		
		void deserialize( IPacket* p );
		void checkReset( double curTime );
		
		static SQInteger ReleaseObject( SQUserPointer up, SQInteger size );
		
		/**
		 * Pushes this a user pointer in the script vm.
		 */
		void pushHandle();
		
		/**
		 * Retrieves the user poiter at the specified position from the vm.
		 * Note that this is no strong reference!
		 */
		static ClientObject* GetHandle( int index );
		
	private:
		Tick m_CreationTick; // Tick when the object was created
};


class ClientObjectManager : public ObjectManager, public SingletonClass<ClientObjectManager>
{
	public:
		ClientObjectManager();
		
		virtual ~ClientObjectManager();
		void update();
		void readUpdate( IPacket* p );
		
		
		static SQInteger fn_AddObject( HSQUIRRELVM vm );
		
		static SQInteger fn_SetObjectCreationCallback( HSQUIRRELVM vm );
		static SQInteger fn_SetObjectUpdateCallback( HSQUIRRELVM vm );
		
	private:
		SquirrelFunction m_ObjectCreationCallback;
		SquirrelFunction m_ObjectUpdateCallback;
		
		// Wann zuletzt ein Update vom Server erhalten wurde
		Tick m_LastUpdateTick;
		// Auf dem Client gibt es kein CurrentTick weil hier die Simulation nur zur Lagkompensation dient
		// (also damit die Pausen zwischen den Ticks nicht bemerkt werden)

		// ^- Öhm.. scheinbar .. wird die Variable *hier* garnicht benötigt .. mhm
		
		friend class ClientObject;
};

#endif