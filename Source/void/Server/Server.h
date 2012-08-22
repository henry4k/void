#ifndef __SERVER_H__
#define __SERVER_H__

#include <void/Network.h>
#include <void/Singleton.h>

class Server : public Network, public SingletonClass<Server>
{
	public:
		Server();
		virtual ~Server();
		
		void service();
		
		/**
		 * 
		 */
		int slotByPeer( const ENetPeer* peer );
		
		void sendPacket( int clientSlot, OPacket* packet );
		
		/**
		 * Broadcast to all active clients.
		 */
		void broadcastPacket( OPacket* packet );
		
	private:
		ENetPeer* clientPeerBySlot( int slot );
		void sendClientList( int slot );
};


#endif
