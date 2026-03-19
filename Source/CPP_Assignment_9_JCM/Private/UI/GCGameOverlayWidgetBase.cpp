// Copyright 2026 Joo CheolMin. All Rights Reserved

#include "CPP_Assignment_9_JCM/Public/UI/GCGameOverlayWidgetBase.h"

#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "CPP_Assignment_9_JCM/Public/Game/GCGameState.h"
#include "CPP_Assignment_9_JCM/Public/Player/GCPlayerState.h"

void UGCGameOverlayWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(PhaseText))
	{
		PhaseText->SetText(FText::FromString(TEXT("Phase : Lobby")));
	}

	if (IsValid(MiniGameText))
	{
		MiniGameText->SetText(FText::FromString(TEXT("Game : None")));
	}

	if (IsValid(RecruitText))
	{
		RecruitText->SetText(FText::FromString(TEXT("Recruiting : No")));
	}

	if (IsValid(TurnStateText))
	{
		TurnStateText->SetText(FText::FromString(TEXT("Turn State : Idle")));
	}

	if (IsValid(TurnTimerText))
	{
		TurnTimerText->SetText(FText::FromString(TEXT("Time Left : 0.0")));
	}

	if (IsValid(PrivateResultText))
	{
		ClearPrivateResultText();
	}

	if (IsValid(SummaryText))
	{
		SummaryText->SetText(FText::GetEmpty());
	}

	RefreshFromGameState();
}

void UGCGameOverlayWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	RefreshFromGameState();
	UpdateRecruitCountdown(InDeltaTime);
	UpdateTurnCountdown(InDeltaTime);
}

void UGCGameOverlayWidgetBase::NotifyPrivateTurnResult(const FString& ResultText)
{
	AppendPrivateResultText(ResultText);
}

void UGCGameOverlayWidgetBase::NotifyTurnStarted(float InTurnTimeLimit)
{
	bTurnCountdownActive = true;
	LocalTurnRemainingTime = InTurnTimeLimit;

	if (IsValid(TurnStateText))
	{
		TurnStateText->SetText(FText::FromString(TEXT("Turn State : Your Turn")));
	}

	UpdateTurnTimerText();
}

void UGCGameOverlayWidgetBase::NotifyTurnEnded()
{
	bTurnCountdownActive = false;
	LocalTurnRemainingTime = 0.0f;

	if (IsValid(TurnStateText))
	{
		TurnStateText->SetText(FText::FromString(TEXT("Turn State : Waiting")));
	}

	UpdateTurnTimerText();
}

void UGCGameOverlayWidgetBase::NotifyTurnTimedOut()
{
	bTurnCountdownActive = false;
	LocalTurnRemainingTime = 0.0f;

	if (IsValid(TurnStateText))
	{
		TurnStateText->SetText(FText::FromString(TEXT("Turn State : Timed Out")));
	}

	UpdateTurnTimerText();
	AppendPrivateResultText(TEXT("시간 초과"));
}

void UGCGameOverlayWidgetBase::ResetPrivateGameRecords()
{
	bTurnCountdownActive = false;
	LocalTurnRemainingTime = 0.0f;

	if (IsValid(TurnStateText))
	{
		TurnStateText->SetText(FText::FromString(TEXT("Turn State : Idle")));
	}

	UpdateTurnTimerText();
	ClearPrivateResultText();
}

