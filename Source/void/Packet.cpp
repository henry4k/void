#include <void/Common.h>
#include <void/Packet.h>


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



/// -------------- Squirrel Bindings ------------------ ///

#include <void/Squirrel.h>

SQInteger ReleaseOPacket( SQUserPointer up, SQInteger size )
{
	delete (OPacket*)up;
	return 1;
}

void OPacket::pushHandle()
{
	HSQUIRRELVM vm = Singleton<Squirrel>()->vm();
	sq_pushuserpointer(vm, this);
	sq_setreleasehook(vm, -1, ReleaseOPacket);
}

OPacket* OPacket::GetHandle( int index )
{
	OPacket* p = NULL;
	sq_getuserpointer(Singleton<Squirrel>()->vm(), index, (SQUserPointer*)&p);
	return p;
}

SQInteger fn_CreateOPacket( HSQUIRRELVM vm ) // channel, messageId
{
	SQInteger channel, messageId;
	sq_getinteger(vm, 2, &channel);
	sq_getinteger(vm, 3, &messageId);
	
	OPacket* p = new OPacket(channel, messageId, 0);
	p->pushHandle();
	return 1;
}
RegisterSqFunction(CreateOPacket, fn_CreateOPacket, 3, ".ii");

/**
	bB char
	hH short
	iI int
	f  float
	s  string
**/
SQInteger fn_PacketWrite( HSQUIRRELVM vm ) // opacket, name, type, value
{
	OPacket* p = OPacket::GetHandle(2);
	
	const SQChar* name;
	sq_getstring(vm, 3, &name);
	
	SQInteger type;
	sq_getinteger(vm, 4, &type);
	
	union
	{
		SQBool b;
		SQInteger i;
		SQFloat f;
		const SQChar* s;
	} data;
	
	switch(sq_gettype(vm, 5))
	{
		case OT_BOOL:    sq_getbool(vm, 5, &data.b);    break;
		case OT_INTEGER: sq_getinteger(vm, 5, &data.i); break;
		case OT_FLOAT:   sq_getfloat(vm, 5, &data.f);   break;
		case OT_STRING:  sq_getstring(vm, 5, &data.s);  break;
		default:
		{
			Log("Unsupported input type\n");
			return 0;
		}
	}
	
	switch(type)
	{
		case 'b': p->write<Int8>(name, data.i); break;
		case 'B': p->write<UInt8>(name, data.i); break;
		
		case 'h': p->write<Int16>(name, data.i); break;
		case 'H': p->write<UInt16>(name, data.i); break;
		
		case 'i': p->write<Int32>(name, data.i); break;
		case 'I': p->write<UInt32>(name, data.i); break;
		
		case 'f': p->write<float>(name, data.f); break;
		
		case 's': p->write(name, std::string(data.s)); break;
		
		default:
		{
			Log("Unsupported output type\n");
			return 0;
		}
	}
	
	return 0;
}
RegisterSqFunction(PacketWrite, fn_PacketWrite, 5, ".psi.");




SQInteger ReleaseIPacket( SQUserPointer up, SQInteger size )
{
	delete (IPacket*)up;
	return 1;
}

void IPacket::pushHandle()
{
	HSQUIRRELVM vm = Singleton<Squirrel>()->vm();
	sq_pushuserpointer(vm, this);
	sq_setreleasehook(vm, -1, ReleaseIPacket);
}

IPacket* IPacket::GetHandle( int index )
{
	IPacket* p = NULL;
	sq_getuserpointer(Singleton<Squirrel>()->vm(), index, (SQUserPointer*)&p);
	return p;
}

/**
	bB char
	hH short
	iI int
	f  float
	s  string
**/
SQInteger fn_PacketRead( HSQUIRRELVM vm ) // ipacket, name, type
{
	IPacket* p = IPacket::GetHandle(2);
	
	const SQChar* name;
	sq_getstring(vm, 3, &name);
	
	SQInteger type;
	sq_getinteger(vm, 4, &type);
	
	switch(type)
	{
		case 'b': sq_pushinteger(vm, p->read<Int8>(name)); break;
		case 'B': sq_pushinteger(vm, p->read<UInt8>(name)); break;
		
		case 'h': sq_pushinteger(vm, p->read<Int16>(name)); break;
		case 'H': sq_pushinteger(vm, p->read<UInt16>(name)); break;
		
		case 'i': sq_pushinteger(vm, p->read<Int32>(name)); break;
		case 'I': sq_pushinteger(vm, p->read<UInt32>(name)); break;
		
		case 'f': sq_pushfloat(vm, p->read<float>(name)); break;
		
		case 's':
		{
			std::string s = p->readString(name);
			sq_pushstring(vm, s.c_str(), -1);
		} break;
		
		default:
		{
			Log("Unsupported output type\n");
			return 0;
		}
	}
	
	return 1;
}
RegisterSqFunction(PacketRead, fn_PacketRead, 4, ".psi");