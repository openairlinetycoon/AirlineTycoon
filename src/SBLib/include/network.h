#pragma once

struct SBNetworkPlayer
{
    unsigned long ID;
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
    SBNetwork(bool, GUID)
    {
        m_connections.Add("Unknown");
    }

    size_t GetMessageCount()
    {
        return 0;
    }

    bool Connect(SBStr)
    {
        return false;
    }

    bool Connect(SBStr, char*)
    {
        return false;
    }

    void DisConnect()
    {
    }

    bool CreateSession(SBStr, SBNetworkCreation*)
    {
        return false;
    }

    void CloseSession()
    {
    }

    unsigned long GetLocalPlayerID()
    {
        return 0;
    }

    SBList<SBStr>* GetConnectionList()
    {
        return &m_connections;
    }

    SBList<SBStr>* GetSessionListAsync()
    {
        return &m_sessions;
    }

    bool StartGetSessionListAsync()
    {
        return false;
    }

    GUID* GetProviderGuid(char*)
    {
        return NULL;
    }

    bool IsEnumSessionFinished()
    {
        return false;
    }

    bool IsInSession()
    {
        return false;
    }

    bool Send(struct TEAKFILE::DummyBuffer&, unsigned long, unsigned long, bool)
    {
        return false;
    }

    bool Receive(UBYTE**, unsigned long&)
    {
        return false;
    }

    bool JoinSession(SBStr, SBStr)
    {
        return false;
    }

    SBList<SBNetworkPlayer>* GetAllPlayers()
    {
        return &m_players;
    }

private:
    SBList<SBStr> m_connections;
    SBList<SBStr> m_sessions;
    SBList<SBNetworkPlayer> m_players;
};
