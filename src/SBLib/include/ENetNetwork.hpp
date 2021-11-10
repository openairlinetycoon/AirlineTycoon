#pragma once

#ifdef WIN32
//#define _WINSOCK2API_   /* Prevent inclusion of winsock2.h in enet.h */
#define INVALID_SOCKET (SOCKET)(~0)
//typedef ptrdiff_t SOCKET;
typedef struct fd_set fd_set;
#endif
#include <enet/enet.h>

#define ENET_TYPE "ENET"

typedef DWORD DPID;

struct ENetNetworkPlayer : public SBNetworkPlayer {
	//ULONG ID;
	ENetPeer* peer;
};

struct ENetNetworkPeer {
	ULONG ID;
	ENetAddress address;
};

struct ENetSessionInfo : SBSessionInfo{
//	ULONG hostID;
//	char sessionName[26];
	ENetAddress address;
};

class ENetNetwork : public BaseNetworkType, public IServerSearchable {
public:
	ENetNetwork();
	~ENetNetwork() override;
	void Initialize() override;
	SLONG GetMessageCount() override;
	bool Connect(const char* address) override;
	void Disconnect() override;
	bool CreateSession(SBNetworkCreation* sessionSettings) override;
	void CloseSession() override;
	ULONG GetLocalPlayerID() override;
	bool IsSessionFinished() override;
	bool IsInSession() override;
	bool Send(BUFFER<UBYTE>& buffer, ULONG length, ULONG peerID, bool compression) override;
	bool Receive(UBYTE** buffer, ULONG& length) override;
	SBList<SBNetworkPlayer*>* GetAllPlayers() override;
	bool IsServerSearchable() override;
	IServerSearchable* GetServerSearcher() override;

	//IServerSearchable:
	SBList<SBStr>* GetSessionListAsync() override;
	bool StartGetSessionListAsync() override;
	bool JoinSession(const SBStr&, SBStr) override;
private:
	ENetAddress mServer;
	ENetHost* mHost;
	ENetPeer* mMaster;
	ENetSocket mSocket;
	SBList<ENetPacket*> mPackets;
};
