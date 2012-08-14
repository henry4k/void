#include <tools4k/Time.h>
#include <void/SqLoader.h>
#include <void/Network.h>
#include <void/Object.h>
#include <void/Engine.h>





Engine::Engine() :
	m_Squirrel(),
	m_CurrentTime(tools4k::RuntimeInSeconds()),
	m_LastTime(0)
{
}

Engine::~Engine()
{
}

bool Engine::simulate()
{
	m_LastTime = m_CurrentTime;
	m_CurrentTime = tools4k::RuntimeInSeconds();
	double timeDelta = m_CurrentTime - m_LastTime;
	
	return onSimulate(timeDelta);
}

bool Engine::loadPackage( const char* name )
{
	// TODO: Umbauen <- Warum?
	
	HSQUIRRELVM vm = m_Squirrel.vm();
	std::string scriptFile = std::string(name)+"/main.nut";

	sq_pushroottable(vm);
	DoFile(vm, scriptFile.c_str(), false, true, 0);
	sq_pop(vm, 1);

	return true;
}