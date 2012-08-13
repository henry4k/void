#include <void/Common.h>
#include <void/Engine.h>
#include <void/Server/Server.h>
#include <void/Server/Object.h>
#include <void/Squirrel.h>

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