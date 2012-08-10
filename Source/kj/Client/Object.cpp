#include <map>
#include <tools4k/Time.h>
#include <kj/Common.h>
#include <kj/Engine.h>
#include <kj/Client/Client.h>
#include <kj/Client/Object.h>

/*
extern std::map<ObjectId,Object*> Objects; // TODO: B-b- BAAKAAA!!!!


// Wann zuletzt ein Update vom Server erhalten wurde
Tick LastUpdateTick = 0;
// Auf dem Client gibt es kein CurrentTick weil hier die Simulation nur zur Lagkompensation dient
// (also damit die Pausen zwischen den Ticks nicht bemerkt werden)

// ^- Öhm.. scheinbar .. wird die Variable *hier* garnicht benötigt .. mhm

#define LocalTimeout 3.0 // Lokale Werte verfallen nach 3 Sekunden


struct ClientObject
{
	// ...
};

void CreateObject_( Object* object )
{
	ClientObject* co = Alloc(ClientObject,1);
	object->custom = co;
	// ...
}

void FreeObject_( Object* object )
{
	ClientObject* co = (ClientObject*)object->custom;
	// ...
	Free(object->custom);
}

// Ein Update vom Server
void DeserializeObject( Object* o, IPacket* p )
{
	Log("Deserializing object %d\n", o->id);
	// ...
}

// Wird regelmäßig (nicht zwingend jede Sekunde) aufgerufen
// um zu prüfen ob ein lokal gesetzter Wert auf den Serverwert zurückgesetzt werden muss
void ObjectCheckReset( Object* o, double curTime )
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

bool InitObjectManager_()
{
	return true;
}

void DeinitObjectManager_()
{
}

void UpdateObjectManager_()
{
	double curTime = RuntimeInSeconds();
	
	std::map<ObjectId,Object*>::iterator i = Objects.begin();
	for(; i != Objects.end(); i++)
	{
		ObjectCheckReset(i->second, curTime);
	}
}

void OnWorldUpdate( IPacket* p )
{
	Tick startTick = PacketReadUInt(p,"StartTick");
	Tick destinationTick = PacketReadUInt(p,"DestinationTick");
	
	// DeserializeObject
}


/// -------------- Squirrel Bindings ------------------ ///

#include "../Squirrel.h"

void RegisterObjectManager_()
{
	// ...
}

*/