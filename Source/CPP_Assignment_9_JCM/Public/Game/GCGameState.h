// Copyright 2026 Joo CheolMin. All Rights Reserved
#pragma once

#include "CoreMinimal.h"
#include "Core/ChatTypes.h"
#include "GameFramework/GameState.h"
#include "GCGameState.generated.h"

class AGCPlayerState;

UCLASS()
class CPP_ASSIGNMENT_9_JCM_API AGCGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	AGCGameState();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	void AddChatMessage(const FChatMessageData& NewMessage);
	void ClearChatMessages();
	void ClearPublicTurnSummary();
	
public:
	void SetCurrentRoomPhase(ERoomPhase NewPhase) {CurrentRoomPhase = NewPhase;}
	void SetCurrentMiniGameType(EMiniGameType NewMiniGameType) {CurrentMiniGameType = NewMiniGameType;}
	void SetRecruitInfo(const FRecruitInfo& NewRecruitInfo) {RecruitInfo = NewRecruitInfo;}
	void SetCurrentRoundIndex(int32 NewRoundIndex) {CurrentRoundIndex = NewRoundIndex;}
	void SetCurrentTurnPlayer(AGCPlayerState* NewTurnPlayer) {CurrentTurnPlayer = NewTurnPlayer;}
	void SetCurrentParticipants(const TArray<TObjectPtr<AGCPlayerState>>& NewParticipants) {CurrentParticipants = NewParticipants;}
	void AddPublicTurnSummaryLine(const FString& NewLine) {PublicTurnSummaryLines.Add(NewLine);}	
public:
	ERoomPhase GetCurrentRoomPhase() const {return CurrentRoomPhase;}
	EMiniGameType GetCurrentMiniGameType() const {return CurrentMiniGameType;}
	const FRecruitInfo& GetRecruitInfo() const {return RecruitInfo;}
	int32 GetCurrentRoundIndex() const {return CurrentRoundIndex;}
	AGCPlayerState* GetCurrentTurnPlayer() const {return CurrentTurnPlayer;}
	const TArray<FChatMessageData>& GetChatMessages() const {return ChatMessages;}
	const TArray<TObjectPtr<AGCPlayerState>>& GetCurrentParticipants() const {return CurrentParticipants;}
	const TArray<FString>& GetPublicTurnSummaryLines() const {return PublicTurnSummaryLines;}
	
protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Room")
	ERoomPhase CurrentRoomPhase = ERoomPhase::Lobby;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Room")
	EMiniGameType CurrentMiniGameType = EMiniGameType::None;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Room")
	FRecruitInfo RecruitInfo;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Turn")
	int32 CurrentRoundIndex = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Turn")
	TObjectPtr<AGCPlayerState> CurrentTurnPlayer = nullptr;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Chat")
	TArray<FChatMessageData> ChatMessages;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Game")
	TArray<TObjectPtr<AGCPlayerState>> CurrentParticipants;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Game")
	TArray<FString> PublicTurnSummaryLines;
};
