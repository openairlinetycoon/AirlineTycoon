#include "stdafx.h"
#include "SbLib.h"
#include "network.h"
#include "BitStream.h"
#include "RAKNetNetwork.hpp"

using namespace RakNet;

void SerializePacket(ATPacket* p, BitStream* data) {
    data->Write(p->messageType);
    data->Write(p->peerID);
    data->Write(p->dataLength);

    for (int i = 0; i < p->dataLength; i++) {
        data->Write(p->data[i]);
    }
}

void DeserializePacket(unsigned char* data, unsigned int length, ATPacket* packet) {
    BitStream dataStream(data, length, false);
    dataStream.Read(packet->messageType);
    dataStream.Read(packet->peerID);
    dataStream.Read(packet->dataLength);

    packet->data = new UBYTE[packet->dataLength];

    for (int i = 0; i < packet->dataLength; i++) {
        dataStream.Read(packet->data[i]);
    }
}

RAKNetNetwork::RAKNetNetwork() : mRoomCallbacks(this) {
    TEAKRAND rand;
    rand.SRandTime();
	
    mLocalID = rand.Rand();

    isHostMigrating = false;

    mMaster = RakPeerInterface::GetInstance();

    RAKNetworkPlayer *player = new RAKNetworkPlayer();
    player->ID = mLocalID;
    player->peer = mMaster->GetMyGUID();
    mPlayers.Add(player);

    SDL_Log("SBNETWORK: RAKNetNetwork initialized! LocalId: %d", mLocalID);
}

SLONG RAKNetNetwork::GetMessageCount() {
    const unsigned int messageCountGame = mMaster->GetReceiveBufferSize();

    if (mState == SBNETWORK_SESSION_SEARCHING) {
        //We are searching for a session on the master server!

    }

    return messageCountGame;
}


bool RAKNetNetwork::Connect(const char* host) {
    if (mMaster == nullptr)
        return false;

    RakNet::SocketDescriptor sd(0, nullptr);
    if (mMaster->Startup(5, &sd, 1) == RAKNET_STARTED) {
        mMaster->SetMaximumIncomingConnections(4);
        mMaster->Connect(host, SERVER_PORT, nullptr, 0);

        return AwaitConnection(mMaster,false);
    }

    return false;
}

void RAKNetNetwork::Initialize() {

}

void RAKNetNetwork::Disconnect() {
    mSessions.Clear();
    mSessionInfo.Clear();
    
	CloseSession();
	mMaster->CloseConnection(mHost, true);
    mMaster->Shutdown(500);
	delete mMaster;
}

bool RAKNetNetwork::CreateSession(SBNetworkCreation* create) {
    RAKSessionInfo *info = new RAKSessionInfo();
    strcpy(info->sessionName, create->sessionName.c_str());
    info->hostID = mLocalID;
    info->address = mMaster->GetMyGUID();
    mSessionInfo.Clear();
    mSessionInfo.Add(info);
	
    mState = SBNETWORK_SESSION_MASTER;
    mSearchTime = SDL_GetTicks();

    switch (create->flags)
    {
    case SBCreationFlags::SBNETWORK_CREATE_TRY_NAT:
    default: //No need for a NAT server if the user chose it
        mMaster->Startup(4, &SocketDescriptor(SERVER_PORT, ""), 1);
        SDL_Log("CREATE SESSION: DIRECT. Our GUID: '%s' and our SystemAddress: '%s'", mMaster->GetMyGUID().ToString(), mMaster->GetSystemAddressFromGuid(mMaster->GetMyGUID()).ToString());
        mMaster->SetMaximumIncomingConnections(4);
        break;
    }

    return true;
}

void RAKNetNetwork::CloseSession() {
	SDL_Log("END SESSION");
	mMaster->Shutdown(100);
    mState = SBNETWORK_SESSION_FINISHED;
}

ULONG RAKNetNetwork::GetLocalPlayerID() {
    return mLocalID;
}


void RAKNetNetwork::RequestHostedClients(RakNetGUID serverGuid) {

}

bool RAKNetNetwork::IsSessionFinished() {
    return mState == SBNETWORK_SESSION_FINISHED;
}

bool RAKNetNetwork::IsInSession() {
    return mState == SBNETWORK_SESSION_MASTER || mState == SBNETWORK_SESSION_CLIENT;
}

