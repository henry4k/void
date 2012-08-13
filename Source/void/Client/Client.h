#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <void/Network.h>


class Client : public Network
{
	public:
		Client();
		virtual ~Client();
		
		void service();
		bool connect( const ENetAddress* address );
		void disconnect();
		
		void sendPacket( OPacket* packet );
		
	private:
		ENetPeer* m_ServerPeer;
};


#endif