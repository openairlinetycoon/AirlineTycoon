#include "stdafx.h"
#include "sblib.h"
#include "network.h"

SBNetwork::SBNetwork(bool)
    : mState(SBNETWORK_SESSION_FINISHED)
    , mHost(NULL)
    , mMaster(NULL)
    , mSearchTime(0)
    , mServer()
    , mSocket()
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

SLONG SBNetwork::GetMessageCount()
{
    ENetEvent event;

    ENetAddress address;
    ENetBuffer buf;
    if (mState == SBNETWORK_SESSION_MASTER)
	{
		SLONG clientID;
		buf.data = &clientID;
		buf.dataLength = sizeof(SLONG);
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
                if(mSessionInfo.GetNumberOfElements() > 0){
                /* Check if we already know about the session */
                    for (mSessionInfo.GetFirst(); !mSessionInfo.IsLast() &&
                        mSessionInfo.GetLastAccessed().hostID != info.hostID; mSessionInfo.GetNext());
                }

                if (mSessionInfo.GetNumberOfElements() == 0 || mSessionInfo.IsLast())
                {
				    info.address.host = address.host;
				    mSessionInfo.Add(info);
				    mSessions.Add(SBStr(info.sessionName));
                }
			}
        }

        /* Automatically refresh every 5 seconds */
        if (enet_time_get() - mSearchTime > 5000)
            StartGetSessionListAsync();
	}

    if (!mHost)
        return 0;

    /* We'll call this regularly so no need to block. */
    while (enet_host_service (mHost, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            /* Store any relevant client information here. */
            if (event.data != 0)
            {
                SBNetworkPlayer player;
                player.ID = event.data;
                player.peer = event.peer;
                player.peer->data = &mPlayers.Add(player);

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
                player.peer->data = &mPlayers.Add(player);
            }
            else
            {
                mPackets.Add(event.packet);
            }
            break;
       
        case ENET_EVENT_TYPE_DISCONNECT:
            /* Delete the player and inform the multiplayer code */
            if (event.peer->data)
            {
                SBNetworkPlayer* player = (SBNetworkPlayer*)event.peer->data;
                DPPacket dp;
                dp.messageType = DPSYS_DESTROYPLAYERORGROUP;
                dp.playerType = DPPLAYERTYPE_PLAYER;
                dp.dpId = player->ID;
                ENetPacket* packet = enet_packet_create (&dp, sizeof(DPPacket), ENET_PACKET_FLAG_RELIABLE);
                mPackets.Add(packet);

                if(mPlayers.GetNumberOfElements() > 0){
                    for (mPlayers.GetFirst(); !mPlayers.IsLast(); mPlayers.GetNext())
                    {
                        if (mPlayers.GetLastAccessed().ID == player->ID)
                        {
                            mPlayers.RemoveLastAccessed();
                            break;
                        }
                    }
                }
            }

            /* Handle host migration and inform the multiplayer code */
            if (event.peer == mMaster)
            {
                SBNetworkPlayer* master = &mPlayers.GetFirst();
                for (mPlayers.GetNext(); !mPlayers.IsLast(); mPlayers.GetNext())
                {
                    if (mPlayers.GetLastAccessed().ID < master->ID)
                        master = &mPlayers.GetLastAccessed();
                }

                if (master->ID == mLocalID)
                {
                    DPPacket dp;
                    dp.messageType = DPSYS_HOST;
                    dp.playerType = DPPLAYERTYPE_PLAYER;
                    dp.dpId = master->ID;
                    ENetPacket* packet = enet_packet_create (&dp, sizeof(DPPacket), ENET_PACKET_FLAG_RELIABLE);
                    mPackets.Add(packet);
                    mState = SBNETWORK_SESSION_MASTER;
                }

                mMaster = master->peer;
            }

            /* Reset the peer's client information. */
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

    mSocket = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
    if (mSocket == ENET_SOCKET_NULL)
        return false;

    enet_socket_set_option(mSocket, ENET_SOCKOPT_REUSEADDR, 1);
    enet_socket_set_option(mSocket, ENET_SOCKOPT_NONBLOCK, 1);
    if (enet_address_set_host_ip(&mServer, host) < 0)
    {
        enet_socket_set_option(mSocket, ENET_SOCKOPT_BROADCAST, 1);
        mServer.host = ENET_HOST_BROADCAST;
    }
    mServer.port = 0xA112;

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 0xA112;
    enet_socket_bind(mSocket, &address);

    address.host = ENET_HOST_ANY;
    address.port = 0xA113;
    mHost = enet_host_create(&address, 8, 2, 0, 0);
    return mHost != NULL;
}

void SBNetwork::DisConnect()
{
    CloseSession();
    enet_host_destroy(mHost);
    enet_deinitialize();
    mHost = NULL;
    mSessions.Clear();
    mSessionInfo.Clear();
}

bool SBNetwork::CreateSession(SBStr name, SBNetworkCreation* create)
{
    SBSessionInfo info;
    strcpy(info.sessionName, create->sessionName.c_str());
    info.hostID = mLocalID;
    info.address.host = ENET_HOST_ANY;
    info.address.port = 0xA113;
    mSessionInfo.Clear();
    mSessionInfo.Add(info);
    mState = SBNETWORK_SESSION_MASTER;
    mSearchTime = enet_time_get();
    return true;
}

void SBNetwork::CloseSession()
{
    mState = SBNETWORK_SESSION_FINISHED;
}

ULONG SBNetwork::GetLocalPlayerID()
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
    ENetBuffer buf;
    buf.data = &mLocalID;
    buf.dataLength = sizeof(mLocalID);
    enet_socket_send(mSocket, &mServer, &buf, 1);
    mState = SBNETWORK_SESSION_SEARCHING;
    mSearchTime = enet_time_get();
    return true;
}

SLONG SBNetwork::GetProviderID(char*)
{
    return NET_MEDIUM_TCPIP;
}

bool SBNetwork::IsEnumSessionFinished()
{
    return mState == SBNETWORK_SESSION_FINISHED;
}

bool SBNetwork::IsInSession()
{
    return mState == SBNETWORK_SESSION_MASTER || mState == SBNETWORK_SESSION_CLIENT;
}

bool SBNetwork::Send(BUFFER<UBYTE>& buffer, ULONG length, ULONG peerID, bool compression)
{
    ENetPacket* packet = enet_packet_create (buffer, length, ENET_PACKET_FLAG_RELIABLE);

    if (peerID)
    {
        for (mPlayers.GetFirst(); !mPlayers.IsLast(); mPlayers.GetNext())
            if (mPlayers.GetLastAccessed().ID == peerID && mPlayers.GetLastAccessed().peer != NULL)
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

bool SBNetwork::Receive(UBYTE** buffer, ULONG& size)
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
    ENetEvent event;
    SBNetworkPlayer player;
    player.ID = info->hostID;
    player.peer = enet_host_connect (mHost, &info->address, 2, mLocalID);
    player.peer->data = &mPlayers.Add(player);
    enet_peer_timeout(player.peer, 100000,0,100000000);
    mMaster = player.peer;
    mState = SBNETWORK_SESSION_CLIENT;
    return enet_host_service (mHost, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT;
}

SBList<SBNetworkPlayer>* SBNetwork::GetAllPlayers()
{
    return &mPlayers;
}
