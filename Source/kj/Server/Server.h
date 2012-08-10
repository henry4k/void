#ifndef __SERVER_H__
#define __SERVER_H__

#include <kj/Network.h>


class Server : public Network
{
	public:
		Server();
		virtual ~Server();
		
		void service();
		
		int slotByPeer( const ENetPeer* peer );
		
		void sendPacket( int clientSlot, OPacket* packet );
		void broadcastPacket( OPacket* packet );
		
	private:
		ENetPeer* clientPeerBySlot( int slot );
};


#endif