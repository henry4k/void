#ifndef __SERVER_OBJECT_H__
#define __SERVER_OBJECT_H__

#include <kj/Object.h>


class ServerObjectManager : public ObjectManager
{
	public:
		ServerObjectManager( Squirrel* squirrel );
		virtual ~ServerObjectManager();
		
		void update();
};

#endif