#include "stdafx.h"
#include "SbLib.h"
#include "network.h"
#include "BitStream.h"
#include "RAKNetNetwork.hpp"
#include "NatPunchthroughClient.h"
#include <StringCompressor.h>
#include "RAKNetRoomCallbacks.hpp"

using namespace RakNet;
#define AT_Log(a,...) AT_Log_I("RAKNetNetwork", a, __VA_ARGS__)

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

void HandleNetMessages(RakPeerInterface* net, std::function<bool(const Packet*, bool*)> callback) {
    SBList<Packet*> queuedMessages;

    while(true) {
	    Packet *p = net->Receive();
        if(p == nullptr)
            continue;

        bool shouldExit = false;
        const bool isPacketConsumed = callback(p, &shouldExit);
        
    	if(!isPacketConsumed) {
	        queuedMessages.Add(p);
        }

        if(shouldExit && !isPacketConsumed) {
	        break;
        }

		net->DeallocatePacket(p);

        if(shouldExit)
            break;
    }

    if(queuedMessages.GetNumberOfElements() == 0)
        return;

    FOREACH_SB(Packet, p, queuedMessages) {
        net->PushBackPacket(p, true);
    }
}

RAKNetNetwork::RAKNetNetwork() {
    TEAKRAND rand;
    rand.SRandTime();
	
    mLocalID = rand.Rand();

    isHostMigrating = false;

    mRoomsPluginClient = nullptr;
    mRoomCallbacks = new RAKNetRoomCallbacks(this);

    mMaster = RakPeerInterface::GetInstance();

    RAKNetworkPlayer *player = new RAKNetworkPlayer();
    player->ID = mLocalID;
    player->peer = mMaster->GetMyGUID();
    mPlayers.Add(player);

    AT_Log("RAKNetNetwork initialized! LocalId: %d", mLocalID);
}

SLONG RAKNetNetwork::GetMessageCount() {
    if(mMaster == nullptr)
        return 0;

    const unsigned int messageCountGame = mMaster->GetReceiveBufferSize();

    if (mState == SBSessionEnum::SBNETWORK_SESSION_SEARCHING) {
        //We are searching for a session on the master server!

    }

    return messageCountGame;
}


bool RAKNetNetwork::Connect(const char* host) {
    if (mMaster == nullptr)
        return false;

    if(mServerBrowserPeer != nullptr) {
        RakNetGUID gameHost = RakNetGUID();
        gameHost.FromString(host);

        this->mState = SBSessionEnum::SBNETWORK_SESSION_CLIENT;

        
        AT_Log("Opening NAT to %s\n", host);
        mNATPlugin->OpenNAT(gameHost, SystemAddress(MASTER_SERVER_ADDRESS, MASTER_SERVER_PORT));

        bool failed = false;
        HandleNetMessages(mServerBrowserPeer, [&](const Packet* packet, bool* shouldExit) {
            if (packet->data[0] == ID_NAT_TARGET_NOT_CONNECTED ||
                packet->data[0] == ID_NAT_TARGET_UNRESPONSIVE ||
                packet->data[0] == ID_NAT_CONNECTION_TO_TARGET_LOST ||
                packet->data[0] == ID_NAT_PUNCHTHROUGH_FAILED) {
                RakNetGUID guid;
                if (packet->data[0] == ID_NAT_PUNCHTHROUGH_FAILED) {
                    guid = packet->guid;
                } else {
                    RakNet::BitStream bs(packet->data, packet->length, false);
                    bs.IgnoreBytes(1);
                    bool b = bs.Read(guid);
                    RakAssert(b);
                }

                switch (packet->data[0]) {
                case ID_NAT_TARGET_NOT_CONNECTED:
                    AT_Log("Failed: ID_NAT_TARGET_NOT_CONNECTED\n");
                    break;
                case ID_NAT_TARGET_UNRESPONSIVE:
                    AT_Log("Failed: ID_NAT_TARGET_UNRESPONSIVE\n");
                    break;
                case ID_NAT_CONNECTION_TO_TARGET_LOST:
                    AT_Log("Failed: ID_NAT_CONNECTION_TO_TARGET_LOST\n");
                    break;
                case ID_NAT_PUNCHTHROUGH_FAILED:
                    AT_Log("Failed: ID_NAT_PUNCHTHROUGH_FAILED\n");
                    break;
                }

                failed = true;
                *shouldExit = false;
                return true;
            } else if (packet->data[0] == ID_NAT_PUNCHTHROUGH_SUCCEEDED) {
                unsigned char weAreTheSender = packet->data[1];
                if (weAreTheSender)
                    AT_Log("NAT punch success to remote system %s.\n", packet->systemAddress.ToString(true));
                else
                    AT_Log("NAT punch success from remote system %s.\n", packet->systemAddress.ToString(true));

                mMaster->Connect(packet->systemAddress.ToString(false), packet->systemAddress.GetPort(), nullptr, 0);
                
                *shouldExit = true;
                failed = AwaitConnection(mMaster, false);
                return true;
            }

        	return false;
        });
    }else{

	    RakNet::SocketDescriptor sd(0, nullptr);
	    if (mMaster->Startup(5, &sd, 1) == RAKNET_STARTED) {
	        mMaster->SetMaximumIncomingConnections(4);
	        mMaster->Connect(host, SERVER_PORT, nullptr, 0);

	        return AwaitConnection(mMaster,false);
	    }
    }
    return false;
}

