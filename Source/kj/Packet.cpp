#include <kj/Common.h>
#include <kj/Packet.h>


/// ---- OPacket ----

OPacket::OPacket( int channel, int messageId, int flags ) :
	m_Channel(channel),
	m_Flags(flags),
	m_Packet(NULL)
{
	write("MessageId", UInt8(messageId));
}

OPacket::~OPacket()
{
}

int OPacket::length() const
{
	return m_Buffer.size();
}

int OPacket::channel() const
{
	return m_Channel;
}

ENetPacket* OPacket::enetPacket()
{
	if(m_Packet)
		return m_Packet;
	
	m_Packet = enet_packet_create(m_Buffer.data(), m_Buffer.size(), m_Flags);
	
	return m_Packet;
}

int OPacket::onWrite( const char* source, int length )
{
	m_Buffer.insert(m_Buffer.end(), source, source+length);
	return length;
}


/// ---- IPacket ----

IPacket::IPacket( int channel, ENetPacket* packet ) :
	m_Packet(packet),
	m_Channel(channel),
	m_Pos(0)
{
	m_MessageId = read<UInt8>("MessageId");
}

int IPacket::length() const
{
	return m_Packet->dataLength;
}

bool IPacket::end() const
{
	return (m_Pos >= m_Packet->dataLength);
}

bool IPacket::available( int bytes ) const
{
	return ( (m_Packet->dataLength - m_Pos) >= bytes );
}

int IPacket::pos() const
{
	return m_Pos;
}

void IPacket::setPos( int pos )
{
	m_Pos = pos;
}

int IPacket::messageId() const
{
	return m_MessageId;
}

int IPacket::channel() const
{
	return m_Channel;
}

int IPacket::onRead( char* target, int length )
{
	if( !available(length) )
	{
		m_Pos += length;
		return 0;
	}

	for(int i = 0; i < length; i++)
		target[i] = m_Packet->data[m_Pos+i];

	m_Pos += length;

	return length;
}
