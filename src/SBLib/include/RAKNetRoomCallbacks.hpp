#pragma once

#define AT_Log(a,...) AT_Log_I("RAKNetNetwork>Lobby", a, __VA_ARGS__)

#define printf AT_Log

#include <rooms-plugin/RoomsPlugin.h>

class RAKNetRoomCallbacks : public RakNet::RoomsCallback
{
private:
	RAKNetRoomCallbacks() {	 }

public:
	RAKNetRoomCallbacks(RAKNetNetwork* parent) : parent(parent) { }
	
	RAKNetNetwork *parent;
	SBList<std::shared_ptr<SBStr>> mMasterRooms = SBList<std::shared_ptr<SBStr>>();

	RakNet::RoomsPluginFunc *lastFunc;
	RakNet::JoinedRoomResult joinedRoom;

	//important:
	void SearchByFilter_Callback(const RakNet::SystemAddress& senderAddress, RakNet::SearchByFilter_Func* callResult) override {
		//Get room list
		//printf("Found %i rooms\n", callResult->roomsOutput.Size());
		if(mMasterRooms.GetNumberOfElements() != 0){
			mMasterRooms.Clear();
		}

		for (unsigned int i = 0; i < callResult->roomsOutput.Size(); i++) {
			mMasterRooms.Add(std::make_shared<SBStr>(callResult->roomsOutput[i]->GetProperty(DefaultRoomColumns::TC_ROOM_NAME)->c));
		}

		lastFunc = callResult;

		//parent->SetRoomList(callResult->roomsOutput.Size(), name);
	}
	void CreateRoom_Callback(const RakNet::SystemAddress& senderAddress, RakNet::CreateRoom_Func* callResult) override {
		(void)senderAddress; callResult->PrintResult(); lastFunc = callResult;
	}
	void EnterRoom_Callback(const RakNet::SystemAddress& senderAddress, RakNet::EnterRoom_Func* callResult) override {
		(void)senderAddress;
		callResult->PrintResult();

		joinedRoom = callResult->joinedRoomResult;

		lastFunc = callResult;
	}


