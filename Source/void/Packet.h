#ifndef __PACKET_H__
#define __PACKET_H__

#include <vector>
#include <enet/enet.h>

#include <void/Serializer.h>

class Squirrel;


class OPacket : public Serializer
{
	public:
		OPacket( int channel, int messageId, int flags );
		virtual ~OPacket();
		
		int length() const;
		
		int channel() const;
		
		ENetPacket* enetPacket();
		
		void pushToVm( Squirrel* squirrel );
		static OPacket* GetFromVm( Squirrel* squirrel, int index );
		
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
		
		int messageId() const;
		
		int channel() const;
		
		void pushToVm( Squirrel* squirrel );
		static IPacket* GetFromVm( Squirrel* squirrel, int index );
		
	private:
		void setPos( int pos ); // TODO: Remove this
		
		int onRead( char* target, int length );
		
		ENetPacket* m_Packet;
		int m_Channel;
		int m_Pos;
		int m_MessageId;
};

#endif