bool RAKNetNetwork::Send(BUFFER<UBYTE>& buffer, ULONG length, ULONG peerID, bool compression) {
    ATPacket a{};
    a.messageType = SBNETWORK_MESSAGE;
    a.dataLength = length;
    a.data = buffer;
	a.peerID = peerID;

    BitStream data;
    SerializePacket(&a, &data);

    if (peerID) {
        SDL_Log("SEND PRIVATE: SBNETWORK_MESSAGE ID: - ID %x TO: %x", (a.data[3] << 24) | (a.data[2] << 16) | (a.data[1] << 8) | (a.data[0]), peerID);

    //    for (mPlayers.GetFirst(); !mPlayers.IsLast(); mPlayers.GetNext())
    //    {
    //        if (mPlayers.GetLastAccessed().ID == peerID){
				//SBNetworkPlayer *player = &mPlayers.GetLastAccessed();
    //        	
    //            int result = mMaster->Send(&data, HIGH_PRIORITY, RELIABLE_ORDERED, 0, static_cast<RAKNetworkPlayer*>(player)->peer, false);
				//SDL_Log("SEND PRIVATE: STATUS: %x", result);
    //        }
    //    }
    } else {
        SDL_Log("SEND: SBNETWORK_MESSAGE ID: - ID %x", (a.data[3] << 24) | (a.data[2] << 16) | (a.data[1] << 8) | (a.data[0]));
    }

	mMaster->Send(&data, HIGH_PRIORITY, RELIABLE_ORDERED, 0, NET_BROADCAST, true);
    

    return true;
}

#pragma optimize("", off)
bool RAKNetNetwork::Receive(UBYTE** buffer, ULONG& size) {
    if (isHostMigrating) { //Host migration:
        SDL_Log("MIGRATING HOST");
        SBNetworkPlayer* master = mPlayers.GetFirst();
        for (mPlayers.GetNext(); !mPlayers.IsLast(); mPlayers.GetNext())
        {
            if (mPlayers.GetLastAccessed()->ID < master->ID)
                master = mPlayers.GetLastAccessed();
        }

        if (master->ID == mLocalID)
        {
            DPPacket dp{};
            dp.messageType = DPSYS_HOST;
            dp.playerType = DPPLAYERTYPE_PLAYER;
            dp.dpId = master->ID;
            size = sizeof(DPPacket);
            *buffer = new UBYTE[size];
            memcpy(*buffer, &dp, size);
            mState = SBNETWORK_SESSION_MASTER;
        }
        isHostMigrating = false;
        return true;
    }


    Packet* p;
    if ((p = mMaster->Receive())) { //Game loop network messages:
        if (p == nullptr)
            return false;

        switch (p->data[0]) {
        case ID_DISCONNECTION_NOTIFICATION:
        case ID_CONNECTION_LOST:
        {
            SDL_Log("RECEIVED: SBNETWORK_DISCONNECT");

            //ATPacket packet{};
            //DeserializePacket(p->data, p->length, &packet);
        		
        	//Find disconnected player 
            SBNetworkPlayer *disconnectedPlayer = nullptr;
            for (mPlayers.GetFirst(); !mPlayers.IsLast(); mPlayers.GetNext()) {
                if (static_cast<RAKNetworkPlayer*>(mPlayers.GetLastAccessed())->peer == p->guid) {
                    disconnectedPlayer = mPlayers.GetLastAccessed();
                    break;
                }
            }

            if (disconnectedPlayer != nullptr) { //relay information to game
                DPPacket dp{};
                dp.messageType = DPSYS_DESTROYPLAYERORGROUP;
                dp.playerType = DPPLAYERTYPE_PLAYER;
                dp.dpId = disconnectedPlayer->ID;
                size = sizeof(DPPacket);
                *buffer = new UBYTE[size];
                memcpy(*buffer, &dp, size);
            	
                mPlayers.RemoveLastAccessed();
                delete disconnectedPlayer;
            }
            if (mHost == p->guid) { //The server disconnected
                isHostMigrating = true;
            }
            return true;
        }
        case SBNETWORK_MESSAGE: //Normal packet
        {
            ATPacket packet{};
            DeserializePacket(p->data, p->length, &packet);

        	//Check if the package was meant for us. 0 for broadcast
            if(packet.peerID && packet.peerID != mLocalID) {
                SDL_Log("RECEIVED PRIVATE: SBNETWORK_MESSAGE - ID: %d. IGNORED, SEND NOT TO US", packet.data[3] << 24 | packet.data[2] << 16 | packet.data[1] << 8 | packet.data[0]);
	            break;
            }
        		
            *buffer = packet.data;
            size = packet.dataLength;
            if(packet.peerID)
                SDL_Log("RECEIVED PRIVATE: SBNETWORK_MESSAGE - ID: %d", packet.data[3] << 24 | packet.data[2] << 16 | packet.data[1] << 8 | packet.data[0]);
            else
				SDL_Log("RECEIVED: SBNETWORK_MESSAGE - ID: %d", packet.data[3] << 24 | packet.data[2] << 16 | packet.data[1] << 8 | packet.data[0]);
            
            mMaster->DeallocatePacket(p);
            return true;
        }
        case SBNETWORK_ESTABLISH_CONNECTION:
        { //New Connection to master peer
            RAKNetworkPlayer *player = new RAKNetworkPlayer();
            BitStream b(p->data, p->length, true);
            b.IgnoreBytes(1); //Net event ID
            b.Read(player->ID);
            player->peer = p->guid;//mMaster->GetGuidFromSystemAddress(p->systemAddress);
            mPlayers.Add(player);

            if (mState == SBNETWORK_SESSION_MASTER) {
                SDL_Log("RECEIVED: SBNETWORK_ESTABLISH_CONNECTION - Broadcast new ID: %x", player->ID);
                /* Broadcast the address of this peer to all other peers */
                RAKNetworkPeer peer;
                peer.netID = SBNETWORK_JOINED;
                peer.ID = player->ID;
                peer.guid = player->peer;
                peer.address = p->systemAddress;
                mMaster->Send((char*)&peer, sizeof(RAKNetworkPeer), HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->systemAddress, true);

                peer.netID = SBNETWORK_JOINED;
                peer.ID = mLocalID;
                peer.guid = mMaster->GetMyGUID();
                peer.address = mMaster->GetSystemAddressFromGuid(mMaster->GetMyGUID());
                mMaster->Send((char*)&peer, sizeof(RAKNetworkPeer), HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->systemAddress, false);
            } else {
                SDL_Log("RECEIVED: SBNETWORK_ESTABLISH_CONNECTION - From: '%d'", player->ID);
            }
            break;
        }
        case SBNETWORK_JOINED: //We joined the server, or someone else joined the master server
            if (mState == SBNETWORK_SESSION_CLIENT) {
                auto* nPeer = reinterpret_cast<RAKNetworkPeer*>(p->data);
                RAKNetworkPlayer *player = new RAKNetworkPlayer();
                player->ID = nPeer->ID;
                player->peer = nPeer->guid;
                mPlayers.Add(player);


                if (nPeer->guid == mHost) {
                    SDL_Log("RECEIVED: SBNETWORK_JOINED - Server acknowledged us!");
                    break;
                }

            	
            	if(isNATMode) {
            		
            	}else {
            		SystemAddress newPlayer = nPeer->address;

                    const char* address = newPlayer.ToString(false);
            		const unsigned short port = newPlayer.GetPort();
            		
                    SDL_Log("CONNECTING: New connection being made to: %s:%u", address, port);
                    mMaster->Connect(address, port, nullptr, 0);//Connect to other client
                    AwaitConnection(mMaster, true);
            	}

                SDL_Log("RECEIVED: SBNETWORK_JOINED - New ID: %u", nPeer->ID);
            }
            break;
        default:
            SDL_Log("Unknown package was received! ID:%d",p->data[0]);
            break;
        }

        mMaster->DeallocatePacket(p);
    }
    return false;
}
#pragma optimize("", on)


