#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <string>

#include <void/Network.h>
#include <void/Singleton.h>

class Client : public Network, public SingletonClass<Client>
{
	public:
		Client();
		virtual ~Client();
		
		void service();
		
		/**
		 * Connects to a server.
		 * This function blocks until either a connection was established
		 * or the timeout was reaced.
		 * Returns true on success.
		 */
		bool connect( const ENetAddress* address );

		/**
		 * Disconnect from the server.
		 * Blocks until server either answers or timeout was reached.
		 * In the last case it forcefully disconnects.
		 * So this function will always succeed.
		 */
		void disconnect();
		
		/**
		 * Enqueues an outgoing packet.
		 * Which is going to be send on the next service() call.
		 */
		void sendPacket( OPacket* packet );
		
	private:
		ENetPeer* m_ServerPeer;
		int m_LocalClientSlot;
};


#endif
