#include <kj/Common.h>
#include <kj/Network.h>


/// ---- ClientInfo ----

static const ClientInfo InvalidClientInfo;


ClientInfo::ClientInfo()
{
	reset();
}

void ClientInfo::activate( int slot )
{
	m_Active = true;
	m_Slot = slot;
}

void ClientInfo::reset()
{
	m_Active = false;
	m_Slot = InvalidSlot;
	m_Name = "InvalidClient";
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
		Error("An error occurred while initializing network.\n");
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
	if(m_ClientInfos.size() <= slot)
		m_ClientInfos.resize(slot+1);
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