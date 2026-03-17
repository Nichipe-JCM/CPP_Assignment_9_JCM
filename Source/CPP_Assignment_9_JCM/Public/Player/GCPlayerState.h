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
	void SetChatNickname(const FString& NewNickname);
	void SetIsMinigameParticipant(bool bNewValue);
	void SetCanSendNormalChat(bool bNewValue);
	void SetHasClearedCurrentGame(bool bNewValue);
	void SetUsedTurnCount(int32 NewCount);
	void ResetMiniGamePlayerState();
	
public:
	FString GetChatNickname() const;
	bool IsMinigameParticipant() const;
	bool CanSendNormalChat() const;
	bool HasClearedCurrentGame() const;
	int32 GetUsedTurnCount() const;
	
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
