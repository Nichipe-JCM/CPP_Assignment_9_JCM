// Copyright 2026 Joo CheolMin. All Rights Reserved
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GCPlayerController.generated.h"

class UGCNicknameEntryWidgetBase;
class UGCChatRoomWidgetBase;
class UGCGameOverlayWidgetBase;

UCLASS()
class CPP_ASSIGNMENT_9_JCM_API AGCPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AGCPlayerController();
	
protected:
	virtual void BeginPlay() override;
	void CreateInitialUI();
	
public:
	UFUNCTION(Server, Reliable)
	void ServerSubmitNickname(const FString& Nickname);
	
	UFUNCTION(Server, Reliable)
	void ServerSubmitChatInput(const FString& InputText);
	
	UFUNCTION(Client, Reliable)
	void ClientReceivePrivateTurnResult(const FString& ResultText);
	
	UFUNCTION(Client, Reliable)
	void ClientNotifyTurnStarted(float InTurnTimeLimit);
	
	UFUNCTION(Client, Reliable)
	void ClientNotifyTurnTimedOut();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGCNicknameEntryWidgetBase> NicknameEntryWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGCChatRoomWidgetBase> ChatRoomWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGCGameOverlayWidgetBase> GameOverlayWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UGCNicknameEntryWidgetBase> NicknameEntryWidgetInstance;
	
	UPROPERTY()
	TObjectPtr<UGCChatRoomWidgetBase> ChatRoomWidgetInstance;
	
	UPROPERTY()
	TObjectPtr<UGCGameOverlayWidgetBase> GameOverlayWidgetInstance;
};
