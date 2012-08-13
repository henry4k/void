#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <vector>
#include <string>
#include <enet/enet.h>
#include <void/Packet.h>

static const int ConnectTimeout = 5000;
static const int DisconnectTimeout = 3000;
static const int NetworkServiceTimeout = 1000; // D-d-debug!
static const int ClientHardLimit = 32;
static const int InvalidSlot = -1;

enum Channel
{
	StuffChannel,
	ChannelCount
};

enum ClientMessage
{
	CM_Count
};

enum ServerMessage
{
	SM_ObjectUpdate,
	SM_Count
};

/** ObjectUpdate
	ui StartTick       - Es wird mindestens dieser Tick benötigt (weil das Update die Differenz zwischen start und destination bearbeitet)
	ui DestinationTick - Der Client hat nach anwenden des Updates diesen Tick
	
	- ObjectCommands
	[
		<ID> [<CommandName> <CommandData>]
		<ID=InvalidId> - Signalisiert das Ende der Liste
	]
**/

/** ObjectCommands
	Create <TypeId> - Objekt wird erstellt und der Script-Konstruktor aufgerufen
	Update <VarName> <SerializedValue>
	Delete
	End
**/
enum ObjectCommand
{
	OC_Create = 0,
	OC_Update,
	OC_Delete,
	OC_End,
	OC_Count
};

class ClientInfo
{
	public:
		ClientInfo();
		void activate( int slot );
		void reset();
		
		bool isActive() const;
		int slot() const;
		const char* name() const;
		
	private:
		bool m_Active;
		int m_Slot;
		std::string m_Name;
};


class Network
{
	public:
		Network();
		virtual ~Network();

		virtual void service() = 0;

		int clientCount() const;
		int nextClientSlot( int i );

		const ClientInfo* clientInfoBySlot( int slot ) const;

	protected:
		ClientInfo* clientInfoBySlot( int slot );
		
		ENetHost* m_Host;
		std::vector<ClientInfo> m_ClientInfos;
};


#endif