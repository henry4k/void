#include <map>
#include <tools4k/Time.h>
#include <void/Common.h>
#include <void/Engine.h>
#include <void/Client/Client.h>
#include <void/Client/Object.h>


const double LocalTimeout = 3.0; // Lokale Werte verfallen nach 3 Sekunden


/// ---- ClientObject ----

ClientObject::ClientObject( ObjectId id, ObjectTypeId typeId, HSQOBJECT scriptObject) :
	Object(id, typeId, scriptObject),
	m_CreationTick(Singleton<ClientObjectManager>()->m_LastUpdateTick)
{
}

ClientObject::~ClientObject()
{
}

void ClientObject::deserialize( IPacket* p )
{
	Log("Deserializing object %d\n", id());
}

// Wird regelmäßig (nicht zwingend jede Sekunde) aufgerufen
// um zu prüfen ob ein lokal gesetzter Wert auf den Serverwert zurückgesetzt werden muss
void ClientObject::checkReset( double curTime )
{
// 	if(lastUpdateTime == -1)
// 		return;
// 	
// 	if(::CurrentTime-LastUpdateTime >= ::LocalTimeout)
// 	{
// 		::print("Local value timed out; Resetting to "+ServerValue+"\n");
// 		LastUpdateTime = -1;
// 		LocalValue = ServerValue;
// 	}
}

SQInteger ClientObject::ReleaseObject( SQUserPointer up, SQInteger size ) //TODO: Somehow this doesnt gets called
{
	ClientObject* p = (ClientObject*)up;
	Log("Object %d was released automatically!\n", (int)p->id());
	Singleton<ClientObjectManager>()->removeObject(p->id());
	return 1;
}

void ClientObject::pushHandle()
{
	HSQUIRRELVM vm = Singleton<Squirrel>()->vm();
	sq_pushuserpointer(vm, static_cast<Object*>(this));
	sq_setreleasehook(vm, -1, ClientObject::ReleaseObject);
}

ClientObject* ClientObject::GetHandle( int index )
{
	Object* p = NULL;
	sq_getuserpointer(Singleton<Squirrel>()->vm(), index, (SQUserPointer*)&p);
	return static_cast<ClientObject*>(p);
}


/// ---- ClientObjectManager ----

ClientObjectManager::ClientObjectManager() :
	m_LastUpdateTick(0)
{
}

ClientObjectManager::~ClientObjectManager()
{
}

void ClientObjectManager::update()
{
	double curTime = tools4k::RuntimeInSeconds();
	
	std::map<ObjectId,Object*>::iterator i = m_Objects.begin();
	for(; i != m_Objects.end(); i++)
	{
		static_cast<ClientObject*>(i->second)->checkReset(curTime);
	}
}

void ClientObjectManager::readUpdate( IPacket* p )
{
	Tick startTick = p->read<UInt32>("StartTick");
	Tick destinationTick = p->read<UInt32>("DestinationTick");
	
	if(m_LastUpdateTick >= destinationTick)
	{
		Log("Ignoring update to %d, cause we are already at %d", destinationTick, m_LastUpdateTick);
		return;
	}
	
	m_LastUpdateTick = destinationTick;
	
	HSQUIRRELVM vm = Singleton<Squirrel>()->vm();
	
	for(;;)
	{
		ObjectId id = p->read<UInt32>("ObjectId");
		if(id == InvalidObjectId)
			break;
		
		for(;;)
		{
			int command = p->read<UInt8>("Command");
			if(command == OC_End)
				break;
			
			switch(command)
			{
				case OC_Create:
				{
					int typeId = p->read<UInt32>("TypeId");
					Object* o = getObject(id);
					if(o)
					{
						assert(o->typeId() == typeId);
					}
					else
					{
						m_ObjectCreationCallback.prepareCall();
						sq_pushinteger(vm, typeId);
						sq_pushinteger(vm, id);
						p->pushHandle();
						m_ObjectCreationCallback.executeCall(3, false, true);
					}
				} break;
				
				case OC_Update:
				{
					m_ObjectUpdateCallback.prepareCall();
					Object* op = getObject(id);
					if(op == NULL)
						Error("Invalid object id!");
					sq_pushobject(vm, op->scriptObject());
					p->pushHandle();
					m_ObjectUpdateCallback.executeCall(2, false, true);
				} break;
				
				case OC_Delete:
				{
				} break;
				
				default:
				{
					// ...
				}
			}
		}
	}
}

SQInteger ClientObjectManager::fn_AddObject( HSQUIRRELVM vm ) // typeId, id, scriptObject
{
	SQInteger typeId;
	SQInteger id;
	HSQOBJECT scriptObject;
	
	sq_getinteger(vm, 2, &typeId);
	sq_getinteger(vm, 3, &id);
	sq_getstackobj(vm, 4, &scriptObject);
	
	ClientObject* o = new ClientObject(id, typeId, scriptObject);
	Singleton<ClientObjectManager>()->registerObject(o);
	
	o->pushHandle();
	return 1;
}
RegisterSqFunction(AddObject, ClientObjectManager::fn_AddObject, 4, ".ii.");

SQInteger ClientObjectManager::fn_SetObjectCreationCallback( HSQUIRRELVM vm ) // env, fun
{
	HSQOBJECT env;
	HSQOBJECT fun;
	sq_getstackobj(vm, 2, &env);
	sq_getstackobj(vm, 3, &fun);
	Singleton<ClientObjectManager>()->m_ObjectCreationCallback = SquirrelFunction(vm, env, fun);
	return 0;
}
RegisterSqFunction(SetObjectCreationCallback, ClientObjectManager::fn_SetObjectCreationCallback, 3, "...");

SQInteger ClientObjectManager::fn_SetObjectUpdateCallback( HSQUIRRELVM vm ) // env, fun
{
	HSQOBJECT env;
	HSQOBJECT fun;
	sq_getstackobj(vm, 2, &env);
	sq_getstackobj(vm, 3, &fun);
	Singleton<ClientObjectManager>()->m_ObjectUpdateCallback = SquirrelFunction(vm, env, fun);
	return 0;
}
RegisterSqFunction(SetObjectUpdateCallback, ClientObjectManager::fn_SetObjectUpdateCallback, 3, "...");