SBList<SBNetworkPlayer*>* RAKNetNetwork::GetAllPlayers() {
    return &mPlayers;
}

bool RAKNetNetwork::IsServerSearchable() {
	return true;
}

IServerSearchable* RAKNetNetwork::GetServerSearcher() {
	return this;
}

bool RAKNetNetwork::AwaitConnection(RakPeerInterface* peerInterface, bool isAnotherPeer) {
    while (true) {
        Packet* p = peerInterface->Receive();
        if (p == nullptr)
            continue;

        switch (p->data[0]) {
        case ID_CONNECTION_REQUEST_ACCEPTED:
        { //Send our ID to server so that others can connect to us
            BitStream data;
            data.Write((char)SBNETWORK_ESTABLISH_CONNECTION);
            data.Write(mLocalID);

        	if(isAnotherPeer == false)
				mHost = p->guid; //Only set the host guid if we are actually connecting to the host
        	
            mState = SBNETWORK_SESSION_CLIENT;
            SDL_Log("Connect(..) successful. We (%s) are now sending our ID: %d", mMaster->GetMyGUID().ToString(), mLocalID);

            peerInterface->Send(&data, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->systemAddress, false);
            peerInterface->DeallocatePacket(p);
            return true;
        }
        case ID_CONNECTION_ATTEMPT_FAILED:
        case ID_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY:
        case ID_OUR_SYSTEM_REQUIRES_SECURITY:
        case ID_PUBLIC_KEY_MISMATCH:
        case ID_INVALID_PASSWORD:
        case ID_CONNECTION_BANNED:
        case ID_INCOMPATIBLE_PROTOCOL_VERSION:
        case ID_NO_FREE_INCOMING_CONNECTIONS:
        case ID_IP_RECENTLY_CONNECTED:
            SDL_Log("Connect(..) failed!");
        	if(isAnotherPeer){
				peerInterface->Shutdown(0);
        	} //TODO: Send message to server to kick(? maybe) the new player whom we can't connect to
            peerInterface->DeallocatePacket(p);
            return false;
        case ID_ALREADY_CONNECTED:
            return false;
        default:
			SDL_Log("Unknown packet in connection phase! ID: %d", p->data[0]);
            peerInterface->DeallocatePacket(p);
            break;
        }

    }
}

