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
    }

    size_t GetMessageCount()
    {
        DebugBreak();
        return 0;
    }

    bool Connect(SBStr)
    {
        DebugBreak();
        return false;
    }

    bool Connect(SBStr, char*)
    {
        DebugBreak();
        return false;
    }

    void DisConnect()
    {
    }

    bool CreateSession(SBStr, SBNetworkCreation*)
    {
        DebugBreak();
        return false;
    }

    void CloseSession()
    {
        DebugBreak();
    }

    unsigned long GetLocalPlayerID()
    {
        return 0;
    }

    SBList<SBStr>* GetConnectionList()
    {
        DebugBreak();
        return NULL;
    }

    SBList<SBStr>* GetSessionListAsync()
    {
        DebugBreak();
        return NULL;
    }

    bool StartGetSessionListAsync()
    {
        DebugBreak();
        return false;
    }

    GUID* GetProviderGuid(char*)
    {
        DebugBreak();
        return NULL;
    }

    bool IsEnumSessionFinished()
    {
        DebugBreak();
        return false;
    }

    bool IsInSession()
    {
        DebugBreak();
        return false;
    }

    bool Send(struct TEAKFILE::DummyBuffer&, unsigned long, unsigned long, bool)
    {
        DebugBreak();
        return false;
    }

    bool Receive(UBYTE**, unsigned long&)
    {
        DebugBreak();
        return false;
    }

    bool JoinSession(SBStr, SBStr)
    {
        DebugBreak();
        return false;
    }

    SBList<SBNetworkPlayer>* GetAllPlayers()
    {
        DebugBreak();
        return NULL;
    }
};
