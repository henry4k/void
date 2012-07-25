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

ObjectTypes <- [];
class ObjectType
{
	Id = 0;
	Class = null;
	Name_ = null;
	
	function constructor( classObject, name = null )
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
	
	function Name()
	{
		return (Name_)?(Name_):(Id.tostring());
	}
}


class NetVar
{
	Value = null;
	Target = null;
	Name_ = null;
	Id = null;
	
	function constructor( value, target = null, name = null )
	{
		Value = value;
		Target = target;
		Name_ = name;
		Id = Target.RegisterNetVar(this);
		// Kein Target.SetDirty() nötig, weil der Konstruktor garantiert
		// auf beiden Seiten (mit den gleichen Werten) aufgerufen wird!
	}
	
	function Serialize()
	{
// 		::PacketWrite(opacket,"Command",'B',ObjectCommand.Update);
// 		::PacketWrite(opacket,"VarId",'B',Id);
	}
	
	function Name()
	{
		return (Name_)?(Name_):(Id.tostring());
	}
	
	
	// Für PoD-Typen:
	
	function Set( v )
	{
		Value = v;
		Target.SetDirty();
	}
	
	function Get()
	{
		return Value;
	}
	
	function Inc()
	{
		Value++;
		Target.SetDirty();
	}
	
	function Dec()
	{
		Value--;
		Target.SetDirty();
	}
}


class Object
{
	Handle = null;
	NetVars = null;
	
	function constructor()
	{
		Handle = ::CreateObject(Type.Id, this);
		NetVars = [];
	}
	
	function _inherited( attributes )
	{
		this.Type <- ::ObjectType(this, ("name" in attributes)?(attributes.name):(null));
	}

	function IsValid()
	{
		return (Handle != null);
	}

	function Id()
	{
		if(!IsValid())
			return null;
		else
			return ::ObjectGetId(Handle);
	}
	
	function Name()
	{
		return Id().tostring()+":"+Type.Name();
	}
	
	function RegisterNetVar( nv )
	{
		if(nv.Name_)
		{
			foreach(v in NetVars)
				::assert(v.Name_ != nv.Name_)
		}
		
		NetVars.append(nv);
		
		return NetVars.len()-1;
	}
	
	function SetDirty()
	{
		::ObjectSetDirty(Handle);
	}
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
function Object::SerializeVariables( opacket, startTick )
{
	::print(Name()+": Serializing script variables ..\n");
	foreach(i,nv in NetVars)
	{
		nv.Serialize(opacket, startTick);
	}
	
// 	::PacketWrite(opacket,"Command",'B',ObjectCommand.Update);
// 	::PacketWrite(opacket,"VarName",'s',"Foo");
// 	::PacketWrite(opacket,"Value",'i',4000);
// 	
// 	::PacketWrite(opacket,"Command",'B',ObjectCommand.Update);
// 	::PacketWrite(opacket,"VarName",'s',"Bar");
// 	::PacketWrite(opacket,"Value",'i',8000);
}




/// ---- Callbacks ----

if(__server__)
function OnSerializeObject( instance, opacket, startTick )
{
	instance.SerializeVariables(opacket, startTick);
}

if(__client__)
function OnCreateClientObject( type, id )
{
	::assert(type in ::ObjectTypes);
	local c = ::ObjectTypes[type];
	
	local i = c.instance();
	i.Id = id;
	::AddObject(i);
	i.constructor();
}