#pragma once

#define RAKNET_NETWORK

#include "BaseNetworkType.hpp"

#ifdef ENET_NETWORK

#ifdef WIN32
#define _WINSOCK2API_   /* Prevent inclusion of winsock2.h in enet.h */
#define INVALID_SOCKET (SOCKET)(~0)
typedef struct fd_set fd_set;
typedef ptrdiff_t SOCKET;
#endif
#include <enet/enet.h>

typedef DWORD DPID;

//Direct play identifiers
#define DPPLAYERTYPE_GROUP          0x0000
#define DPPLAYERTYPE_PLAYER         0x0001
#define DPSYS_DESTROYPLAYERORGROUP  0x0005
#define DPSYS_SESSIONLOST           0x0031
#define DPSYS_HOST                  0x0101

struct DPPacket
{
    ULONG messageType;
    DWORD playerType;
    DPID  dpId;
};

struct SBNetworkPlayer
{
    ULONG ID;
    ENetPeer* peer;
};

struct SBNetworkPeer
{
    ULONG ID;
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

enum SBTypeEnum
{
    SBNETWORK_ENET,
};

struct SBNetworkCreation
{
    SBStr sessionName;
    SLONG maxPlayers;
    SLONG flags;
};

struct SBSessionInfo
{
    ULONG hostID;
    char sessionName[26];
    ENetAddress address;
};

class SBNetwork
{
public:
    SBNetwork(bool);

    SLONG GetMessageCount();
    bool Connect(SBStr);
    bool Connect(SBStr, char*);
    void DisConnect();
    bool CreateSession(SBStr, SBNetworkCreation*);
    void CloseSession();
    ULONG GetLocalPlayerID();
    SBList<SBStr>* GetConnectionList();
    SBList<SBStr>* GetSessionListAsync();
    bool StartGetSessionListAsync();
    SLONG GetProviderID(char*);
    bool IsEnumSessionFinished();
    bool IsInSession();
    bool Send(BUFFER<UBYTE>&, ULONG, ULONG, bool);
    bool Receive(UBYTE**, ULONG&);
    bool JoinSession(SBStr, SBStr);
    SBList<SBNetworkPlayer>* GetAllPlayers();

private:
    SBSessionEnum mState;
    ULONG mLocalID;
    SBList<SBStr> mConnections;
    SBList<SBStr> mSessions;
    SBList<SBSessionInfo> mSessionInfo;
    SBList<SBNetworkPlayer> mPlayers;

    SBTypeEnum mType;
    unsigned int mSearchTime;
    ENetAddress mServer;
    ENetHost* mHost;
    ENetPeer* mMaster;
    ENetSocket mSocket;
    SBList<ENetPacket*> mPackets;
};

#endif

#ifdef RAKNET_NETWORK


enum SBMultiplayerStateEnum
{
    SBNETWORK_SEARCHING,
	SBNETWORK_PLAYING,
	SBNETWORK_IDLE,
};


class SBNetwork
{
public:
    SBNetwork(bool);

    SLONG GetMessageCount();
    bool Connect(SBStr);
    bool Connect(SBStr, const char*);
    void DisConnect();
    bool CreateSession(SBStr, SBNetworkCreation*);
    void CloseSession();
    ULONG GetLocalPlayerID();
    SBList<SBStr>* GetConnectionList();
    SBList<SBStr>* GetSessionListAsync();
    bool StartGetSessionListAsync();
    SLONG GetProviderID(char*);
    void SetProvider(SBTypeEnum);
    bool IsEnumSessionFinished() const;
    bool IsInSession() const;
	bool IsInitialized() const;
    bool Send(BUFFER<UBYTE>&, ULONG, ULONG, bool);
    bool Receive(UBYTE**, ULONG&);
    bool JoinSession(const SBStr&, SBStr);
    SBList<SBNetworkPlayer*>* GetAllPlayers();

private:
    SBList<SBStr> mConnections;

    SBMultiplayerStateEnum mState;
	
	BaseNetworkType *mNetwork;
    SBTypeEnum mType;
};

#endif
