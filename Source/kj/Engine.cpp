#include <tools4k/Time.h>
#include <kj/SqLoader.h>
#include <kj/Squirrel.h>
#include <kj/Network.h>
#include <kj/Object.h>
#include <kj/Engine.h>





Engine::Engine() :
	m_CurrentTime(tools4k::RuntimeInSeconds()),
	m_LastTime(0)
{
// 	if(!InitSquirrel())
// 		return false;
// 
// 	RegisterPacket();
// 	RegisterObjectManager();
// 
// 	if(!InitObjectManager())
// 		return false;
// 	if(!InitNetwork())
// 		return false;
// 
// 	if(!loadPackage("Base"))
// 		return false;
}

Engine::~Engine()
{
// 	DeinitNetwork();
// 	DeinitObjectManager();
// 	DeinitSquirrel();
}

void Engine::simulate()
{
// 	m_LastTime = m_CurrentTime;
// 	m_CurrentTime = RuntimeInSeconds();
// 	double timeDelta = m_CurrentTime - m_LastTime;
// 
// 	NetworkService();
// 	UpdateObjectManager();
}

bool Engine::loadPackage( const char* name )
{
	// TODO: Umbauen
	
// 	HSQUIRRELVM vm = GetVm();
// 	char* scriptFile = Concat2Strings(name,"/main.nut");
// 
// 	sq_pushroottable(vm);
// 	DoFile(vm, scriptFile, false, true, 0);
// 	sq_pop(vm, 1);
// 
// 	Free(scriptFile);

	return true;
}