	// Results of calls
	void JoinByFilter_Callback(const RakNet::SystemAddress& senderAddress, RakNet::JoinByFilter_Func* callResult) override {
		(void)senderAddress;
		callResult->PrintResult();
		lastFunc = callResult;
	}
	void LeaveRoom_Callback(const RakNet::SystemAddress& senderAddress, RakNet::LeaveRoom_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); lastFunc = callResult; }
	void GetInvitesToParticipant_Callback(const RakNet::SystemAddress& senderAddress, RakNet::GetInvitesToParticipant_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); lastFunc = callResult; }
	void SendInvite_Callback(const RakNet::SystemAddress& senderAddress, RakNet::SendInvite_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void AcceptInvite_Callback(const RakNet::SystemAddress& senderAddress, RakNet::AcceptInvite_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void StartSpectating_Callback(const RakNet::SystemAddress& senderAddress, RakNet::StartSpectating_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void StopSpectating_Callback(const RakNet::SystemAddress& senderAddress, RakNet::StopSpectating_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void GrantModerator_Callback(const RakNet::SystemAddress& senderAddress, RakNet::GrantModerator_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void ChangeSlotCounts_Callback(const RakNet::SystemAddress& senderAddress, RakNet::ChangeSlotCounts_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void SetCustomRoomProperties_Callback(const RakNet::SystemAddress& senderAddress, RakNet::SetCustomRoomProperties_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void GetRoomProperties_Callback(const RakNet::SystemAddress& senderAddress, RakNet::GetRoomProperties_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void ChangeRoomName_Callback(const RakNet::SystemAddress& senderAddress, RakNet::ChangeRoomName_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void SetHiddenFromSearches_Callback(const RakNet::SystemAddress& senderAddress, RakNet::SetHiddenFromSearches_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void SetDestroyOnModeratorLeave_Callback(const RakNet::SystemAddress& senderAddress, RakNet::SetDestroyOnModeratorLeave_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void SetReadyStatus_Callback(const RakNet::SystemAddress& senderAddress, RakNet::SetReadyStatus_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void GetReadyStatus_Callback(const RakNet::SystemAddress& senderAddress, RakNet::GetReadyStatus_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void SetRoomLockState_Callback(const RakNet::SystemAddress& senderAddress, RakNet::SetRoomLockState_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void GetRoomLockState_Callback(const RakNet::SystemAddress& senderAddress, RakNet::GetRoomLockState_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void AreAllMembersReady_Callback(const RakNet::SystemAddress& senderAddress, RakNet::AreAllMembersReady_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void KickMember_Callback(const RakNet::SystemAddress& senderAddress, RakNet::KickMember_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void UnbanMember_Callback(const RakNet::SystemAddress& senderAddress, RakNet::UnbanMember_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void GetBanReason_Callback(const RakNet::SystemAddress& senderAddress, RakNet::GetBanReason_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void AddUserToQuickJoin_Callback(const RakNet::SystemAddress& senderAddress, RakNet::AddUserToQuickJoin_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void RemoveUserFromQuickJoin_Callback(const RakNet::SystemAddress& senderAddress, RakNet::RemoveUserFromQuickJoin_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void IsInQuickJoin_Callback(const RakNet::SystemAddress& senderAddress, RakNet::IsInQuickJoin_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void ChangeHandle_Callback(const RakNet::SystemAddress& senderAddress, RakNet::ChangeHandle_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	void Chat_Callback(const RakNet::SystemAddress& senderAddress, RakNet::Chat_Func* callResult) override { (void)senderAddress; callResult->PrintResult(); }
	// Notifications due to other room members
	void QuickJoinExpired_Callback(const RakNet::SystemAddress& senderAddress, RakNet::QuickJoinExpired_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void QuickJoinEnteredRoom_Callback(const RakNet::SystemAddress& senderAddress, RakNet::QuickJoinEnteredRoom_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void RoomMemberStartedSpectating_Callback(const RakNet::SystemAddress& senderAddress, RakNet::RoomMemberStartedSpectating_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void RoomMemberStoppedSpectating_Callback(const RakNet::SystemAddress& senderAddress, RakNet::RoomMemberStoppedSpectating_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void ModeratorChanged_Callback(const RakNet::SystemAddress& senderAddress, RakNet::ModeratorChanged_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void SlotCountsSet_Callback(const RakNet::SystemAddress& senderAddress, RakNet::SlotCountsSet_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void CustomRoomPropertiesSet_Callback(const RakNet::SystemAddress& senderAddress, RakNet::CustomRoomPropertiesSet_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void RoomNameSet_Callback(const RakNet::SystemAddress& senderAddress, RakNet::RoomNameSet_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void HiddenFromSearchesSet_Callback(const RakNet::SystemAddress& senderAddress, RakNet::HiddenFromSearchesSet_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void RoomMemberReadyStatusSet_Callback(const RakNet::SystemAddress& senderAddress, RakNet::RoomMemberReadyStatusSet_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void RoomLockStateSet_Callback(const RakNet::SystemAddress& senderAddress, RakNet::RoomLockStateSet_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void RoomMemberKicked_Callback(const RakNet::SystemAddress& senderAddress, RakNet::RoomMemberKicked_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void RoomMemberHandleSet_Callback(const RakNet::SystemAddress& senderAddress, RakNet::RoomMemberHandleSet_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void RoomMemberLeftRoom_Callback(const RakNet::SystemAddress& senderAddress, RakNet::RoomMemberLeftRoom_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void RoomMemberJoinedRoom_Callback(const RakNet::SystemAddress& senderAddress, RakNet::RoomMemberJoinedRoom_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void RoomInvitationSent_Callback(const RakNet::SystemAddress& senderAddress, RakNet::RoomInvitationSent_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void RoomInvitationWithdrawn_Callback(const RakNet::SystemAddress& senderAddress, RakNet::RoomInvitationWithdrawn_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void RoomDestroyedOnModeratorLeft_Callback(const RakNet::SystemAddress& senderAddress, RakNet::RoomDestroyedOnModeratorLeft_Notification* notification) override { (void)senderAddress; notification->PrintResult(); }
	void Chat_Callback(const RakNet::SystemAddress& senderAddress, RakNet::Chat_Notification* notification) override { (void)senderAddress; notification->PrintResult(); printf("Chat=%s\nFiltered=%s\n", notification->chatMessage.C_String(), notification->filteredChatMessage.C_String()); }
};

#undef printf
#undef AT_Log
