/** Object class (and its utillity functions/classes)
**/

// Command ids for object updates
enum ObjectCommand
{
	Create,
	Update,
	Delete,
	End
}


/// ---- ObjectType ----

ObjectTypes <- [];
class ObjectType
{
	Id = 0;
	Class = null;
	Name_ = null;
}
	
function ObjectType::constructor( classObject, name = null )
{
	// Code wird auf Client und Server in gleicher Reihenfolge ausgeführt,
	// entsprechend haben die Objekt-Typen auch die gleiche Reihenfolge.
	
	if(name)
	{
		foreach(v in ::ObjectTypes)
		{
			if(v.Name_ == name)
			{
				::print("An object type with this name already exists\n");
				::assert(false);
			}
		}
	}
	
	::ObjectTypes.append(this);
	
	Id = ::ObjectTypes.len()-1;
	Class = classObject.weakref(); // Warum weakref?
	Name_ = name;
}
	
function ObjectType::Name()
{
	return (Name_)?(Name_):(Id.tostring());
}



/// ---- NetVar ----

class NetVar
{
	Value = null;
	Target = null;
	Name_ = null;
	Id = null;
}

function NetVar::constructor( value, target = null, name = null )
{
	Value = value;
	Target = target;
	Name_ = name;
	Id = Target.RegisterNetVar(this);
	// Kein Target.SetDirty() nötig, weil der Konstruktor garantiert
	// auf beiden Seiten (mit den gleichen Werten) aufgerufen wird!
}
	
function NetVar::Name()
{
	return (Name_)?(Name_):(Id.tostring());
}
	
	
// Für PoD-Typen:

function NetVar::Set( v )
{
	Value = v;
	if(__server__)
		Target.SetDirty();
}

function NetVar::Get()
{
	return Value;
}

function NetVar::Inc()
{
	Value++;
	if(__server__)
		Target.SetDirty();
}

function NetVar::Dec()
{
	Value--;
	if(__server__)
		Target.SetDirty();
}

if(__server__)
function NetVar::Serialize( opacket, startTick )
{
	::print("NetVar::Serialize\n");
	::PacketWrite(opacket,"Command",'B',ObjectCommand.Update);
	::PacketWrite(opacket,"VarId",'B',Id);
	::PacketWrite(opacket,"Value",'i',Value);
}

if(__client__)
function NetVar::Deserialize( ipacket )
{
	::print("NetVar::Deserialize\n");
	Value = ::PacketRead(ipacket,"Value",'i');
}




/// ---- Object ----

class Object
{
	Handle = null;
	NetVars = null;
}

function Object::_inherited( attributes )
{
	this.Type <- ::ObjectType(this, ("name" in attributes)?(attributes.name):(null));
}

function Object::IsValid()
{
	return (Handle != null);
}

function Object::Id()
{
	if(!IsValid())
		return null;
	else
		return ::ObjectGetId(Handle);
}

function Object::Name()
{
	return Id().tostring()+":"+Type.Name();
}

function Object::RegisterNetVar( nv )
{
	if(nv.Name_)
	{
		foreach(v in NetVars)
			::assert(v.Name_ != nv.Name_)
	}
	
	NetVars.append(nv);
	
	return NetVars.len()-1;
}

if(__server__)
function Object::constructor()
{
	Handle = ::CreateObject(Type.Id, this);
	NetVars = [];
}

if(__client__)
function Object::constructor()
{
	::assert(Handle != null);
	NetVars = [];
}

if(__server__)
function Object::SetDirty()
{
	::ObjectSetDirty(Handle);
}

if(__server__)
function Object::Remove()
{
	if(!IsValid())
		return;
	::RemoveObject(Handle);
	Handle = null;
}

if(__server__)
function Object::Serialize( opacket, startTick )
{
	::print(Name()+": Serializing script variables ..\n");
	foreach(i,nv in NetVars)
	{
		nv.Serialize(opacket, startTick);
	}
}

if(__client__)
function Object::DeserializeVar( ipacket )
{
	::print(Name()+": Deserializing script variable ..\n");
	
	local varId = ::PacketRead(ipacket,"VarId",'B');
	NetVars[varId].Deserialize(ipacket);
}



/// ---- Callbacks ----

if(__server__)
{
	function SerializeCallback( instance, opacket, startTick )
	{
		::print("SerializeCallback("+instance+", "+opacket+", "+startTick+")\n");
		
		instance.Serialize(opacket, startTick);
	}
	::SetSerializeCallback(this, SerializeCallback);
}

if(__client__)
{
	function ObjectCreationCallback( type, id, ipacket )
	{
		::print("ObjectCreationCallback("+type+", "+id+", "+ipacket+")\n");
		
		::assert(type in ::ObjectTypes);
		local c = ::ObjectTypes[type];
		
		local i = c.Class.instance();
		i.Handle = ::AddObject(type, id, i);
		::print("AddObject(...) = "+i.Handle+"\n");
		i.constructor();
		::print("Constructed!\n");
	}
	::SetObjectCreationCallback(this, ObjectCreationCallback);
	
	function ObjectUpdateCallback( instance, ipacket )
	{
		::print("ObjectUpdateCallback("+instance+", "+ipacket+")\n");
		
		instance.DeserializeVar(ipacket);
	}
	::SetObjectUpdateCallback(this, ObjectUpdateCallback);
}
