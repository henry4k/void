#include <kj/Common.h>
#include <kj/Engine.h>
#include <kj/Server/Server.h>
#include <kj/Server/Object.h>
#include <kj/Squirrel.h>

int main()
{
	Squirrel squirrel;
	squirrel.setConst("__server__", 1);
	squirrel.setConst("__client__", 0);
	
	Server server;
	ServerObjectManager objectManager(&squirrel);
	
	for(;;)
		server.service();
	
	return 0;
}