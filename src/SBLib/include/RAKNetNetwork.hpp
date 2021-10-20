#pragma once

#include "BaseNetworkType.hpp"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "CloudClient.h"

#define RAKNET_TYPE_DIRECT_JOIN "Direct-IP  Join"
#define RAKNET_TYPE_DIRECT_HOST "Direct-IP  Host"
#define RAKNET_TYPE_NAT_HOST "RAKNet NAT Host"
#define RAKNET_TYPE_NAT_JOIN "RAKNet NAT Join"


#pragma pack(push, 1)
namespace RakNet {
	class NatPunchthroughClient;
}

struct RAKNetworkPeer
{
	BYTE netID;
	ULONG ID;
	RakNet::RakNetGUID guid;
	RakNet::SystemAddress address;
};
#pragma pack(pop)
struct RAKNetworkPlayer : public SBNetworkPlayer
{
	//ULONG ID;
	RakNet::RakNetGUID peer;
};
struct RAKSessionInfo : public SBSessionInfo
{
	//ULONG hostID;
	//char sessionName[26];
	RakNet::RakNetGUID address;
};

class RAKNetNetwork : public BaseNetworkType, public IServerSearchable {
public:
	RAKNetNetwork();

	void Initialize() override;
	void Disconnect() override;
	bool IsSessionFinished() override;
	bool IsInSession() override;
	SLONG GetMessageCount() override;
	bool Connect(const char*) override;
	bool CreateSession(SBNetworkCreation*) override;
	void CloseSession() override;
	ULONG GetLocalPlayerID() override;
	bool Send(BUFFER<UBYTE>&, ULONG, ULONG, bool) override;
	bool Receive(UBYTE**, ULONG&) override;
    SBList<SBNetworkPlayer*>* GetAllPlayers() override;
	bool IsServerSearchable() override;
	IServerSearchable *GetServerSearcher() override;

	//Server Searchable:
	
	SBList<SBStr>* GetSessionListAsync() override;
	bool StartGetSessionListAsync() override;
	bool JoinSession(const SBStr& sessionName, SBStr nickname) override;

private:
    /// <summary>
	/// Polls the interface until a connection was established
	/// </summary>
	/// <param name="peerInterface">The interface to check fo a successful connection</param>
	/// <returns>true - when a connection was established</returns>
    bool AwaitConnection(RakNet::RakPeerInterface* peerInterface, bool isAnotherPeer);

    RakNet::SystemAddress mServer;
    RakNet::RakNetGUID mHost;
    RakNet::RakPeerInterface* mMaster = nullptr;
	RakNet::NatPunchthroughClient* mNATPlugin = nullptr;
	bool isNATMode = false;

    SBList<RakNet::Packet*> mPackets;

	/// <summary>
	/// Starts to retrieve a list of clients that are connected to the specified master server
	/// Response will be send to the mServerSearch peer
	/// </summary>
	/// <param name="serverGuid">The GUID of the master server</param>
	void RequestHostedClients(RakNet::RakNetGUID serverGuid);

	//Server Search network elements:
	RakNet::RakPeerInterface* mRoomClient;
};