void UGCGameOverlayWidgetBase::RefreshFromGameState()
{
	AGCGameState* GCGS = GetWorld() ? GetWorld()->GetGameState<AGCGameState>() : nullptr;
	if (!IsValid(GCGS)) return;
	
	const ERoomPhase NewPhase = GCGS->GetCurrentRoomPhase();
	if (CachedRoomPhase != NewPhase)
	{
		const ERoomPhase PreviousPhase = CachedRoomPhase;
		CachedRoomPhase = NewPhase;
		UpdatePhaseText(NewPhase);

		if (PreviousPhase != ERoomPhase::Playing && NewPhase == ERoomPhase::Playing)
		{
			ClearPrivateResultText();
		}

		if (NewPhase != ERoomPhase::Playing)
		{
			bTurnCountdownActive = false;
			LocalTurnRemainingTime = 0.0f;
			UpdateTurnTimerText();

			if (IsValid(TurnStateText))
			{
				TurnStateText->SetText(FText::FromString(TEXT("Turn State : Idle")));
			}
		}
	}

	const EMiniGameType NewGameType = GCGS->GetCurrentMiniGameType();
	if (CachedMiniGameType != NewGameType)
	{
		CachedMiniGameType = NewGameType;
		UpdateMiniGameText(NewGameType);
	}

	const FRecruitInfo& NewRecruitInfo = GCGS->GetRecruitInfo();
	if (CachedRecruitInfo.TargetGame != NewRecruitInfo.TargetGame ||
		CachedRecruitInfo.bIsRecruiting != NewRecruitInfo.bIsRecruiting ||
		!FMath::IsNearlyEqual(CachedRecruitInfo.RemainingTime, NewRecruitInfo.RemainingTime))
	{
		CachedRecruitInfo = NewRecruitInfo;
		LocalRecruitRemainingTime = NewRecruitInfo.RemainingTime;
		bRecruitCountdownActive = NewRecruitInfo.bIsRecruiting;
		UpdateRecruitText();
	}

	const float NewTurnRemainingTime = GCGS->GetTurnRemainingTime();
	const bool bShouldShowTurnCountdown = NewPhase == ERoomPhase::Playing && NewTurnRemainingTime > 0.0f;
	if (!FMath::IsNearlyEqual(LocalTurnRemainingTime, NewTurnRemainingTime) || bTurnCountdownActive != bShouldShowTurnCountdown)
	{
		LocalTurnRemainingTime = NewTurnRemainingTime;
		bTurnCountdownActive = bShouldShowTurnCountdown;
		UpdateTurnTimerText();
	}

	AGCPlayerState* NewTurnPlayer = GCGS->GetCurrentTurnPlayer();
	if (CachedTurnPlayer != NewTurnPlayer)
	{
		CachedTurnPlayer = NewTurnPlayer;

		const AGCPlayerState* LocalPlayerState = GetOwningPlayerState<AGCPlayerState>();
		const bool bIsLocalPlayersTurn =
			NewPhase == ERoomPhase::Playing &&
			IsValid(LocalPlayerState) &&
			LocalPlayerState == NewTurnPlayer;

		if (!bIsLocalPlayersTurn)
		{
			if (IsValid(TurnStateText))
			{
				TurnStateText->SetText(
					FText::FromString(
						NewPhase == ERoomPhase::Playing ? TEXT("Turn State : Waiting") : TEXT("Turn State : Idle")
					)
				);
			}
		}
	}

	const TArray<FString>& SummaryLines = GCGS->GetPublicTurnSummaryLines();
	if (CachedSummaryCount != SummaryLines.Num())
	{
		CachedSummaryCount = SummaryLines.Num();
		UpdateSummaryText(SummaryLines);
	}
}

void UGCGameOverlayWidgetBase::UpdateRecruitCountdown(float InDeltaTime)
{
	if (!bRecruitCountdownActive) return;

	if (LocalRecruitRemainingTime <= 0.0f)
	{
		bRecruitCountdownActive = false;
		UpdateRecruitText();
	}
}

void UGCGameOverlayWidgetBase::UpdateTurnCountdown(float InDeltaTime)
{
	if (!bTurnCountdownActive) return;

	if (LocalTurnRemainingTime <= 0.0f)
	{
		bTurnCountdownActive = false;
		UpdateTurnTimerText();
	}
}

void UGCGameOverlayWidgetBase::UpdatePhaseText(ERoomPhase NewPhase)
{
	if (!IsValid(PhaseText)) return;
	PhaseText->SetText(
		FText::FromString(FString::Printf(TEXT("Phase : %s"), *MakeRoomPhaseString(NewPhase)))
	);
}

void UGCGameOverlayWidgetBase::UpdateMiniGameText(EMiniGameType NewGameType)
{
	if (!IsValid(MiniGameText)) return;

	MiniGameText->SetText(
		FText::FromString(FString::Printf(TEXT("Game : %s"), *MakeMiniGameTypeString(NewGameType)))
	);
}

