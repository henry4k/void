#include <map>
#include <tools4k/Time.h>
#include <void/Common.h>
#include <void/Engine.h>
#include <void/Server/Server.h>
#include <void/Server/Object.h>




ServerObjectManager::ServerObjectManager( Squirrel* squirrel ) :
	ObjectManager(squirrel)
{
}

ServerObjectManager::~ServerObjectManager()
{
}

void ServerObjectManager::update()
{
	// ...
}



/*



extern std::map<ObjectId,Object*> Objects; // TODO: B-b- BAAKAAA!!!!

int NextObjectId = FirstObjectId;
Tick CurrentTick = GenesisTick;
double LastTickTime = 0;
Tick LastUpdateTick = 0;
HSQOBJECT OnWriteUpdateFn;

inline bool IsRegularUpdate( Tick startTick ) { return (startTick == LastUpdateTick); }

struct ServerObject
{
	Tick creationTick; // Tick when the object was created
	bool scriptSerialize; // when set, the next regular update calls the script function "OnSerializeObject( instance, opacket, startTick )"
};

void CreateObject_( Object* object )
{
	ServerObject* so = Alloc(ServerObject,1);
	object->custom = so;
	so->creationTick = CurrentTick;
	so->scriptSerialize = false;
	// ...
}

void FreeObject_( Object* object )
{
	ServerObject* so = (ServerObject*)object->custom;
	// ...
	Free(so);
}

Object* CreateNewObject( unsigned int typeId, HSQOBJECT scriptObject )
{
	Object* o = (Object*)CreateObject(NextObjectId, typeId, scriptObject);
	NextObjectId++;
	return o;
}

bool InitObjectManager_()
{
	NextObjectId = FirstObjectId;
	
	return true;
}

void DeinitObjectManager_()
{
}

void SerializeObject( Object* o, OPacket* p, Tick startTick )
{
	Log("Serializing object %d ..\n", o->id);
	
	bool blockStarted = false;
	
	ServerObject* so = (ServerObject*)o->custom;
	if(so->creationTick > startTick)
	{
		PacketWriteUInt(p, "ObjectId", o->id);
		blockStarted = true;
		
		PacketWriteUByte(p, "Command", OC_Create);
		PacketWriteUInt(p, "TypeId", o->typeId);
	}
	
	if(IsRegularUpdate(startTick) && !so->scriptSerialize)
	{
		if(blockStarted)
			PacketWriteUByte(p, "Command", OC_End);
		return;
	}
	
	if(IsRegularUpdate(startTick) && so->scriptSerialize)
		so->scriptSerialize = false;
	
	if(!blockStarted)
	{
		PacketWriteUInt(p, "ObjectId", o->id);
		blockStarted = true;
	}
	
	// "OnSerializeObject( instance, opacket, startTick )"
	HSQUIRRELVM vm = GetVm();
	sq_pushroottable(vm);
	sq_pushstring(vm, "OnSerializeObject", -1);
	if(SQ_FAILED(sq_get(vm, -2)))
	{
		sq_pop(vm, 1);
		Log("Can't find script function 'OnSerializeObject'\n");
		return;
	}
	sq_pushroottable(vm); // this
	sq_pushobject(vm, o->scriptObject);
	sq_pushuserpointer(vm, p); // pointer ist nur diesen funktionsaufruf lang gültig! (macht sonst auch wenig sinn)
	sq_pushinteger(vm, startTick);
	sq_call(vm, 4, false, true);
	sq_pop(vm, 2); // pop roottable and function
	
	if(blockStarted)
		PacketWriteUByte(p, "Command", OC_End);
}

void WriteUpdate( Tick startTick )
{
	Log("Writing update for tick %d ..\n", startTick);
	
	OPacket p;
	CreateOPacket(&p, StuffChannel, SM_ObjectUpdate);
	PacketWriteUInt(&p,"StartTick",startTick);
	PacketWriteUInt(&p,"DestinationTick",CurrentTick);
	
	std::map<ObjectId,Object*>::iterator i = Objects.begin();
	for(; i != Objects.end(); i++)
	{
		SerializeObject(i->second, &p, startTick);
	}
	PacketWriteUInt(&p, "ObjectId", InvalidObjectId);
	
	BroadcastPacket(&p);
	
	FreeOPacket(&p);
}

void UpdateObjectManager_()
{
	double curTime = RuntimeInSeconds();
	if(curTime-LastTickTime < ServerTimestep)
		return; // TODO: Bad solution. :I
	
	// CurrentTick++;
	
	// Hier simulieren und Zeug machen
	Log("%d\n", CurrentTick);
	// ...
	
	if(CurrentTick-LastUpdateTick >= UpdateRate)
	{
		// Regelmäßiges Update schreiben
		
		WriteUpdate(LastUpdateTick);
		LastUpdateTick = CurrentTick;
	}
	
	LastTickTime = curTime;
	CurrentTick++;
}




/// -------------- Squirrel Bindings ------------------ ///

SQInteger ReleaseObject( SQUserPointer up, SQInteger size ) //TODO: Somehow this doesnt gets called
{
	Object* p = (Object*)up;
	Log("Object %d was released automatically!\n", (int)p->id);
	RemoveObject(p->id);
	return 1;
}

SQInteger fn_CreateObject( HSQUIRRELVM vm ) // typeId, scriptObject
{
	SQInteger typeId;
	sq_getinteger(vm, 2, &typeId);
	
	HSQOBJECT scriptObject;
	sq_getstackobj(vm, 3, &scriptObject);
	
	sq_pushuserpointer(vm, CreateNewObject(typeId, scriptObject));
	sq_setreleasehook(vm, -1, ReleaseObject);
	return 1;
}

SQInteger fn_RemoveObject( HSQUIRRELVM vm ) // handle
{
	Object* p = NULL;
	sq_getuserpointer(GetVm(), 2, (SQUserPointer*)&p);
	
	sq_setreleasehook(vm, 2, NULL); // ReleaseHook würde auch RemoveObject aufrufen ..
	
	RemoveObject(p->id);
	return 0;
}

SQInteger fn_ObjectSetDirty( HSQUIRRELVM vm ) // handle
{
	Object* p = NULL;
	sq_getuserpointer(GetVm(), 2, (SQUserPointer*)&p);
	ServerObject* so = (ServerObject*)p->custom;
	so->scriptSerialize = true;
	return 0;
}

void RegisterObjectManager_()
{
	RegisterSqFunction("CreateObject", fn_CreateObject, 3, ".i.");
	RegisterSqFunction("RemoveObject", fn_RemoveObject, 2, ".p");
	RegisterSqFunction("ObjectSetDirty", fn_ObjectSetDirty, 2, ".p");
}

*/