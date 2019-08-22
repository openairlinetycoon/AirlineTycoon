#pragma once

#include <enet/enet.h>

struct DPPacket
{
    ULONG messageType;
    DWORD playerType;
    DPID  dpId;
};

struct SBNetworkPlayer
{
    unsigned long ID;
    ENetPeer* peer;
};

struct SBNetworkPeer
{
    unsigned long ID;
    ENetAddress address;
};

enum SBSessionEnum
{
    SBNETWORK_SESSION_DEFAULT,
    SBNETWORK_SESSION_SEARCHING,
    SBNETWORK_SESSION_MASTER,
    SBNETWORK_SESSION_CLIENT,
    SBNETWORK_SESSION_FINISHED
};

struct SBNetworkCreation
{
    SBStr sessionName;
    long maxPlayers;
    long flags;
};

struct SBSessionInfo
{
    unsigned long hostID;
    char sessionName[26];
    ENetAddress address;
};

class SBNetwork
{
public:
    SBNetwork(bool, GUID);

    long GetMessageCount();
    bool Connect(SBStr);
    bool Connect(SBStr, char*);
    void DisConnect();
    bool CreateSession(SBStr, SBNetworkCreation*);
    void CloseSession();
    unsigned long GetLocalPlayerID();
    SBList<SBStr>* GetConnectionList();
    SBList<SBStr>* GetSessionListAsync();
    bool StartGetSessionListAsync();
    GUID* GetProviderGuid(char*);
    bool IsEnumSessionFinished();
    bool IsInSession();
    bool Send(BUFFER<UBYTE>&, unsigned long, unsigned long, bool);
    bool Receive(UBYTE**, unsigned long&);
    bool JoinSession(SBStr, SBStr);
    SBList<SBNetworkPlayer>* GetAllPlayers();

private:
    SBSessionEnum mState;
    unsigned long mLocalID;
    SBList<SBStr> mConnections;
    SBList<SBStr> mSessions;
    SBList<SBSessionInfo> mSessionInfo;
    SBList<SBNetworkPlayer> mPlayers;

    ENetAddress mServer;
    ENetHost* mHost;
    ENetPeer* mMaster;
    ENetSocket mSocket;
    SBList<ENetPacket*> mPackets;
};
