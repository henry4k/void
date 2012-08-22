#ifndef __SERVER_OBJECT_H__
#define __SERVER_OBJECT_H__

#include <void/Engine.h>
#include <void/Object.h>
#include <void/Singleton.h>


class ServerObject : public Object
{
	public:
		ServerObject( ObjectId id, ObjectTypeId typeId, HSQOBJECT scriptObject );
		virtual ~ServerObject();
		
		void serialize( OPacket* p, Tick startTick );
		
		static SQInteger ReleaseObject( SQUserPointer up, SQInteger size );
		static SQInteger fn_RemoveObject( HSQUIRRELVM vm );
		static SQInteger fn_ObjectSetDirty( HSQUIRRELVM vm );
		
		/**
		 * Pushes this a user pointer in the script vm.
		 */
		void pushHandle();
		
		/**
		 * Retrieves the user poiter at the specified position from the vm.
		 * Note that this is no strong reference!
		 */
		static ServerObject* GetHandle( int index );
		
	private:
		Tick m_CreationTick; // Tick when the object was created
		bool m_ScriptSerialize; // when set, the next regular update calls the script function "OnSerializeObject( instance, opacket, startTick )"
};


class ServerObjectManager : public ObjectManager, public SingletonClass<ServerObjectManager>
{
	public:
		ServerObjectManager();
		virtual ~ServerObjectManager();
		
		void update();
		
		void onClientConnected( int slot );
		
		static SQInteger fn_CreateObject( HSQUIRRELVM vm );
		static SQInteger fn_SetSerializeCallback( HSQUIRRELVM vm );
		
	private:
		bool isRegularUpdate( Tick startTick ) const;
		void writeUpdate( OPacket* packet, Tick startTick );
		ServerObject* createObject( ObjectTypeId typeId, HSQOBJECT scriptObject );
		
		ObjectId m_NextObjectId;
		Tick m_CurrentTick;
		double m_LastTickTime;
		Tick m_LastUpdateTick;
		SquirrelFunction m_SerializeCallback;
		
		friend class ServerObject;
};

#endif