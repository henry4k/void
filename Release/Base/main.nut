/** main script
**/


function include( file )
{
	local c = ::loadfile("Base/"+file);
	return c();
}


include("Object.nut");

class Pistol extends Object
</ name = "Pistol" />
{
	Ammo = null
	
	function constructor()
	{
		base.constructor()
		Ammo = ::NetVar(10,this,"Ammo")
	}
	
	function Fire()
	{
		Ammo.Dec() // Shortcut für Ammo.Set(Ammo.Get()-1)
	}
}



class LaserPistol extends Pistol
</ name = "LaserPistol" />
{
	Energy = null
	
	function constructor()
	{
		base.constructor()
		Energy = ::NetVar(100,this,"Energy")
	}
}



MyPistol <- LaserPistol()
MyPistol.Fire()

return;



class Pistol extends Object {}

Pistol.Ammo <- NetVar(0)
Pistol.Icon <- "Pistol"
Pistol.Size <- 10

function Pistol::constructor()
{
	base.constructor()
	Size = 11
}

function Pistol::Fire()
{
	Ammo.Dec
}





MyPistol <- Pistol();

MyPistol.Ammo.set(5);
MyPistol.Fire();