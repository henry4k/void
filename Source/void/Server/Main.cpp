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
		bool onSimulate( double timeDelta );
		
		Server m_Server;
		Map m_Map;
};


ServerEngine::ServerEngine() : 
	Engine(),
	m_Server(),
	m_Map()
{
	// Setup Squirrel
	m_Squirrel.setConst("__server__", 1);
	m_Squirrel.setConst("__client__", 0);
}

bool ServerEngine::initialize()
{
	return true;
}

ServerEngine::~ServerEngine()
{
}

bool ServerEngine::onSimulate(double timeDelta)
{
	m_Server.service();
	return true;
}



int main( int argc, char* argv[] )
{
	ServerEngine engine;
	if(!engine.initialize())
		return 0;
	while(engine.simulate())
		;
	return 0; 
}