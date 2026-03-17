// Copyright 2026 Joo CheolMin. All Rights Reserved
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GCPlayerState.generated.h"

UCLASS()
class CPP_ASSIGNMENT_9_JCM_API AGCPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AGCPlayerState();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	void SetChatNickname(const FString& NewNickname) {ChatNickname = NewNickname;}
	void SetIsMinigameParticipant(bool bNewValue) {bIsMinigameParticipant = bNewValue;}
	void SetCanSendNormalChat(bool bNewValue) {bCanSendNormalChat = bNewValue;}
	void SetHasClearedCurrentGame(bool bNewValue) {bHasClearedCurrentGame = bNewValue;}
	void SetUsedTurnCount(int32 NewCount) {UsedTurnCount = NewCount;}
	void ResetMiniGamePlayerState();
	
public:
	FString GetChatNickname() const {return ChatNickname;}
	bool IsMinigameParticipant() const {return bIsMinigameParticipant;}
	bool CanSendNormalChat() const {return bCanSendNormalChat;}
	bool HasClearedCurrentGame() const {return bHasClearedCurrentGame;}
	int32 GetUsedTurnCount() const {return UsedTurnCount;}
	
protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	FString ChatNickname;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bIsMinigameParticipant = false;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bCanSendNormalChat = true;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bHasClearedCurrentGame = false;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	int32 UsedTurnCount = 0;
};
