#include <void/Common.h>
#include <void/Engine.h>
#include <void/Map.h>
#include <void/Server/Server.h>
#include <void/Server/Object.h>
#include <void/Squirrel.h>



class ServerEngine : public Engine
{
	public:
		ServerEngine();
		bool initialize();
		virtual ~ServerEngine();
		
	private:
	bool onUpdate( double timeDelta );
		
		Server m_Server;
		ServerObjectManager m_ObjectManager;
		Map m_Map;
};


ServerEngine::ServerEngine()
{
	// Setup Squirrel
	m_Squirrel.setConst("__server__", 1);
	m_Squirrel.setConst("__client__", 0);
}

bool ServerEngine::initialize()
{
	if(!loadPackage("Base"))
		return false;
	
	return true;
}

ServerEngine::~ServerEngine()
{
}

bool ServerEngine::onUpdate(double timeDelta)
{
	m_Server.service();
	m_ObjectManager.update();
	return true;
}



int main( int argc, char* argv[] )
{
	ServerEngine engine;
	if(!engine.initialize())
		return 0;
	while(engine.update())
		;
	return 0; 
}
