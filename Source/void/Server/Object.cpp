#include <tools4k/Time.h>
#include <void/Common.h>
#include <void/Engine.h>
#include <void/Server/Server.h>
#include <void/Server/Object.h>

/// ----- ServerObject ----- ///

ServerObject::ServerObject( ObjectId id, ObjectTypeId typeId, HSQOBJECT scriptObject ) :
	Object(id, typeId, scriptObject),
	m_CreationTick(Singleton<ServerObjectManager>()->m_CurrentTick),
	m_ScriptSerialize(false)
{
}

ServerObject::~ServerObject()
{
}

void ServerObject::serialize( OPacket* p, Tick startTick )
{
	Log("Serializing object %d ..\n", id());
	
	bool blockStarted = false;
	
	// Wenn es das Objekt vor startTick noch nicht gab,
	// muss ein OC_Create gesendet werden.
	if(m_CreationTick > startTick)
	{
		p->write<UInt32>("ObjectId", id());
		blockStarted = true;
		
		p->write<UInt8>("Command", OC_Create);
		p->write<UInt32>("TypeId", typeId());
	}
	
	if(Singleton<ServerObjectManager>()->isRegularUpdate(startTick) && !m_ScriptSerialize)
	{
		if(blockStarted)
			p->write<UInt8>("Command", OC_End);
		return;
	}
	
	if(Singleton<ServerObjectManager>()->isRegularUpdate(startTick) && m_ScriptSerialize)
		m_ScriptSerialize = false;
	
	if(!blockStarted)
	{
		p->write<UInt32>("ObjectId", id());
		blockStarted = true;
	}
	
	ServerObjectManager* som = Singleton<ServerObjectManager>();
	
	som->m_SerializeCallback.prepareCall();
	sq_pushobject(Singleton<Squirrel>()->vm(), scriptObject());
	p->pushHandle();
	sq_pushinteger(Singleton<Squirrel>()->vm(), startTick);
	som->m_SerializeCallback.executeCall(3, false, true);
	
	if(blockStarted)
		p->write<UInt8>("Command", OC_End);
}

void ServerObject::pushHandle()
{
	HSQUIRRELVM vm = Singleton<Squirrel>()->vm();
	sq_pushuserpointer(vm, static_cast<Object*>(this));
	sq_setreleasehook(vm, -1, ServerObject::ReleaseObject);
}

ServerObject* ServerObject::GetHandle( int index )
{
	Object* p = NULL;
	sq_getuserpointer(Singleton<Squirrel>()->vm(), index, (SQUserPointer*)&p);
	return static_cast<ServerObject*>(p);
}




/// ----- ServerObjectManager ----- ///

ServerObjectManager::ServerObjectManager() :
	m_NextObjectId(FirstObjectId),
	m_CurrentTick(GenesisTick),
	m_LastTickTime(0.0),
	m_LastUpdateTick(0)
{
}

ServerObjectManager::~ServerObjectManager()
{
}

void ServerObjectManager::update()
{
	double curTime = tools4k::RuntimeInSeconds();
	if(curTime-m_LastTickTime < ServerTimestep)
		return; // TODO: Bad solution. :I
	
	// m_CurrentTick++;
	
	// Hier simulieren und Zeug machen
	Log("%d\n", m_CurrentTick);
	// ...
	
	if(m_CurrentTick-m_LastUpdateTick >= UpdateRate)
	{
		// Regelmäßiges Update schreiben
		
		Log("Writing regular update for tick %d ..\n", m_LastUpdateTick);
		OPacket p(StuffChannel, SM_ObjectUpdate, 0);
		writeUpdate(&p, m_LastUpdateTick);
		Singleton<Server>()->broadcastPacket(&p);
		
		m_LastUpdateTick = m_CurrentTick;
	}
	
	m_LastTickTime = curTime;
	m_CurrentTick++;
}

void ServerObjectManager::onClientConnected( int slot )
{
	Log("Writing complete update for client #%d ..\n", slot);
	OPacket p(StuffChannel, SM_ObjectUpdate, 0);
	writeUpdate(&p, 0);
	Singleton<Server>()->sendPacket(slot, &p);
}

bool ServerObjectManager::isRegularUpdate( Tick startTick ) const
{
	return (startTick == m_LastUpdateTick);
}

ServerObject* ServerObjectManager::createObject( ObjectTypeId typeId, HSQOBJECT scriptObject )
{
	ServerObject* o = new ServerObject(m_NextObjectId, typeId, scriptObject);
	m_NextObjectId++;
	registerObject(o);
	return o;
}

void ServerObjectManager::writeUpdate( OPacket* packet, Tick startTick )
{
	packet->write<UInt32>("StartTick", startTick);
	packet->write<UInt32>("DestinationTick", m_CurrentTick);
	
	std::map<ObjectId,Object*>::iterator i = m_Objects.begin();
	for(; i != m_Objects.end(); i++)
	{
		static_cast<ServerObject*>(i->second)->serialize(packet, startTick);
	}
	packet->write<UInt32>("ObjectId", InvalidObjectId);
}


/// -------------- Squirrel Bindings ------------------ ///

#include <void/Squirrel.h>

SQInteger ServerObjectManager::fn_SetSerializeCallback( HSQUIRRELVM vm ) // env, fun
{
	HSQOBJECT env;
	HSQOBJECT fun;
	sq_getstackobj(vm, 2, &env);
	sq_getstackobj(vm, 3, &fun);
	Singleton<ServerObjectManager>()->m_SerializeCallback = SquirrelFunction(vm, env, fun);
	return 0;
}
RegisterSqFunction(SetSerializeCallback, ServerObjectManager::fn_SetSerializeCallback, 3, "...");

SQInteger ServerObject::ReleaseObject( SQUserPointer up, SQInteger size ) //TODO: Somehow this doesnt gets called
{
	ServerObject* p = static_cast<ServerObject*>((Object*)up);
	Log("Object %d was released automatically!\n", (int)p->id());
	Singleton<ServerObjectManager>()->removeObject(p->id());
	return 1;
}

SQInteger ServerObjectManager::fn_CreateObject( HSQUIRRELVM vm ) // typeId, scriptObject
{
	SQInteger typeId;
	sq_getinteger(vm, 2, &typeId);
	
	HSQOBJECT scriptObject;
	sq_getstackobj(vm, 3, &scriptObject);
	
	Singleton<ServerObjectManager>()->createObject(typeId, scriptObject)->pushHandle();
	return 1;
}
RegisterSqFunction(CreateObject, ServerObjectManager::fn_CreateObject, 3, ".i.");

SQInteger ServerObject::fn_RemoveObject( HSQUIRRELVM vm ) // handle
{
	ServerObject* p = ServerObject::GetHandle(2);
	sq_setreleasehook(vm, 2, NULL); // ReleaseHook würde auch removeObject aufrufen ..
	
	Singleton<ServerObjectManager>()->removeObject(p->id());
	return 0;
}
RegisterSqFunction(RemoveObject, ServerObject::fn_RemoveObject, 2, ".p");

SQInteger ServerObject::fn_ObjectSetDirty( HSQUIRRELVM vm ) // handle
{
	ServerObject* p = ServerObject::GetHandle(2);
	p->m_ScriptSerialize = true;
	return 0;
}
RegisterSqFunction(ObjectSetDirty, ServerObject::fn_ObjectSetDirty, 2, ".p");
