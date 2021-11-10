#pragma once

#include "MessageIdentifiers.h"

//Direct play identifiers
constexpr auto  DPPLAYERTYPE_GROUP = 0x0000;
constexpr auto  DPPLAYERTYPE_PLAYER = 0x0001;
constexpr auto  DPSYS_DESTROYPLAYERORGROUP = 0x0005;
constexpr auto  DPSYS_SESSIONLOST = 0x0031;
constexpr auto  DPSYS_HOST = 0x0101;

constexpr auto  SERVER_PORT = 60011;
constexpr auto  CLIENT_PORT = 60012;


constexpr auto  KEY_GET_HOST_LIST = "GetHostList";

#define NET_BROADCAST UNASSIGNED_RAKNET_GUID


typedef DWORD DPID;

#pragma pack(push, 1)
struct ATPacket
{
	unsigned char messageType;
	ULONG peerID;
	ULONG dataLength;
	UBYTE* data;
};
#pragma pack(pop)
struct DPPacket
{
	ULONG messageType;
	DWORD playerType;
	DPID  dpId;
};

struct SBNetworkAddress {
	ULONG peerID;
};

struct SBNetworkPlayer
{
	ULONG ID;
	//RAKNetworkPeer peer;
};

enum SBSessionEnum
{
	SBNETWORK_SESSION_DEFAULT,
	SBNETWORK_SESSION_SEARCHING,
	//User is server host
	SBNETWORK_SESSION_MASTER,
	//User is client
	SBNETWORK_SESSION_CLIENT,
	//Session is over
	SBNETWORK_SESSION_FINISHED
};

enum SBTypeEnum
{
	SBNETWORK_RAKNET_DIRECT_JOIN,
	SBNETWORK_RAKNET_DIRECT_HOST,
	SBNETWORK_RAKNET_NAT_JOIN,
	SBNETWORK_RAKNET_NAT_HOST,
	SBNETWORK_ENET,
};

enum SBEventEnum {
	SBNETWORK_ESTABLISH_CONNECTION = ID_USER_PACKET_ENUM + 1,
	SBNETWORK_JOINED,
	SBNETWORK_MESSAGE,
	SBNETWORK_DISCONNECT,
};

enum class SBCreationFlags {
	SBNETWORK_CREATE_NONE = 2^0,
	SBNETWORK_CREATE_TRY_NAT = 2^1,
};

struct SBNetworkCreation
{
	SBStr sessionName;
	SLONG maxPlayers;
	SBCreationFlags flags;
};

struct SBSessionInfo
{
	ULONG hostID;
	char sessionName[26];
};

class IServerSearchable {
public:
	IServerSearchable() {  }
	virtual ~IServerSearchable() = default;
	virtual SBList<SBStr>* GetSessionListAsync() = 0;
	virtual bool StartGetSessionListAsync() = 0;
	virtual bool JoinSession(const SBStr&, SBStr) = 0;

protected:
	SBList<SBStr> mSessions;
	SBList<SBSessionInfo *> mSessionInfo;
	unsigned int mSearchTime = 0;
};

class BaseNetworkType {
public:
	BaseNetworkType() {  }
	
	virtual ~BaseNetworkType() = default;

	/// <summary>
	/// Initializes the Network Adapter
	/// </summary>
	virtual void Initialize() = 0;
	/// <summary>
	/// Calculates the number of messages in the network message queue
	/// </summary>
	/// <returns>Nr of messages in queue</returns>
	virtual SLONG GetMessageCount() = 0;
	/// <summary>
	/// Connects to the given address
	/// </summary>
	/// <param name="address">The address to connect to</param>
	/// <returns>Whether it succeeded</returns>
	virtual bool Connect(const char* address) = 0;
	/// <summary>
	/// Disconnects from the current session
	/// </summary>
	virtual void Disconnect() = 0;
	/// <summary>
	/// Creates a new session with the given settings.
	/// </summary>
	/// <param name="sessionSettings"></param>
	/// <returns>Whether it succeeded</returns>
	virtual bool CreateSession(SBNetworkCreation* sessionSettings) = 0;
	/// <summary>
	/// Closes the current session.
	/// </summary>
	virtual void CloseSession() = 0;
	/// <summary>
	/// Returns the local player ID. Uses the implementations definition of ID
	/// </summary>
	virtual ULONG GetLocalPlayerID() = 0;
	
	virtual bool IsSessionFinished() = 0;
	virtual bool IsInSession() = 0;

	/// <summary>
	/// Sends the buffer to (a) the player with matching peerID (b) everyone when peerID == 0
	/// </summary>
	/// <param name="compression">Whether to suggest compression. Depends on the actual implementation</param>
	/// <returns>Whether it succeeded</returns>
	virtual bool Send(BUFFER<UBYTE>& buffer, ULONG length, ULONG peerID, bool compression) = 0;

	/// <summary>
	/// Retrieves a packet from the message buffer. Does not wait for a packet to arrive.
	/// Handles host migration if the host dropped
	/// </summary>
	/// <returns>Whether it succeeded</returns>
	virtual bool Receive(UBYTE** buffer, ULONG& length) = 0;

	/// <summary>
	/// Retrieves a list of every connected player (including the local player)
	/// </summary>
	virtual SBList<SBNetworkPlayer*>* GetAllPlayers() = 0;

	virtual bool IsServerSearchable() = 0;
	virtual IServerSearchable* GetServerSearcher() = 0;
	
	SBSessionEnum GetState() const {
		return mState;
	}

protected:
	bool isHostMigrating = false;
	ULONG mLocalID = -1;

	SBList<SBNetworkPlayer*> mPlayers;
	
	SBSessionEnum mState = SBNETWORK_SESSION_DEFAULT;
};
