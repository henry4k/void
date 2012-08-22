#include <void/Common.h>
#include <void/Network.h>


/// ---- ClientInfo ----

static const ClientInfo InvalidClientInfo;


ClientInfo::ClientInfo( int slot ) :
	m_Slot(slot)
{
	reset();
}

void ClientInfo::activate( std::string name )
{
	m_Active = true;
	m_Name = name;
	
	Log("Activated #%d alias \"%s\"", m_Slot, m_Name.c_str());
}

void ClientInfo::reset()
{
	m_Active = false;
	m_Name = "InvalidClient";
	
	Log("Reset #%d", m_Slot);
}

bool ClientInfo::isActive() const
{
	return m_Active;
}

int ClientInfo::slot() const
{
	return m_Slot;
}

const char* ClientInfo::name() const
{
	return m_Name.c_str();
}




/// ---- Network ----

Network::Network() :
	m_Host(NULL)
{
	if(enet_initialize() != 0)
		Error("An error occurred while initializing network.");
	
	m_ClientInfos.resize(ClientHardLimit);
	for(int i = 0; i < m_ClientInfos.size(); ++i)
	{
		m_ClientInfos[i] = ClientInfo(i);
	}
}

Network::~Network()
{
	if(m_Host)
		enet_host_destroy(m_Host);
	enet_deinitialize();
}

int Network::clientCount() const
{
	return m_ClientInfos.size();
}

const ClientInfo* Network::clientInfoBySlot( int slot ) const
{
	if( InsideArray(slot,m_ClientInfos.size()) )
		return &m_ClientInfos[slot];
	else
		return &InvalidClientInfo;
}

ClientInfo* Network::clientInfoBySlot( int slot )
{
	assert(tools4k::InsideArray(slot, m_ClientInfos.size()));
	return &m_ClientInfos[slot];
}

int Network::nextClientSlot( int i )
{
	i = (i == InvalidSlot)?(0):(i+1);
	for(; i < m_ClientInfos.size(); i++)
	{
		if(m_ClientInfos[i].isActive())
			return i;
	}
	return InvalidSlot;
}

SQInteger Network::fn_SetPacketCallback( HSQUIRRELVM vm ) // env, fun
{
	HSQOBJECT env;
	HSQOBJECT fun;
	sq_getstackobj(vm, 2, &env);
	sq_getstackobj(vm, 3, &fun);
	Singleton<Network>()->m_PacketCallback = SquirrelFunction(vm, env, fun);
	return 0;
}
RegisterSqFunction(SetPacketCallback, Network::fn_SetPacketCallback, 3, "...");