#include "stdafx.h"
#include "..\include\sblib.h"
#include "..\include\network.h"

SBNetwork::SBNetwork(bool, GUID)
    : mState(SBNETWORK_SESSION_FINISHED)
    , mHost(NULL)
{
    TEAKRAND rand;
    rand.SRandTime();
    mLocalID = rand.Rand();

    SBNetworkPlayer player;
    player.ID = mLocalID;
    player.peer = NULL;
    mPlayers.Add(player);

    mConnections.Add("enet");
}

long SBNetwork::GetMessageCount()
{
    ENetEvent event;

    ENetAddress address;
    ENetBuffer buf;
    if (mState == SBNETWORK_SESSION_MASTER)
	{
		long clientID;
		buf.data = &clientID;
		buf.dataLength = sizeof(long);
		if (enet_socket_receive(mSocket, &address, &buf, 1) > 0)
		{
			if (clientID != mLocalID)
			{
				buf.data = &mSessionInfo.GetFirst();
				buf.dataLength = sizeof(SBSessionInfo);
				enet_socket_send(mSocket, &address, &buf, 1);
			}
		}
	}
	else if (mState == SBNETWORK_SESSION_SEARCHING)
	{
		SBSessionInfo info;
		buf.data = &info;
		buf.dataLength = sizeof(SBSessionInfo);
		if(enet_socket_receive(mSocket, &address, &buf, 1) > 0)
		{
			if (info.hostID != mLocalID)
			{
				info.address.host = address.host;
				mSessionInfo.Add(info);
				mSessions.Add(SBStr(info.sessionName));
			}
		}
	}

    if (!mHost)
        return 0;

    /* We'll call this regularly so no need to block. */
    while (enet_host_service (mHost, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            printf ("A new client connected from %x:%u.\n", 
                    event.peer -> address.host,
                    event.peer -> address.port);
            /* Store any relevant client information here. */
            if (event.data != 0)
            {
                SBNetworkPlayer player;
                player.ID = event.data;
                player.peer = event.peer;
                mPlayers.Add(player);

                if (mState == SBNETWORK_SESSION_MASTER)
                {
                    /* Broadcast the address of this peer to all other peers */
                    SBNetworkPeer peer;
                    peer.ID = event.data;
                    peer.address = event.peer->address;
                    ENetPacket* packet = enet_packet_create (&peer, sizeof(SBNetworkPeer), ENET_PACKET_FLAG_RELIABLE);
                    enet_host_broadcast (mHost, 1, packet);
                }
            }
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
                    event.packet -> dataLength,
                    event.packet -> data,
                    event.peer -> data,
                    event.channelID);
            if (event.channelID == 1)
            {
                if (event.packet->dataLength != sizeof(SBNetworkPeer))
                    break;

                SBNetworkPeer* peer = (SBNetworkPeer*)event.packet->data;
                if (peer->ID == mLocalID)
                    break;

                /* Initiate the connection, allocating the two channels 0 and 1. */
                SBNetworkPlayer player;
                player.ID = peer->ID;
                player.peer = enet_host_connect (mHost, &peer->address, 2, mLocalID);
                mPlayers.Add(player);
            }
            else
            {
                mPackets.Add(event.packet);
            }
            break;
       
        case ENET_EVENT_TYPE_DISCONNECT:
            printf ("%s disconnected.\n", event.peer -> data);
            /* Reset the peer's client information. */
            //mPlayers.Remove((SBNetworkPlayer*)event.peer->data);
            event.peer->data = NULL;
        }
    }

    return mPackets.GetNumberOfElements();
}

// Connect to a non-IP medium
bool SBNetwork::Connect(SBStr medium)
{
    // Unsupported
    return false;
}

bool SBNetwork::Connect(SBStr medium, char* host)
{
    if (enet_initialize() != 0)
        return false;

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 0xA112;

    mSocket = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
    enet_socket_set_option(mSocket, ENET_SOCKOPT_REUSEADDR, 1);
	enet_socket_set_option(mSocket, ENET_SOCKOPT_BROADCAST, 1);
    enet_socket_set_option(mSocket, ENET_SOCKOPT_NONBLOCK, 1);
    enet_socket_bind(mSocket, &address);
    if (mSocket == NULL)
        return false;

    address.host = ENET_HOST_ANY;
    address.port = 0xA113;
    mHost = enet_host_create(&address, 8, 2, 0, 0);
    return mHost != NULL;
}

