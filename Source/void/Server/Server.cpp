#include <void/Common.h>
#include <void/Packet.h>
#include <void/Server/Object.h>
#include <void/Server/Server.h>




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

// int Server::getFreeSlot()
// {
// 	for(int i = 0; i < m_ClientInfos.size(); ++i)
// 	{
// 		if(!m_ClientInfos[i].isActive())
// 			return i;
// 	}
// }

void Server::sendClientList( int slot )
{
	OPacket p(StuffChannel, SM_ClientList, 0);
	
	for(int i = 0; i < m_ClientInfos.size(); ++i)
	{
		ClientInfo* ci = &m_ClientInfos[i];
		if(!ci->isActive())
			continue;
		
		p.write<Int32>("Slot", ci->slot());
		p.write("Name", std::string(ci->name()));
	}
	
	sendPacket(slot, &p);
}

void Server::service()
{
	ENetEvent event;
	while(enet_host_service(m_Host, &event, NetworkServiceTimeout) > 0)
	{
		int slot = slotByPeer(event.peer);
		ClientInfo* ci = clientInfoBySlot(slot);
		
		switch(event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
			{
				Log("New client %d connected from %x:%u.\n",
					slot,
					event.peer->address.host,
					event.peer->address.port
				);
				
				event.peer->data = ci;
				assert(slot == ci->slot());
			} break;
			
			case ENET_EVENT_TYPE_RECEIVE:
			{
				IPacket p(event.channelID, event.packet);
				switch(p.messageId())
				{
					case CM_LoginRequest:
					{
						std::string name = p.readString("Name");
						
						// Broadcast SM_ClientConnected to all other clients
						{
							OPacket p(StuffChannel, SM_ClientConnected, 0);
							p.write<Int32>("Slot", slot);
							p.write("Name", name);
							broadcastPacket(&p);
						}
						
						// Activate it too, send SM_LoginResponse and SM_ClientList
						ci->activate(name);
						
						{
							OPacket p(StuffChannel, SM_LoginResponse, 0);
							p.write<Int32>("Slot", slot);
						}
						
						sendClientList(slot);
						
						Singleton<ServerObjectManager>()->onClientConnected(slot);
					} break;
					
					default:
					{
						// Wenn sonst nix passt wirds ans Script geleitet
						m_PacketCallback.prepareCall();
						p.pushHandle();
						m_PacketCallback.executeCall(1, false, true);
						
// 						Error("Got unknown network message");
					}
				}
				
				enet_packet_destroy(event.packet);
			} break;
			
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				int slot = slotByPeer(event.peer);
				Log("%d disconnected.\n", slot);
				
				clientInfoBySlot(slot)->reset();
				event.peer->data = NULL;
				
				OPacket p(StuffChannel, SM_ClientDisconnected, 0);
				p.write<Int32>("Slot", slot);
				broadcastPacket(&p);
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
	for(int i = 0; i < m_ClientInfos.size(); ++i)
	{
		if(m_ClientInfos[i].isActive())
			sendPacket(i, packet);
	}
	
	// enet_host_broadcast(m_Host, packet->channel(), packet->enetPacket());
	// enet_host_flush(m_Host);
}

ENetPeer* Server::clientPeerBySlot( int slot )
{
	return &m_Host->peers[slot];
}

SQInteger fn_SendPacket( HSQUIRRELVM vm ) // client, packet
{
	SQInteger clientSlot;
	sq_getinteger(vm, 2, &clientSlot);
	OPacket* p = OPacket::GetHandle(3);
	
	Singleton<Server>()->sendPacket(clientSlot, p);
	return 0;
}
RegisterSqFunction(SendPacket, fn_SendPacket, 3, ".ip");

SQInteger fn_BroadcastPacket( HSQUIRRELVM vm ) // packet
{
	OPacket* p = OPacket::GetHandle(2);
	
	Singleton<Server>()->broadcastPacket(p);
	return 0;
}
RegisterSqFunction(BroadcastPacket, fn_BroadcastPacket, 2, ".p");