// Copyright 2026 Joo CheolMin. All Rights Reserved
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/ChatTypes.h"
#include "GCGameOverlayWidgetBase.generated.h"

class UTextBlock;
class UScrollBox;
class AGCPlayerState;

UCLASS()
class CPP_ASSIGNMENT_9_JCM_API UGCGameOverlayWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void NotifyPrivateTurnResult(const FString& ResultText);
	void NotifyTurnStarted(float InTurnTimeLimit);
	void NotifyTurnEnded();
	void NotifyTurnTimedOut();

protected:
	void RefreshFromGameState();
	void UpdateRecruitCountdown(float InDeltaTime);
	void UpdateTurnCountdown(float InDeltaTime);

	void UpdatePhaseText(ERoomPhase NewPhase);
	void UpdateMiniGameText(EMiniGameType NewGameType);
	void UpdateRecruitText();
	void UpdateSummaryText(const TArray<FString>& SummaryLines);
	void UpdateTurnTimerText();
	void UpdatePrivateResultText(const FString& NewText);
	void AppendPrivateResultText(const FString& NewText);
	void ClearPrivateResultText();

	FString MakeRoomPhaseString(ERoomPhase InPhase) const;
	FString MakeMiniGameTypeString(EMiniGameType InType) const;
	FString BuildSummaryString(const TArray<FString>& SummaryLines) const;
	FString BuildPrivateResultString() const;

protected:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* PhaseText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* MiniGameText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* RecruitText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TurnStateText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TurnTimerText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* PrivateResultText;

	UPROPERTY(meta=(BindWidget))
	UScrollBox* SummaryScrollBox;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* SummaryText;
	
protected:
	ERoomPhase CachedRoomPhase = ERoomPhase::Lobby;
	EMiniGameType CachedMiniGameType = EMiniGameType::None;
	FRecruitInfo CachedRecruitInfo;
	TObjectPtr<AGCPlayerState> CachedTurnPlayer = nullptr;
	int32 CachedSummaryCount = INDEX_NONE;
	TArray<FString> PrivateResultHistory;
	
	float LocalRecruitRemainingTime = 0.f;
	bool bRecruitCountdownActive = false;
	
	float LocalTurnRemainingTime = 0.f;
	bool bTurnCountdownActive = false;
};