void SBNetwork::DisConnect()
{
    enet_host_destroy(mHost);
    enet_deinitialize();
    mHost = NULL;
}

bool SBNetwork::CreateSession(SBStr name, SBNetworkCreation* create)
{
    SBSessionInfo info;
    strcpy(info.sessionName, create->sessionName);
    info.hostID = mLocalID;
    info.address.host = ENET_HOST_ANY;
    info.address.port = 0xA113;
    mSessionInfo.Add(info);
    mState = SBNETWORK_SESSION_MASTER;
    return true;
}

void SBNetwork::CloseSession()
{
    mState = SBNETWORK_SESSION_FINISHED;
}

unsigned long SBNetwork::GetLocalPlayerID()
{
    return mLocalID;
}

SBList<SBStr>* SBNetwork::GetConnectionList()
{
    return &mConnections;
}

SBList<SBStr>* SBNetwork::GetSessionListAsync()
{
    return &mSessions;
}

bool SBNetwork::StartGetSessionListAsync()
{
    if (mState == SBNETWORK_SESSION_SEARCHING)
        return true;

    ENetBuffer buf;
    ENetAddress address;
    address.host = ENET_HOST_BROADCAST;
    address.port = 0xA112;
    buf.data = &mLocalID;
    buf.dataLength = sizeof(mLocalID);
    enet_socket_send(mSocket, &address, &buf, 1);
    mState = SBNETWORK_SESSION_SEARCHING;
    return true;
}

GUID* SBNetwork::GetProviderGuid(char*)
{
    return (GUID*)&DPSPGUID_TCPIP;
}

bool SBNetwork::IsEnumSessionFinished()
{
    return mState == SBNETWORK_SESSION_FINISHED;
}

bool SBNetwork::IsInSession()
{
    return mState == SBNETWORK_SESSION_MASTER || mState == SBNETWORK_SESSION_CLIENT;
}

bool SBNetwork::Send(BUFFER<UBYTE>& buffer, unsigned long length, unsigned long peerID, bool compression)
{
    ENetPacket* packet = enet_packet_create (buffer, length, ENET_PACKET_FLAG_RELIABLE);

    if (peerID)
    {
        for (mPlayers.GetFirst(); !mPlayers.IsLast(); mPlayers.GetNext())
            if (mPlayers.GetLastAccessed().ID == peerID)
                enet_peer_send (mPlayers.GetLastAccessed().peer, 0, packet);

        if (mPlayers.IsLast())
            return false;
    }
    else
    {
        enet_host_broadcast (mHost, 0, packet);
    }
    enet_host_flush (mHost);
    return true;
}

bool SBNetwork::Receive(UBYTE** buffer, unsigned long& size)
{
    mPackets.GetFirst();
    if (mPackets.IsLast())
        return false;

    ENetPacket* packet = mPackets.GetLastAccessed();
    size = packet->dataLength;
    *buffer = new UBYTE[size];
    memcpy(*buffer, packet->data, size);

    /* Clean up the packet now that we're done using it. */
    enet_packet_destroy(packet);
    mPackets.RemoveLastAccessed();
    return true;
}

bool SBNetwork::JoinSession(SBStr session, SBStr nickname)
{
    SBSessionInfo* info = NULL;
    for (mSessionInfo.GetFirst(); !mSessionInfo.IsLast(); mSessionInfo.GetNext())
    {
        if (session == mSessionInfo.GetLastAccessed().sessionName)
            info = &mSessionInfo.GetLastAccessed();
    }

    if (!info)
        return false;

    /* Initiate the connection, allocating the two channels 0 and 1. */
    SBNetworkPlayer player;
    player.ID = info->hostID;
    player.peer = enet_host_connect (mHost, &info->address, 2, mLocalID);
    mPlayers.Add(player);
    mState = SBNETWORK_SESSION_CLIENT;
    return true;
}

SBList<SBNetworkPlayer>* SBNetwork::GetAllPlayers()
{
    return &mPlayers;
}