void RAKNetNetwork::Initialize() {

}

void RAKNetNetwork::Disconnect() {
    delete mSessionInfo;
    mSessionInfo = nullptr;
    
	CloseSession();
    if(mMaster){
		mMaster->CloseConnection(mHost, true);
	    mMaster->Shutdown(500);
    }
	delete mMaster;
}

bool RAKNetNetwork::CreateSession(SBNetworkCreation* create) {
    RAKSessionInfo *info = new RAKSessionInfo();
    strcpy(info->sessionName, create->sessionName.c_str());
    info->hostID = mLocalID;
    info->address = mMaster->GetMyGUID();
    delete mSessionInfo;
    mSessionInfo = nullptr;
    mSessionInfo = info;

    mState = SBSessionEnum::SBNETWORK_SESSION_MASTER;

    switch (create->flags)
    {
    case SBCreationFlags::SBNETWORK_CREATE_TRY_NAT:
        if(mServerBrowserPeer == nullptr || !mServerBrowserPeer->IsActive())
			ConnectToMasterServer();

    	if(mServerBrowserPeer != nullptr) {
            AT_Log("CREATE SESSION: NAT. Our GUID: '%s' and our SystemAddress: '%s'", mMaster->GetMyGUID().ToString(), mMaster->GetSystemAddressFromGuid(mMaster->GetMyGUID()).ToString());
            return CreateRoom(info->sessionName, "ASS");
        }
        //allowed flow skip!
    case SBCreationFlags::SBNETWORK_CREATE_NONE:
    default: //No need for a NAT server if the user chose it
        mMaster->Startup(4, &SocketDescriptor(SERVER_PORT, ""), 1);
        AT_Log("CREATE SESSION: DIRECT. Our GUID: '%s' and our SystemAddress: '%s'", mMaster->GetMyGUID().ToString(), mMaster->GetSystemAddressFromGuid(mMaster->GetMyGUID()).ToString());
        mMaster->SetMaximumIncomingConnections(4);
        break;
    }

    return true;
}

void RAKNetNetwork::CloseSession() {
    AT_Log("END SESSION");
	mMaster->Shutdown(100);
    if(mServerBrowserPeer) {
	    mServerBrowserPeer->Shutdown(100);
        delete mServerBrowserPeer;
        delete mRoomsPluginClient;
        mRoomCallbacks->mMasterRooms.Clear();
        mServerBrowserPeer = nullptr;
        mMaster = nullptr;
        mRoomsPluginClient = nullptr;
    }
    mState = SBSessionEnum::SBNETWORK_SESSION_FINISHED;
}

ULONG RAKNetNetwork::GetLocalPlayerID() {
    return mLocalID;
}


void RAKNetNetwork::RequestHostedClients(RakNetGUID serverGuid) {

}

bool RAKNetNetwork::IsSessionFinished() {
    return mState == SBSessionEnum::SBNETWORK_SESSION_FINISHED || mState == SBSessionEnum::SBNETWORK_SESSION_SEARCHING;
}

