#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <vector>
#include <string>
#include <enet/enet.h>

#include <void/Squirrel.h>
#include <void/Packet.h>
#include <void/Singleton.h>

static const int ConnectTimeout = 5000;
static const int DisconnectTimeout = 3000;
static const int NetworkServiceTimeout = 1000; // D-d-debug!
static const int ClientHardLimit = 8;
static const int InvalidSlot = -1;

enum Channel
{
	StuffChannel,
	ChannelCount
};

enum ClientMessage
{
	/**
	 * string Name
	 */
	CM_LoginRequest,
	CM_Count
};

enum ServerMessage
{
	/**
	 * ui StartTick       - Client needs at least this tick (cause the update describse the difference between two ticks)
	 * ui DestinationTick - Updates the client to that tick.
	 * 
	 * - ObjectCommands
	 * [
	 * 	<ID> [<CommandName> <CommandData>]
	 * 	<ID=InvalidId> - Signalizes the end of the list.
	 * ]
	 */
	SM_ObjectUpdate,
	
	/**
	 * int Slot
	 */
	SM_LoginResponse,
	
	/**
	 * [ int Slot, string Name ], ...
	 */
	SM_ClientList,
	
	/**
	 * int Slot, string Name
	 */
	SM_ClientConnected,
	
	/**
	 * int Slot
	 */
	SM_ClientDisconnected,
	
	SM_Count
};

enum ObjectCommand
{
	/**
	 * Create <TypeId> - Allocates the object and calls its script constructor.
	 */
	OC_Create,

	/**
	 * <VarIndex> <SerializedValue>
	 */
	OC_Update,

	/**
	 * - Removes this object.
	 */
	OC_Delete,

	/**
	 * Signalizes end of the command list.
	 */
	OC_End,

	OC_Count
};

class ClientInfo
{
	public:
		ClientInfo() {}
		ClientInfo( int slot );

		/**
		 * Activates the client and assings it to a slot. // TODO: Warum ist das im Common Bereich .. der Client kann doch garnicht andere Clients aktivieren?
		 */
		void activate( std::string name );

		/**
		 * Resets this slot, so another client may use it.
		 */
		void reset();
		
		/**
		 * Inactive clients don't receive any network messages nor take part in the game.
		 * // The server awaits an authentification message and forcefully disconnects them after a timelimit.
		 */
		bool isActive() const;

		/**
		 * Returns the slot index
		 * or InvalidSlot when this slot is not yet activated.
		 */
		int slot() const;

		/**
		 * Unique client name. (i.e. player name)
		 * No two clients on the server can have the same names.
		 */
		const char* name() const;
		
	private:
		bool m_Active;
		int m_Slot;
		std::string m_Name;
};


class Network : public SingletonClass<Network>
{
	public:
		Network();
		virtual ~Network();
		
		/**
		 * Should be called regulary.
		 * Receives new messages and pushes the outgoing message queue to the ether.
		 */
		virtual void service() = 0;
		
		/**
		 * Amount of slots. // TODO: Vielleicht so umbauen das es die Anzahl der aktiven Clients zurückgibt.
		 */
		int clientCount() const;
		
		// int maxClients() const;

		/**
		 * Use this to iterate the active client slots.
		 * Start value must be InvalidSlot.
		 * Returns the next active slot index
		 * or InvalidSlot when you've reached the end of the list.
		 */
		int nextClientSlot( int i );
		
		/**
		 * Returns the ClientInfo structure.
		 * The const version will return InvalidClientInfo on out of bound scenarios. // TODO: NULL währe imho besser.
		 */
		const ClientInfo* clientInfoBySlot( int slot ) const;
		
		static SQInteger fn_SetPacketCallback( HSQUIRRELVM vm );

	protected:
		ClientInfo* clientInfoBySlot( int slot );
		
		ENetHost* m_Host;
		std::vector<ClientInfo> m_ClientInfos;
		SquirrelFunction m_PacketCallback;
};


#endif
