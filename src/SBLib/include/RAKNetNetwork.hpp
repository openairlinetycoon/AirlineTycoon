#pragma once

#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "rooms-plugin\RoomsPlugin.h"

#define RAKNET_TYPE_DIRECT_JOIN "Direct-IP  Join"
#define RAKNET_TYPE_DIRECT_HOST "Direct-IP  Host"
#define RAKNET_TYPE_NAT_HOST "RAKNet NAT Host"
#define RAKNET_TYPE_NAT_JOIN "RAKNet NAT Join"

constexpr auto  MASTER_SERVER_GAME_ID = "ATD";
constexpr auto  MASTER_SERVER_PORT = 61013;
constexpr auto  MASTER_SERVER_ADDRESS = "92.205.167.58";//"62.75.148.96";

#pragma pack(push, 1)
namespace RakNet {
	class NatPunchthroughClient;
	class UDPProxyClient;
}

class RAKNetRoomCallbacks;
class UDPCallbacks;

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
	bool ConnectToUDP(RakNet::RakNetGUID gameHost);
	bool Connect(const char*) override;
	bool CreateSession(SBNetworkCreation*) override;
	void CloseSession() override;
	ULONG GetLocalPlayerID() override;
	bool Send(BUFFER<UBYTE>&, ULONG, ULONG, bool) override;
	bool Receive(UBYTE**, ULONG&) override;
    SBList<SBNetworkPlayer*>* GetAllPlayers() override;
	SBCapabilitiesFlags GetCapabilities() override;
	bool IsServerSearchable() override;
	IServerSearchable *GetServerSearcher() override;
	int CheckConnectionPacket(RakNet::Packet* p, RakNet::RakPeerInterface* peerInterface, bool isAnotherPeer);

	//Server Searchable:

	void LoginMasterServer();
	void RetrieveRoomList();
	bool JoinRoom(const SBStr* roomName);
	bool DidFuncSucceed(const RakNet::RoomsPluginOperations func) const;
	SBList<std::shared_ptr<SBStr>>* GetSessionListAsync() override;
	bool StartGetSessionListAsync() override;
	bool JoinSession(const SBStr& sessionName, SBStr nickname) override;
	void SetNatMode(bool enabled);
	
private:
	friend class UDPCallbacks;

    /// <summary>
	/// Polls the interface until a connection was established
	/// </summary>
	/// <param name="peerInterface">The interface to check fo a successful connection</param>
	/// <returns>true - when a connection was established</returns>
    bool AwaitConnection(RakNet::RakPeerInterface* peerInterface);

	void InitPlayerList();

    RakNet::SystemAddress mServer;
    RakNet::RakNetGUID *mHost = nullptr;
    RakNet::RakPeerInterface* mMaster = nullptr;
	RakNet::NatPunchthroughClient* mNATPlugin = nullptr;
	RakNet::UDPProxyClient*  mUdpClient = nullptr;
	bool mIsNATMode = false;

    SBList<RakNet::Packet*> mPackets;

	RakNet::RoomsPlugin* mRoomsPluginClient = nullptr;
	RakNet::RakPeerInterface* mServerBrowserPeer = nullptr;
	RAKNetRoomCallbacks *mRoomCallbacks = nullptr;
	RAKSessionInfo *mSessionInfo = nullptr;
	bool mIsConnectingToMaster = false;
	UDPCallbacks *mUdpCallbackHandler = nullptr;

	/// <summary>
	/// Starts to retrieve a list of clients that are connected to the specified master server
	/// Response will be send to the mServerSearch peer
	/// </summary>
	/// <param name="serverGuid">The GUID of the master server</param>
	void RequestHostedClients(RakNet::RakNetGUID serverGuid);

	/// <summary>
	/// Connects blocking to the master server and on successful connection will attempt a login with the username
	/// </summary>
	/// <returns>true - on success, otherwise false</returns>
	bool ConnectToMasterServer();

	bool CreateRoom(const char* roomName, const char* password) const;

	//Server Search network elements:
	RakNet::RakPeerInterface* mRoomClient = nullptr;
};