bool RAKNetNetwork::IsInSession() {
    return mState == SBSessionEnum::SBNETWORK_SESSION_MASTER || mState == SBSessionEnum::SBNETWORK_SESSION_CLIENT;
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
        AT_Log("SEND PRIVATE: SBNETWORK_MESSAGE ID: - ID %x TO: %x", (a.data[3] << 24) | (a.data[2] << 16) | (a.data[1] << 8) | (a.data[0]), peerID);

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
        AT_Log("SEND: SBNETWORK_MESSAGE ID: - ID %x", (a.data[3] << 24) | (a.data[2] << 16) | (a.data[1] << 8) | (a.data[0]));
    }

	mMaster->Send(&data, HIGH_PRIORITY, RELIABLE_ORDERED, 0, NET_BROADCAST, true);
    

    return true;
}

#pragma optimize("", off)
bool RAKNetNetwork::Receive(UBYTE** buffer, ULONG& size) {
    if(mState == SBSessionEnum::SBNETWORK_SESSION_SEARCHING)
        return false;

    if (isHostMigrating) { //Host migration:
        AT_Log("MIGRATING HOST");
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
            mState = SBSessionEnum::SBNETWORK_SESSION_MASTER;
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
            AT_Log("RECEIVED: SBNETWORK_DISCONNECT");

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
        case SBEventEnum::SBNETWORK_MESSAGE: //Normal packet
        {
            ATPacket packet{};
            DeserializePacket(p->data, p->length, &packet);

        	//Check if the package was meant for us. 0 for broadcast
            if(packet.peerID && packet.peerID != mLocalID) {
                AT_Log("RECEIVED PRIVATE: SBNETWORK_MESSAGE - ID: %d. IGNORED, SEND NOT TO US", packet.data[3] << 24 | packet.data[2] << 16 | packet.data[1] << 8 | packet.data[0]);
	            break;
            }
        		
            *buffer = packet.data;
            size = packet.dataLength;
            if(packet.peerID)
                AT_Log("RECEIVED PRIVATE: SBNETWORK_MESSAGE - ID: %d", packet.data[3] << 24 | packet.data[2] << 16 | packet.data[1] << 8 | packet.data[0]);
            else
                AT_Log("RECEIVED: SBNETWORK_MESSAGE - ID: %d", packet.data[3] << 24 | packet.data[2] << 16 | packet.data[1] << 8 | packet.data[0]);
            
            mMaster->DeallocatePacket(p);
            return true;
        }
        case SBEventEnum::SBNETWORK_ESTABLISH_CONNECTION:
        { //New Connection to master peer
            RAKNetworkPlayer *player = new RAKNetworkPlayer();
            BitStream b(p->data, p->length, true);
            b.IgnoreBytes(1); //Net event ID
            b.Read(player->ID);
            player->peer = p->guid;//mMaster->GetGuidFromSystemAddress(p->systemAddress);
            mPlayers.Add(player);

            if (mState == SBSessionEnum::SBNETWORK_SESSION_MASTER) {
                AT_Log("RECEIVED: SBNETWORK_ESTABLISH_CONNECTION - Broadcast new ID: %x", player->ID);
                /* Broadcast the address of this peer to all other peers */
                RAKNetworkPeer peer;
                peer.netID = SBEventEnum::SBNETWORK_JOINED;
                peer.ID = player->ID;
                peer.guid = player->peer;
                peer.address = p->systemAddress;
                mMaster->Send((char*)&peer, sizeof(RAKNetworkPeer), HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->systemAddress, true);

                peer.netID = SBEventEnum::SBNETWORK_JOINED;
                peer.ID = mLocalID;
                peer.guid = mMaster->GetMyGUID();
                peer.address = mMaster->GetSystemAddressFromGuid(mMaster->GetMyGUID());
                mMaster->Send((char*)&peer, sizeof(RAKNetworkPeer), HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->systemAddress, false);
            } else {
                AT_Log("RECEIVED: SBNETWORK_ESTABLISH_CONNECTION - From: '%d'", player->ID);
            }
            break;
        }
        case SBEventEnum::SBNETWORK_JOINED: //We joined the server, or someone else joined the master server
            if (mState == SBSessionEnum::SBNETWORK_SESSION_CLIENT) {
                auto* nPeer = reinterpret_cast<RAKNetworkPeer*>(p->data);
                RAKNetworkPlayer *player = new RAKNetworkPlayer();
                player->ID = nPeer->ID;
                player->peer = nPeer->guid;
                mPlayers.Add(player);


                if (nPeer->guid == mHost) {
                    AT_Log("RECEIVED: SBNETWORK_JOINED - Server acknowledged us!");
                    break;
                }

            	
            	if(isNATMode) {
            		
            	}else {
            		SystemAddress newPlayer = nPeer->address;

                    const char* address = newPlayer.ToString(false);
            		const unsigned short port = newPlayer.GetPort();
            		
                    AT_Log("CONNECTING: New connection being made to: %s:%u", address, port);
                    mMaster->Connect(address, port, nullptr, 0);//Connect to other client
                    AwaitConnection(mMaster, true);
            	}

                AT_Log("RECEIVED: SBNETWORK_JOINED - New ID: %u", nPeer->ID);
            }
            break;
		case ID_ROOMS_EXECUTE_FUNC:{
            bool ignoreFunc = false;

            switch (p->data[1]) {
	            case RPN_ROOM_MEMBER_JOINED_ROOM:
                case RPN_ROOM_MEMBER_LEFT_ROOM:
                case RPO_SEARCH_BY_FILTER:
	                ignoreFunc = true;
	                break;
            }

            if(ignoreFunc)
                break;
        }
        case ID_ROOMS_LOGON_STATUS:
        case ID_ROOMS_HANDLE_CHANGE:
            AT_Log("Main loop recieved lobby message! ID:%d, Func ID:%d", p->data[0], p->data[1]);
            mMaster->PushBackPacket(p, true);
            return false;

        //TODO: Add NAT Succeed message handling here!
        default:
            AT_Log("Unknown package was received! ID:%d",p->data[0]);
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

SBCapabilitiesFlags RAKNetNetwork::GetCapabilities() {
	if(this->GetState() == SBSessionEnum::SBNETWORK_SESSION_MASTER)
        return SBCapabilitiesFlags::SBNETWORK_NONE;

    return  SBCapabilitiesFlags::SBNETWORK_HAS_SERVER_BROWSER;
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
            data.Write((char)SBEventEnum::SBNETWORK_ESTABLISH_CONNECTION);
            data.Write(mLocalID);

        	if(isAnotherPeer == false)
				mHost = p->guid; //Only set the host guid if we are actually connecting to the host
        	
            mState = SBSessionEnum::SBNETWORK_SESSION_CLIENT;
            AT_Log("Connect(..) successful. We (%s) are now sending our ID: %d", mMaster->GetMyGUID().ToString(), mLocalID);

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
            AT_Log("Connect(..) failed! Reason: %d", p->data[0]);
        	if(isAnotherPeer){
				peerInterface->Shutdown(0);
        	} //TODO: Send message to server to kick(? maybe) the new player whom we can't connect to
            peerInterface->DeallocatePacket(p);
            return false;
        case ID_ALREADY_CONNECTED:
            return false;
        default:
            AT_Log("Unknown packet in connection phase! ID: %d", p->data[0]);
            peerInterface->PushBackPacket(p,false);
            break;
        }

    }
}

