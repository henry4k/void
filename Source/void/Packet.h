#ifndef __PACKET_H__
#define __PACKET_H__

#include <vector>
#include <enet/enet.h>

#include <void/Serializer.h>

class Squirrel;


class OPacket : public Serializer
{
	public:
		/**
		 * @see channel()
		 * @see ClientMessage
		 * @see ServerMessage
		 * ...
		 */
		OPacket( int channel, int messageId, int flags );
		virtual ~OPacket();
		
		int length() const;
		
		/**
		 * The enet channel on that this packet should be send.
		 * @see Channel 
		 */
		int channel() const;
		
		/**
		 * Finalizes the packet buffer and creates the enetPacket.
		 * May be called multiple times.
		 */
		ENetPacket* enetPacket();
		
		/**
		 * Pushes this a user pointer in the script vm.
		 */
		void pushHandle();
		
		/**
		 * Retrieves the user poiter at the specified position from the vm.
		 * Note that this is no strong reference!
		 */
		static OPacket* GetHandle( int index );
		
	private:
		int onWrite( const char* source, int length );
		std::vector<char> m_Buffer;
		
		int m_Channel;
		int m_Flags;
		
		ENetPacket* m_Packet;
};


class IPacket : public Serializer
{
	public:
		IPacket( int channel, ENetPacket* packet );
		
		int length() const;
		bool end() const;
		bool available( int bytes ) const;
		
		int pos() const;
		
		/**
		 * @see ClientMessage
		 * @see ServerMessage
		 */
		int messageId() const;
		
		/**
		 * The enet channel on which this packet was received.
		 * @see Channel
		 */
		int channel() const;
		
		/**
		 * Pushes this a user pointer in the script vm.
		 */
		void pushHandle();
		
		/**
		 * Retrieves the user poiter at the specified position from the vm.
		 * Note that this is no strong reference!
		 */
		static IPacket* GetHandle( int index );
		
	private:
		void setPos( int pos ); // TODO: Remove this
		
		int onRead( char* target, int length );
		
		ENetPacket* m_Packet;
		int m_Channel;
		int m_Pos;
		int m_MessageId;
};

#endif