void UGCGameOverlayWidgetBase::UpdateRecruitText()
{
	if (!IsValid(RecruitText)) return;

	if (!CachedRecruitInfo.bIsRecruiting)
	{
		RecruitText->SetText(FText::FromString(TEXT("Recruiting : No")));
		return;
	}

	RecruitText->SetText(
		FText::FromString(
			FString::Printf(
				TEXT("Recruiting : Yes / Game : %s / Time : %.1f"),
				*MakeMiniGameTypeString(CachedRecruitInfo.TargetGame),
				LocalRecruitRemainingTime
			)
		)
	);
}

void UGCGameOverlayWidgetBase::UpdateSummaryText(const TArray<FString>& SummaryLines)
{
	if (!IsValid(SummaryText))
	{
		return;
	}

	SummaryText->SetText(FText::FromString(BuildSummaryString(SummaryLines)));

	if (IsValid(SummaryScrollBox))
	{
		SummaryScrollBox->ScrollToEnd();
	}
}

void UGCGameOverlayWidgetBase::UpdateTurnTimerText()
{
	if (!IsValid(TurnTimerText))
	{
		return;
	}

	TurnTimerText->SetText(
		FText::FromString(FString::Printf(TEXT("Time Left : %.1f"), LocalTurnRemainingTime))
	);
}

void UGCGameOverlayWidgetBase::UpdatePrivateResultText(const FString& NewText)
{
	if (!IsValid(PrivateResultText))
	{
		return;
	}

	if (NewText.IsEmpty())
	{
		PrivateResultText->SetText(FText::FromString(TEXT("Private Result :")));
		return;
	}

	PrivateResultText->SetText(
		FText::FromString(FString::Printf(TEXT("Private Result :\n%s"), *NewText))
	);
}

void UGCGameOverlayWidgetBase::AppendPrivateResultText(const FString& NewText)
{
	if (NewText.IsEmpty())
	{
		return;
	}

	PrivateResultHistory.Add(NewText);

	constexpr int32 MaxPrivateResultCount = 12;
	if (PrivateResultHistory.Num() > MaxPrivateResultCount)
	{
		PrivateResultHistory.RemoveAt(0, PrivateResultHistory.Num() - MaxPrivateResultCount);
	}

	UpdatePrivateResultText(BuildPrivateResultString());
}

void UGCGameOverlayWidgetBase::ClearPrivateResultText()
{
	PrivateResultHistory.Empty();
	UpdatePrivateResultText(TEXT(""));
}

FString UGCGameOverlayWidgetBase::MakeRoomPhaseString(ERoomPhase InPhase) const
{
	switch (InPhase)
	{
	case ERoomPhase::Lobby:
		return TEXT("Lobby");

	case ERoomPhase::SelectingGame:
		return TEXT("Selecting");

	case ERoomPhase::Recruiting:
		return TEXT("Recruiting");

	case ERoomPhase::Playing:
		return TEXT("Playing");

	case ERoomPhase::Ending:
		return TEXT("Ending");

	default:
		return TEXT("Unknown");
	}
}

FString UGCGameOverlayWidgetBase::MakeMiniGameTypeString(EMiniGameType InType) const
{
	switch (InType)
	{
	case EMiniGameType::None:
		return TEXT("None");

	case EMiniGameType::Wordle:
		return TEXT("Wordle");

	case EMiniGameType::NumberBaseball:
		return TEXT("Number Baseball");

	default:
		return TEXT("Unknown");
	}
}

FString UGCGameOverlayWidgetBase::BuildSummaryString(const TArray<FString>& SummaryLines) const
{
	FString CombinedText;

	for (const FString& Line : SummaryLines)
	{
		CombinedText += Line;
		CombinedText += TEXT("\n");
	}

	return CombinedText;
}

FString UGCGameOverlayWidgetBase::BuildPrivateResultString() const
{
	FString CombinedText;

	for (int32 Index = 0; Index < PrivateResultHistory.Num(); ++Index)
	{
		CombinedText += PrivateResultHistory[Index];

		if (Index + 1 < PrivateResultHistory.Num())
		{
			CombinedText += TEXT("\n");
		}
	}

	return CombinedText;
}

