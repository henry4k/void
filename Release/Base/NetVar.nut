/** Netzwerk Variablen
**/

/// --------------- Server ------------------ //


if(__server__)
{

// Tick der gerade simuliert wird
CurrentTick <- 1; // Spiel beginnt bei Tick 1!

// Wann zuletzt ein regemäßiges Update geschrieben wurde
LastUpdateTick <- 0;


class ServerVar
{
	// Wann die Variable zuletzt geändert wurde
	LastUpdate = 0;
	
	Value = null;
	
	
	function constructor()
	{
	}
	
	function Get()
	{
		return Value;
	}
	
	// Wert und Veränderungstick setzen
	function Set( v )
	{
		Value = v;
		LastUpdate = ::CurrentTick;
		::print("Set to "+v+" on tick "+LastUpdate+"\n");
	}
	
	// Wert serialisieren, wenn jünger als <tick>
	function Serialize( tick )
	{
		if(LastUpdate > tick)
		{
			::print("Serializing "+Value+"\n");
		}
	}
}

TestVar <- ServerVar();
TestVar.Set(4000);

// Update was von <tick> zum aktuellen Stand aktualisiert
function WriteUpdate( tick )
{
	::print("Writing update for tick "+tick+" ..\n");
	// D.h. alle Objekte die nach <tick> aktualisiert wurden, werden gesendet!
	TestVar.Serialize(tick);
}

// Simulieren ...
for(; ::CurrentTick < 20; ::CurrentTick++)
{
	::print(::CurrentTick+"\n");
	
	// Hier simulieren und Zeug machen
	
	if(::CurrentTick == 9)
		::TestVar.Set(8000);
	
	// Regelmäßiges Update schreiben
	if(::CurrentTick-::LastUpdateTick >= 3)
	{
		::WriteUpdate(::LastUpdateTick);
		::LastUpdateTick = ::CurrentTick;
	}
}


// Angenommen jemand verpasst einige Ticks weil er später joint (was immer der fall ist)
// Dann muss ein komplettes Update geschrieben werden (also kein Differenz-Update)
::print("------------\n");
::print("Writing complete update ..\n");
::WriteUpdate(0);

}


/// --------------- Client ------------------ ///

if(__client__)
{

// Wann zuletzt ein Update vom Server erhalten wurde
LastUpdateTick <- 0;
// Auf dem Client gibt es kein CurrentTick weil hier die Simulation nur zur Lagkompensation dient
// (also damit die Pausen zwischen den Ticks nicht bemerkt werden)

// ^- Öhm.. scheinbar .. wird die Variable *hier* garnicht benötigt .. mhm


// Aktuelle Zeit (hier als Variable .. später als Funktion, welche z.B. die Spielzeit in Sekunden zurückgibt .. oder so)
CurrentTime <- 0;

LocalTimeout <- 3; // Lokale Werte verfallen nach 3 Sekunden


class ClientVar
{
	// Letzter vom Server empfangener Wert, auf welchen zurückgesetzt wird falls Desync vermutet wird.
	// Wenn die Variable vom Client geändert wird, geht die Engine davon aus das der Wert demnächst vom Server bestätigt wird
	// d.h. das ein Update ankommt wo die Variable aktualisiert wird
	//
	// Die Variable kann überhaupt nur vom Client geändert werden, damit Dinge die dieser Client selbst auslöst
	// bei ihm ohne Verzögherung sichtbar sind.
	// Z.B. Client feuert Pistole ab und *sofort* wird die Munitionsvariable dekrementiert.
	// Falls an der Verbindung etwas nicht stimmt, oder der Schuss vom Server als ungültig erklärt wird,
	// dann wird die Munitionsvariable automatisch nach N Ticks zurückgesetzt.
	ServerValue = null;
	LocalValue = null;
	
	// Wann lokal zuletzt verändert wurde (Ein Zeitstempel, kein Simulationstick .. siehe >LastUpdateTick)
	LastUpdateTime = -1; // -1 für deaktiviert (siehe Server/ClientValue)
	
	
	function constructor()
	{
	}
	
	function Get()
	{
		return LocalValue;
	}
	
	// Wert und Veränderungszeit setzen
	function Set( v )
	{
		LocalValue = v;
		LastUpdateTime = ::CurrentTime;
		::print("Locally set to "+v+" on "+LastUpdateTime+" seconds\n");
	}
	
	// Ein Update vom Server
	function Deserialize( value )
	{
		::print("Deserializing "+value+"\n");
		LastUpdateTime = -1;
		LocalValue = ServerValue = value;
	}
	
	// Wird regelmäßig (nicht zwingend jede Sekunde) aufgerufen
	// um zu prüfen ob ein lokal gesetzter Wert auf den Serverwert zurückgesetzt werden muss
	function CheckReset()
	{
		if(LastUpdateTime == -1)
			return;
		
		if(::CurrentTime-LastUpdateTime >= ::LocalTimeout)
		{
			::print("Local value timed out; Resetting to "+ServerValue+"\n");
			LastUpdateTime = -1;
			LocalValue = ServerValue;
		}
	}
}


TestVar <- ClientVar();

// Die Zeit vergeht ..
for(; ::CurrentTime < 20; ::CurrentTime++)
{
	::print(::CurrentTime+"\n");
	
	// if(!(::CurrentTime % 2)) // alle zwei Sekunden testen
	{
		TestVar.CheckReset();
	}
	
	if(::CurrentTime == 5)
	{
		TestVar.Deserialize(4000);
	}
	
	if(::CurrentTime == 10)
	{
		TestVar.Set(8000);
	}
	
	if(::CurrentTime == 18)
	{
		TestVar.Deserialize(8000);
	}
}


}