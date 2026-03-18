// Copyright 2026 Joo CheolMin. All Rights Reserved

#include "CPP_Assignment_9_JCM/Public/UI/GCGameOverlayWidgetBase.h"

#include "CPP_Assignment_9_JCM/Public/Game/GCGameState.h"

void UGCGameOverlayWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	
	RefreshFromGameState();
}

void UGCGameOverlayWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	RefreshFromGameState();
}

void UGCGameOverlayWidgetBase::NotifyPrivateTurnResult(const FString& ResultText)
{
	BP_OnPrivateTurnResultReceived(ResultText);
}

void UGCGameOverlayWidgetBase::NotifyTurnStarted(float InTurnTimeLimit)
{
	BP_OnTurnStarted(InTurnTimeLimit);
}

void UGCGameOverlayWidgetBase::NotifyTurnTimedOut()
{
	BP_OnTurnTimedOut();
}

void UGCGameOverlayWidgetBase::RefreshFromGameState()
{
	AGCGameState* GCGS = GetWorld() ? GetWorld()->GetGameState<AGCGameState>() : nullptr;
	if (!IsValid(GCGS)) return;
	
	const ERoomPhase NewPhase = GCGS->GetCurrentRoomPhase();
	if (CachedRoomPhase != NewPhase)
	{
		CachedRoomPhase = NewPhase;
		BP_OnRoomPhaseChanged(NewPhase);
	}

	const EMiniGameType NewGameType = GCGS->GetCurrentMiniGameType();
	if (CachedMiniGameType != NewGameType)
	{
		CachedMiniGameType = NewGameType;
		BP_OnMiniGameTypeChanged(NewGameType);
	}

	const FRecruitInfo& NewRecruitInfo = GCGS->GetRecruitInfo();
	if (CachedRecruitInfo.TargetGame != NewRecruitInfo.TargetGame ||
		CachedRecruitInfo.bIsRecruiting != NewRecruitInfo.bIsRecruiting ||
		!FMath::IsNearlyEqual(CachedRecruitInfo.RemainingTime, NewRecruitInfo.RemainingTime))
	{
		CachedRecruitInfo = NewRecruitInfo;
		BP_OnRecruitInfoUpdated(NewRecruitInfo);
	}

	const TArray<FString>& SummaryLines = GCGS->GetPublicTurnSummaryLines();
	if (CachedSummaryCount != SummaryLines.Num())
	{
		CachedSummaryCount = SummaryLines.Num();
		BP_OnPublicTurnSummaryUpdated(SummaryLines);
	}
}
