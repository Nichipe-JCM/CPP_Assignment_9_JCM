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
	ERoomPhase GetCurrentRoundPhase() const;
	EMiniGameType GetCurrentMiniGameType() const;
	const FRecruitInfo& GetRecruitInfo() const;
	int32 GetCurrentRoundIndex() const;
	AGCPlayerState* GetCurrentTurnPlayer() const;
	const TArray<FChatMessageData>& GetChatMessages() const;
	const TArray<TObjectPtr<AGCPlayerState>>& GetCurrentParticipants() const;
	const TArray<FString>& GetPublicTurnSummaryLines() const;
	
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
