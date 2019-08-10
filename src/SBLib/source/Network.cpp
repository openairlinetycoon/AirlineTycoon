#include "stdafx.h"
#include "..\include\sblib.h"
#include "..\include\network.h"

SBNetwork::SBNetwork(bool, GUID)
    : m_InSession(false)
    , m_Host(NULL)
{
    TEAKRAND rand;
    rand.SRandTime();
    m_LocalID = rand.Rand();
    m_Connections.Add("Unknown");
    m_Sessions.Add("TestSession");
}

long SBNetwork::GetMessageCount()
{
    ENetEvent event;

    if (!m_Host)
        return 0;

    /* We'll call this regularly so no need to block. */
    while (enet_host_service (m_Host, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            printf ("A new client connected from %x:%u.\n", 
                    event.peer -> address.host,
                    event.peer -> address.port);
            /* Store any relevant client information here. */
            SBNetworkPlayer player;
            player.ID = event.data;
            player.peer = event.peer;
            event.peer->data = &m_Players.Add(player);
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
                    event.packet -> dataLength,
                    event.packet -> data,
                    event.peer -> data,
                    event.channelID);
            m_Packets.Add(event.packet);
            break;
       
        case ENET_EVENT_TYPE_DISCONNECT:
            printf ("%s disconnected.\n", event.peer -> data);
            /* Reset the peer's client information. */
            event.peer -> data = NULL;
        }
    }

    return m_Packets.GetNumberOfElements();
}

// Connect to a non-IP medium
bool SBNetwork::Connect(SBStr medium)
{
    // Unsupported
    return false;
}

bool SBNetwork::Connect(SBStr medium, char* host)
{
    return enet_initialize() == 0;
}

void SBNetwork::DisConnect()
{
    enet_host_destroy(m_Host);
    enet_deinitialize();
    m_Host = NULL;
}

bool SBNetwork::CreateSession(SBStr, SBNetworkCreation*)
{
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 0xA112;
    m_Host = enet_host_create(&address, 8, 2, 0, 0);
    m_InSession = m_Host != NULL;
    return true;
}

void SBNetwork::CloseSession()
{
    m_InSession = false;
}

unsigned long SBNetwork::GetLocalPlayerID()
{
    return m_LocalID;
}

SBList<SBStr>* SBNetwork::GetConnectionList()
{
    return &m_Connections;
}

SBList<SBStr>* SBNetwork::GetSessionListAsync()
{
    return &m_Sessions;
}

bool SBNetwork::StartGetSessionListAsync()
{
    return true;
}

GUID* SBNetwork::GetProviderGuid(char*)
{
    return (GUID*)&DPSPGUID_TCPIP;
}

bool SBNetwork::IsEnumSessionFinished()
{
    return !m_InSession;
}

bool SBNetwork::IsInSession()
{
    return m_InSession;
}

bool SBNetwork::Send(BUFFER<UBYTE>& buffer, unsigned long length, unsigned long peerID, bool compression)
{
    ENetPacket * packet = enet_packet_create (buffer, length, ENET_PACKET_FLAG_RELIABLE);

    if (peerID)
    {
        for (m_Players.GetFirst(); !m_Players.IsLast(); m_Players.GetNext())
            if (m_Players.GetLastAccessed().ID == peerID)
                enet_peer_send (m_Players.GetLastAccessed().peer, 0, packet);

        if (m_Players.IsLast())
            return false;
    }
    else
    {
        enet_host_broadcast (m_Host, 0, packet);
    }
    enet_host_flush (m_Host);
    return true;
}

bool SBNetwork::Receive(UBYTE** buffer, unsigned long& size)
{
    m_Packets.GetFirst();
    if (m_Packets.IsLast())
        return false;

    ENetPacket* packet = m_Packets.GetLastAccessed();
    size = packet->dataLength;
    *buffer = new UBYTE[size];
    memcpy(*buffer, packet->data, size);

    /* Clean up the packet now that we're done using it. */
    enet_packet_destroy(packet);
    m_Packets.RemoveLastAccessed();
    return true;
}

bool SBNetwork::JoinSession(SBStr session, SBStr nickname)
{
    m_Host = enet_host_create(NULL, 8, 2, 0, 0);
    m_InSession = m_Host != NULL;
    if (!m_InSession)
        return false;

    /* Connect to some.server.net:1234. */
    ENetAddress address;
    enet_address_set_host (&address, "localhost");
    address.port = 0xA112;
    /* Initiate the connection, allocating the two channels 0 and 1. */
    enet_host_connect (m_Host, &address, 2, 0);
    return true;
}

SBList<SBNetworkPlayer>* SBNetwork::GetAllPlayers()
{
    return &m_Players;
}