SBList<SBStr>* RAKNetNetwork::GetSessionListAsync() {
	return nullptr;
    /*mSessions.Clear();
    mSessionInfo.Clear();

    if (mState == SBNETWORK_SESSION_SEARCHING) {
        Packet* packet;
        for (packet = mServerSearch->Receive(); packet; mServerSearch->DeallocatePacket(packet), packet = mServerSearch->Receive()) {
            switch (packet->data[0])
            {
            case ID_CONNECTION_LOST:
                printf("Lost connection to server.\n");
                break;
            case ID_CLOUD_GET_RESPONSE:
                CloudQueryResult cloudQueryResult;
                mSearchClient->OnGetReponse(&cloudQueryResult, packet);
                unsigned int rowIndex;

                for (rowIndex = 0; rowIndex < cloudQueryResult.rowsReturned.Size(); rowIndex++)
                {
                    CloudQueryRow* row = cloudQueryResult.rowsReturned[rowIndex];

                    BitStream bsIn(row->data, row->length, false);

                    SBSessionInfo info;
                    bsIn.Read(info);
                }
                break;
            }
        }
    }

    return &mSessions;*/
}

bool RAKNetNetwork::StartGetSessionListAsync() {
	if(mState != SBNETWORK_SESSION_SEARCHING) {
        mState = SBNETWORK_SESSION_SEARCHING;

		mServerBrowserPeer = RakPeerInterface::GetInstance();

        SocketDescriptor sd(0, nullptr);
        mServerBrowserPeer->Startup(1, &sd, 1);
		mServerBrowserPeer->SetMaximumIncomingConnections(0); //We don't want that anyone can connect to us, we connect to the server

		delete mRoomsPluginClient;

		mRoomsPluginClient = new RoomsPlugin();
		mServerBrowserPeer->AttachPlugin(mRoomsPluginClient);

		mRoomsPluginClient->SetServerAddress(SystemAddress(MASTER_SERVER_ADDRESS, MASTER_SERVER_PORT));
	}
	
    //if (mState != SBNETWORK_SESSION_SEARCHING) {
    //    //start searching..
    //    mServerSearch = RakPeerInterface::GetInstance();

    //    SocketDescriptor sd(0, nullptr);
    //    StartupResult sr = mServerSearch->Startup(1, &sd, 1);
    //    mServerSearch->SetMaximumIncomingConnections(0); //We don't want that anyone can connect to us, we connect to the server

    //    mServerSearch->AttachPlugin(mSearchClient);

    //    ConnectionAttemptResult result = mServerSearch->Connect(MASTER_SERVER_ADDRESS, MASTER_SERVER_PORT, nullptr, 0);

    //    if (result == CONNECTION_ATTEMPT_STARTED) {
    //        if (AwaitConnection(mServerSearch) == false) {
    //            return false;
    //        }
    //        mServerSearch->GetGUIDFromIndex(0);
    //    }

    //    mState = SBNETWORK_SESSION_SEARCHING;
    //}

    return true;
}

bool RAKNetNetwork::JoinSession(const SBStr& session, SBStr nickname) {
    RAKSessionInfo* info = NULL;
    for (mSessionInfo.GetFirst(); !mSessionInfo.IsLast(); mSessionInfo.GetNext()) {
        if (session == mSessionInfo.GetLastAccessed()->sessionName)
            info = static_cast<RAKSessionInfo*>(mSessionInfo.GetLastAccessed());
    }

    if (!info)
        return false;

    /* Initiate the connection, allocating the two channels 0 and 1. */
    if (Connect(info->address.ToString())) {
        mState = SBNETWORK_SESSION_CLIENT;
    }

    return true;
}