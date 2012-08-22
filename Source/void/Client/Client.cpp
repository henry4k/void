#include <void/Common.h>
#include <void/Packet.h>
#include <void/Client/Object.h>
#include <void/Client/Client.h>


Client::Client() :
	m_ServerPeer(NULL),
	m_LocalClientSlot(InvalidSlot)
{
	m_Host = enet_host_create(
		NULL, // create a client host
		1, // only allow 1 outgoing connection
		ChannelCount, // allow up 2 channels to be used, 0 and 1
		57600 / 8, // 56K modem with 56 Kbps downstream bandwidth
		14400 / 8 // 56K modem with 14 Kbps upstream bandwidth
	);

	if(m_Host == NULL)
		Error("An error occurred while trying to create the client.\n");
}

Client::~Client()
{
}

void Client::service()
{
	ENetEvent event;
	while(enet_host_service(m_Host, &event, NetworkServiceTimeout) > 0)
	{
		switch(event.type)
		{
			case ENET_EVENT_TYPE_RECEIVE:
			{
				IPacket p(event.channelID, event.packet);
				switch(p.messageId())
				{
					case SM_LoginResponse:
					{
						int slot = p.read<Int32>("Slot");
						m_LocalClientSlot = slot;
					} break;
					
					case SM_ClientList:
					{
						while(p.available(sizeof(Int32)))
						{
							int slot = p.read<Int32>("Slot");
							std::string name = p.readString("Name");
							clientInfoBySlot(slot)->activate(name);
						}
					} break;
					
					case SM_ClientConnected:
					{
						int slot = p.read<Int32>("Slot");
						std::string name = p.readString("Name");
						clientInfoBySlot(slot)->activate(name);
					} break;
					
					case SM_ClientDisconnected:
					{
						int slot = p.read<Int32>("Slot");
						ClientInfo* ci = clientInfoBySlot(slot);
						ci->reset();
					} break;
					
					case SM_ObjectUpdate:
					{
						Singleton<ClientObjectManager>()->readUpdate(&p);
					} break;
					
					default:
					{
						// Wenn sonst nix passt wirds ans Script geleitet
						m_PacketCallback.prepareCall();
// 						p.pushHandle();
						m_PacketCallback.executeCall(0, false, true);
						
// 						Error("Got unknown network message");
					}
				}
				
				enet_packet_destroy(event.packet);
			} break;
			
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				Log("Server disconnected.\n");
				m_ServerPeer = NULL;
				// TODO: What now?
			} break;
			
			default:
			{
				Log("This should not be called O_o\n");
			}
		}
	}
}

bool Client::connect( const ENetAddress* address )
{
	if(m_ServerPeer)
	{
		Log("Already connected to a server.\n");
		return false;
	}
	
	m_ServerPeer = enet_host_connect(m_Host, address, ChannelCount, 0);
	if(m_ServerPeer == NULL)
	{
		Log("No available peers for initiating an connection.\n");
		return false;
	}
	
	// Wait 5 seconds to connect
	ENetEvent event;
	if((enet_host_service(m_Host, &event, ConnectTimeout) > 0) && (event.type == ENET_EVENT_TYPE_CONNECT))
	{
		Log("Connected!\n");
		
		// Send login request
		OPacket p(StuffChannel, CM_LoginRequest, 0);
		p.write("Name", std::string("Wiff"));
		sendPacket(&p);
		
		return true;
	}
	else
	{
		// Either the 5 seconds are up or a disconnect event was received.
		// Reset the peer in the event the 5 seconds had run out without any significant event.
		enet_peer_reset(m_ServerPeer);
		Log("Connection attempt failed.\n");
		return false;
	}
}

void Client::disconnect()
{
	if(!m_ServerPeer)
		return;
	
	enet_peer_disconnect(m_ServerPeer, 0);
	
	// Allow up to 3 seconds for the disconnect
	// to succeed and drop any packets received packets.
	bool disconnected = false;
	ENetEvent event;
	while(enet_host_service(m_Host, &event, DisconnectTimeout) > 0)
	{
		switch(event.type)
		{
			case ENET_EVENT_TYPE_RECEIVE:
			{
				enet_packet_destroy(event.packet);
			} break;

			case ENET_EVENT_TYPE_DISCONNECT:
			{
				disconnected = true;
			} break;
			
			default:
			{
			}
		}
		
		if(disconnected)
			break;
	}
	
	if(disconnected)
		Log("Disconnection succeeded.\n");
	else
		Log("Disconnection failed.\n");
	
	enet_peer_reset(m_ServerPeer);
	m_ServerPeer = NULL;
}

void Client::sendPacket( OPacket* packet )
{
	if(!m_ServerPeer)
	{
		Log("Can't send packet: Not connected to a server.\n");
		return;
	}
	
	enet_peer_send(m_ServerPeer, packet->channel(), packet->enetPacket());
	// enet_host_flush(m_Host);
}

SQInteger fn_SendPacket( HSQUIRRELVM vm ) // packet
{
	OPacket* p = OPacket::GetHandle(2);
	
	Singleton<Client>()->sendPacket(p);
	return 0;
}
RegisterSqFunction(SendPacket, fn_SendPacket, 2, ".p");
