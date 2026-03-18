// Copyright 2026 Joo CheolMin. All Rights Reserved
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/ChatTypes.h"
#include "GCGameOverlayWidgetBase.generated.h"

UCLASS()
class CPP_ASSIGNMENT_9_JCM_API UGCGameOverlayWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnRoomPhaseChanged(ERoomPhase NewPhase);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnMiniGameTypeChanged(EMiniGameType NewGameType);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnRecruitInfoUpdated(const FRecruitInfo& NewRecruitInfo);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnPublicTurnSummaryUpdated(const TArray<FString>& TurnSummaryLines);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnPrivateTurnResultReceived(const FString& ResultText);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnTurnStarted(float InTurnTimeLimit);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnTurnTimedOut();

public:
	void NotifyPrivateTurnResult(const FString& ResultText);
	void NotifyTurnStarted(float InTurnTimeLimit);
	void NotifyTurnTimedOut();

protected:
	void RefreshFromGameState();

protected:
	ERoomPhase CachedRoomPhase = ERoomPhase::Lobby;
	EMiniGameType CachedMiniGameType = EMiniGameType::None;
	FRecruitInfo CachedRecruitInfo;
	int32 CachedSummaryCount = INDEX_NONE;
	
};
