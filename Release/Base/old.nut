/** main script
**/

function filename()
{
	return ::getstackinfos(2).src;
}

function dirname()
{
	local sourceFile = ::filename();
	// ...
}

function include( file )
{
	local c = ::loadfile(::dirname()+file);
	return c();
}



/// Server

if(__server__)
{

function OnSerializeObject( instance, opacket, startTick )
{
	instance.SerializeVariables(opacket, startTick);
}

}

enum ObjectCommand
{
	Create,
	Update,
	Delete,
	End
}

class Object
{
	Handle = null;
	
	function constructor()
	{
		Handle = ::CreateObject(42,this);
	}
	
	function IsValid()
	{
		return (Handle != null);
	}
	
	function Remove()
	{
		if(!IsValid())
			return;
		::RemoveObject(Handle);
		Handle = null;
	}
	
	function Id()
	{
		if(!IsValid())
			return null;
		else
			return ::ObjectGetId(Handle);
	}
	
	function SerializeVariables( opacket, startTick )
	{
		::PacketWrite(opacket,"Command",'B',ObjectCommand.Update);
		::PacketWrite(opacket,"VarName",'s',"Foo");
		::PacketWrite(opacket,"Value",'i',4000);
		
		::PacketWrite(opacket,"Command",'B',ObjectCommand.Update);
		::PacketWrite(opacket,"VarName",'s',"Bar");
		::PacketWrite(opacket,"Value",'i',8000);
	}
}

TestObject <- Object();
// ::print("object id = "+TestObject.Id()+"\n");
ObjectSetDirty(TestObject.Handle);

return;

/// Client

function OnCreateClientObject( p )
{
	local args = p; // DeserializeTable(p);
	
	assert(args.type in ::getroottable());
	
	local c = ::getroottable()[args.type];
	
	local i = c.instance();
	i.Id = args.id;
	::AddObject(i);
	i.constructor(args);
}

local args =
{
	type = "Pistol",
	id = 4000
};
OnCreateClientObject(args);

class Object
{
	Id = 0;
	
	function constructor()
	{
	}
}

class Pistol extends Object
{
	
	
	function constructor()
	{
		
	}
}