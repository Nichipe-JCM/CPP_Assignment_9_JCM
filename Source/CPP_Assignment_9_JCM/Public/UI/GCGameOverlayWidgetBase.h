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
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnRoomPhaseChanged(ERoomPhase NewPhase);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnMiniGameTypeChanged(EMiniGameType NewGameType);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnRecruitInfoUpdated(const FRecruitInfo& NewRecruitInfo);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnWordleTurnLogsUpdated(const TArray<FWordleTurnResult>& TurnLogs);
};
