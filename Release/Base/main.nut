/** main script
**/


function include( file )
{
	local c = ::loadfile("Base/"+file);
	return c();
}


include("Object.nut");







/// ---- Object Test ----


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







/// ---- Voxel Test ----



MyTileMap <-
{
	tileSize = 32
	diffuse = ::Texture("Standard.png")
	bump = ::Texture("StandardBump.png")
	glow = ::Texture("StandartGlow.png")
}



enum VoxelShape
{
	None = 0 // Leerer Raum
	Block    // Ein Würfel
	Slope    // etc ..
}

enum VoxelRender
{
	Invisible = 0 // 
	Solid         // Alle Flächen sind undurchsichtig
	Alpha         // Hartes Alphablending (hm.. eventuell auch weiches Blending .. hmm ..)
	Additive      // Wird additiv gerendert (Leuchtet also im Dunklen ^^)
}

RegisterVoxelType(
{
	// Der Name mit dem man den Blocktyp ansprechen kann (IDs werden nur Engine-intern verwendet)
	name = "Rock"
	
	// Form wird für Kollision und  Darstellung verwendet
	shape = VoxelShape.Block
	
	// Wie der Block beim Client dargestllt wird
	render = VoxelRender.Solid
	
	// Name der Textur (d.h. TileMap)
	tileMap = MyTileMap
	
	// Welche Tiles den einzelnen Flächen zugeweisen werden
	faces =
	{
		front  = [0,0]
		back   = [1,0]
		top    = [2,0]
		bottom = [3,0]
		left   = [0,1]
		right  = [1,1]
	}
});

GetVoxelType("Rock");
