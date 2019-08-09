#pragma once

#include <enet/enet.h>

struct SBNetworkPlayer
{
    unsigned long ID;
    ENetPeer* peer;
};

enum
{
    SBNETWORK_SESSION_DEFAULT
};

class SBNetworkCreation
{
public:
    SBStr sessionName;
    long maxPlayers;
    long flags;
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
    bool m_InSession;
    unsigned long m_LocalID;
    SBList<SBStr> m_Connections;
    SBList<SBStr> m_Sessions;
    SBList<SBNetworkPlayer> m_Players;

    ENetHost* m_Host;
    SBList<ENetPacket*> m_Packets;
};
