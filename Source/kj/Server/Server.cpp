#include <kj/Common.h>
#include <kj/Packet.h>
#include <kj/Server/Server.h>




Server::Server()
{
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = 1234;
	
	m_Host = enet_host_create(
		&address, // the address to bind the server host to 
		ClientHardLimit, // allow up to 32 clients and/or outgoing connections
		ChannelCount,  // allow up to 2 channels to be used, 0 and 1
		0,  // assume any amount of incoming bandwidth
		0   // assume any amount of outgoing bandwidth
	);

	if(m_Host == NULL)
		Error("An error occurred while trying to create the server.\n");
}

Server::~Server()
{
}

void Server::service()
{
	ENetEvent event;
	while(enet_host_service(m_Host, &event, NetworkServiceTimeout) > 0)
	{
		switch(event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
			{
				int slot = slotByPeer(event.peer);
				
				Log("New client %d connected from %x:%u.\n",
					slot,
					event.peer->address.host,
					event.peer->address.port
				);
				
				ClientInfo* ci = clientInfoBySlot(slot);
				ci->activate(slot);
				event.peer->data = ci;
				
				OPacket p(StuffChannel, 0, ENET_PACKET_FLAG_RELIABLE);
				p.write("Text", "Hello client, I'm the server.");
				p.write<Int32>("Number", 4000);
				sendPacket(slot, &p);
			} break;
			
			case ENET_EVENT_TYPE_RECEIVE:
			{
				Log("Channel=%u [%s]\n",
					event.channelID,
					(const char*)event.packet->data
				);
				enet_packet_destroy(event.packet);
			} break;
			
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				int slot = slotByPeer(event.peer);
				Log("%d disconnected.\n", slot);
				clientInfoBySlot(slot)->reset();
				event.peer->data = NULL;
			} break;
			
			default:
			{
			}
		}
	}
}

int Server::slotByPeer( const ENetPeer* peer )
{
	return peer->incomingPeerID;
}

void Server::sendPacket( int clientSlot, OPacket* packet )
{
	enet_peer_send(clientPeerBySlot(clientSlot), packet->channel(), packet->enetPacket());
	// enet_host_flush(m_Host);
}

void Server::broadcastPacket( OPacket* packet )
{
	enet_host_broadcast(m_Host, packet->channel(), packet->enetPacket());
	// enet_host_flush(m_Host);
}

ENetPeer* Server::clientPeerBySlot( int slot )
{
	return &m_Host->peers[slot];
}

// int Server::nextClientSlot( int i )
// {
// 	i = (i == InvalidSlot)?(0):(i+1);
// 	for(; i < m_Host->peerCount; i++)
// 	{
// 		if(m_Host->peers[i].data)
// 			return i;
// 	}
// 	return InvalidSlot;
// }