enum ServerBrowserMessageTypes : unsigned char {
    REGISTER_NAME = ID_USER_PACKET_ENUM,
};


void WriteStringToBitStream(const char* myString, RakNet::BitStream* output) {
    StringCompressor::Instance()->EncodeString(myString, 256, output);
}

#pragma region Master Server Functions


RakString username;
RakString baseName = "WizzardMaker";
void RAKNetNetwork::LoginMasterServer() {
    AT_Log("SBNETWORK: Connection to master server established, sending username \"%s\"", username.C_String());
    
    const int64_t id = rand();
    username = (baseName + RakString::ToString(id)).C_String();
	RakNet::BitStream* data = new RakNet::BitStream();
	data->Write(REGISTER_NAME);
	WriteStringToBitStream(username, data);
	mServerBrowserPeer->Send(data, IMMEDIATE_PRIORITY, RELIABLE, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
    delete data;
}

bool RAKNetNetwork::ConnectToMasterServer() {
    AT_Log("Connecting to master server...");
    mServerBrowserPeer = mMaster;//RakPeerInterface::GetInstance();

    SocketDescriptor sd(0, "");
    mServerBrowserPeer->Startup(5, &sd, 1);
    mServerBrowserPeer->SetMaximumIncomingConnections(5);

    delete mRoomsPluginClient;
    delete mNATPlugin;

    mNATPlugin = new NatPunchthroughClient();
    mServerBrowserPeer->AttachPlugin(mNATPlugin);
    NatPunchthroughDebugInterface_Printf *debug = new NatPunchthroughDebugInterface_Printf();
    mNATPlugin->SetDebugInterface(debug);
    PunchthroughConfiguration* config = mNATPlugin->GetPunchthroughConfiguration();

    config->retryOnFailure = true;

    mRoomsPluginClient = new RoomsPlugin();
    mServerBrowserPeer->AttachPlugin(mRoomsPluginClient);

    mRoomsPluginClient->SetServer(false);
    mRoomsPluginClient->SetServerAddress(SystemAddress(MASTER_SERVER_ADDRESS, MASTER_SERVER_PORT));
    mRoomsPluginClient->SetRoomsCallback(mRoomCallbacks);

    RakNet::ConnectionAttemptResult car = mServerBrowserPeer->Connect(MASTER_SERVER_ADDRESS, MASTER_SERVER_PORT, nullptr, 0);
    while (true) {
        Packet* packet = mServerBrowserPeer->Receive();
        if (packet == nullptr)
            continue;

        switch (packet->data[0]) {
        case ID_CONNECTION_REQUEST_ACCEPTED:
            AT_Log("Connected to master server!");
            LoginMasterServer();
            mServerBrowserPeer->DeallocatePacket(packet);
            mIsConnectingToMaster = false;
            return true;
        case ID_CONNECTION_ATTEMPT_FAILED:
        case ID_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY:
        case ID_OUR_SYSTEM_REQUIRES_SECURITY:
        case ID_PUBLIC_KEY_MISMATCH:
        case ID_INVALID_PASSWORD:
        case ID_CONNECTION_BANNED:
        case ID_INCOMPATIBLE_PROTOCOL_VERSION:
        case ID_NO_FREE_INCOMING_CONNECTIONS:
        case ID_IP_RECENTLY_CONNECTED:
        case ID_ALREADY_CONNECTED:
            AT_Log("Connect to master server failed! Reason: %d", packet->data[0]);
            break;
        default:
            AT_Log("Unknown packet in master server connection phase! ID: %d", packet->data[0]);
            mServerBrowserPeer->PushBackPacket(packet, false);
            break;
        }
        mIsConnectingToMaster = false;
        mServerBrowserPeer->DeallocatePacket(packet);
        mState = SBSessionEnum::SBNETWORK_SESSION_FINISHED;
        return false;
    }
}

#pragma endregion

bool RAKNetNetwork::DidFuncSucceed(const RoomsPluginOperations func) const {
    bool failed = false;
    HandleNetMessages(mServerBrowserPeer, [&](const Packet* p, bool* shouldExit) {
        *shouldExit = true;

        switch (p->data[0]) {
        case ID_ROOMS_EXECUTE_FUNC:
			if(p->data[1] != func)
                break;

            //We received an answer to our room creation
            if (mRoomCallbacks->lastFunc->resultCode != REC_SUCCESS) {
                failed = true;
            }

            return true;
        }

        *shouldExit = false;
        return false;
    });

    return !failed;
}

void RAKNetNetwork::RetrieveRoomList() {
    if(mState != SBSessionEnum::SBNETWORK_SESSION_SEARCHING)
        return;

    AT_Log("Fetching room list...");
    RakNet::SearchByFilter_Func func;
    func.gameIdentifier = MASTER_SERVER_GAME_ID;
    func.onlyJoinable = true;
    func.userName = username;
    mRoomsPluginClient->ExecuteFunc(&func);
    const bool succeeded = DidFuncSucceed(RPO_SEARCH_BY_FILTER);
}

bool RAKNetNetwork::JoinRoom(const SBStr* roomName) {
    AT_Log("Joining Room (%s)", roomName->c_str());
    RakNet::EnterRoom_Func func;
    func.gameIdentifier = MASTER_SERVER_GAME_ID;
    func.userName = username;
    
    char *roomSearchName = new char[roomName->GetLength()];
    strcpy(roomSearchName, roomName->c_str());

    func.query.AddQuery_STRING(
        DefaultRoomColumns::GetColumnName(DefaultRoomColumns::TC_ROOM_NAME),
        roomSearchName);
    func.query.AddQuery_STRING(
        DefaultRoomColumns::GetColumnName(DefaultRoomColumns::TC_ROOM_PASSWORD),
        "ASS");
    func.roomMemberMode = RMM_PUBLIC;
    mRoomsPluginClient->ExecuteFunc(&func);

    const bool succeeded = DidFuncSucceed(RPO_ENTER_ROOM);

    if (!succeeded)
        DisplayBroadcastMessage("Failed to join a session");

    return succeeded;
}

bool RAKNetNetwork::CreateRoom(const char* roomNameC, const char* password) const {
    CString roomName = CString(roomNameC).Trim();

	AT_Log("Creating Room (%s)", roomName.c_str());

    RakNet::CreateRoom_Func func{};
    func.gameIdentifier = MASTER_SERVER_GAME_ID;
    func.userName = username;

    func.networkedRoomCreationParameters.roomName = roomName;
    func.networkedRoomCreationParameters.slots.publicSlots = 4;
    func.networkedRoomCreationParameters.password = password;
    
    mRoomsPluginClient->ExecuteFunc(&func);

    const bool succeeded = DidFuncSucceed(RPO_CREATE_ROOM);

    if(!succeeded)
        DisplayBroadcastMessage("Failed to create a new session");

    return succeeded;
}

SBList<std::shared_ptr<SBStr>>* RAKNetNetwork::GetSessionListAsync() {
	if (mState == SBSessionEnum::SBNETWORK_SESSION_SEARCHING && mServerBrowserPeer && !mIsConnectingToMaster) {
    //    while (true) {
    //        Packet* packet = mServerBrowserPeer->Receive();
    //        if (packet == nullptr)
    //            break;

    //        if(packet->data[0] == ID_ROOMS_EXECUTE_FUNC) {
    //            mServerBrowserPeer->DeallocatePacket(packet);
    //        }else {
				//mServerBrowserPeer->PushBackPacket(packet,false);    
    //        }
    //    }

		RetrieveRoomList();

        if (mRoomCallbacks->mMasterRooms.GetNumberOfElements() != 0)
            return &mRoomCallbacks->mMasterRooms;
    }

	return nullptr;
}

bool RAKNetNetwork::StartGetSessionListAsync() {
	if(mState != SBSessionEnum::SBNETWORK_SESSION_SEARCHING) {
        mIsConnectingToMaster = true;
        mState = SBSessionEnum::SBNETWORK_SESSION_SEARCHING;

        return ConnectToMasterServer();
	}

	return true;
}

bool RAKNetNetwork::JoinSession(const SBStr& session, SBStr nickname) {
    if(!JoinRoom(&session))
        return false;

    RAKSessionInfo *info = new RAKSessionInfo();

    strcpy(info->sessionName, session.c_str());
    info->address = mRoomCallbacks->joinedRoom.roomDescriptor.roomMemberList[0].guid;
    
    mSessionInfo = info;
    
   // while (true) {
   //     bool skp = false, brk = false;
   //     Packet* packet = mServerBrowserPeer->Receive();
   //     switch (packet->data[0]) {
			//case ID_ROOMS_EXECUTE_FUNC:
   //             
   //             break;
   //         default:
   //             queuedMessages.Add(packet);
			//	skp = true;
   //     }

   //     if(!skp)
			//mServerBrowserPeer->DeallocatePacket(packet);

   // 	if(brk)
   //         break;
   // }    

    if (mSessionInfo == nullptr){
        DisplayBroadcastMessage("Failed to join session " + session);
    	return false;
    }

    /* Initiate the connection, allocating the two channels 0 and 1. */
    if (Connect(mSessionInfo->address.ToString())) {
        mState = SBSessionEnum::SBNETWORK_SESSION_CLIENT;
    }

    return true;
}