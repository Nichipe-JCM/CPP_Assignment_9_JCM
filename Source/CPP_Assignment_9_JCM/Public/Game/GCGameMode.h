// Copyright 2026 Joo CheolMin. All Rights Reserved
#pragma once

#include "CoreMinimal.h"
#include "Core/ChatTypes.h"
#include "GameFramework/GameMode.h"
#include "GCGameMode.generated.h"

class AGCPlayerController;
class AGCPlayerState;
class AGCGameState;

UCLASS()
class CPP_ASSIGNMENT_9_JCM_API AGCGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AGCGameMode();
	
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* ExitingController) override;
	
public:
	void HandleNicknameSubmit(AGCPlayerController* SenderPC, const FString& Nickname);
	void HandleChatInput(AGCPlayerController* SenderPC, const FString& InputText);
	
protected:
	void ProcessCommand(AGCPlayerController* SenderPC, const FString& CommandText);
	void ProcessNormalChat(AGCPlayerController* SenderPC, const FString& ChatText);
	void BroadcastSystemMessage(const FString& Message);
	void ResetRoomState();
	
	void StartGameSelection(AGCPlayerController* SenderPC);
	void SelectMiniGame(EMiniGameType InGameType);
	void StartRecruitment();
	void FinishRecruitment();
	void AddParticipant(AGCPlayerState* InPlayerState);
	bool IsParticipant(const AGCPlayerState* InPlayerState) const;
	void ClearParticipants();
	
	void StartMiniGame();
	void StartTurn();
	void EndTurnByInput(AGCPlayerController* SenderPC, const FString& InputText);
	void EndTurnByTimeout();
	void AdvanceTurn();
	void FinishRound();
	void EndCurrentGame(const FString& EndMessage = TEXT("현재 게임을 종료합니다."));
	
	bool IsCurrentTurnPlayer(const AGCPlayerController* SenderPC) const;
	AGCPlayerState* GetCurrentTurnPlayerState() const;
	int32 GetMaxTurnCountForCurrentGame() const;
	
	void LoadWordleWordLists();
	bool IsAllowedWord(const FString& Word) const;
	FString PickRandomWordleAnswer() const;
	void StartWordleGame();
	bool ValidateWordleGuess(const FString& GuessWord) const;
	void ResolveWordleTurn(AGCPlayerController* SenderPC, const FString& GuessWord);
	
	void StartNumberBaseballGame();
	FString GenerateAnswerNumber() const;
	bool IsGuessNumberString(const FString& InNumberString) const;
	void ResolveNumberBaseballTurn(AGCPlayerController* SenderPC, const FString& GuessNumberString);
	
	void SyncGameState();
	
private:
	TArray<EWordleLetterState> JudgeWordleStates(const FString& Answer, const FString& Guess) const;
	FString MakeWordlePrivateResultText(const FString& GuessWord, const TArray<EWordleLetterState>& States) const;
	FString MakeWordlePublicSummary(const FString& PlayerName, const TArray<EWordleLetterState>& States) const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Wordle|Data")
	FString AnswerWordsJsonRelativePath;
	
	UPROPERTY(EditDefaultsOnly, Category = "Wordle|Data")
	FString AllowedWordsJsonRelativePath;
	
	UPROPERTY()
	TArray<FString> AnswerWords;
	
	TSet<FString> AllowedWords;
	
	UPROPERTY()
	FString CurrentWordleAnswer;
	
	UPROPERTY()
	FString AnswerNumberString;
	
	UPROPERTY()
	TArray<TObjectPtr<AGCPlayerState>> ActiveParticipants;
	
	UPROPERTY(EditDefaultsOnly, Category = "Turn")
	float TurnTimeLimit = 15.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Turn")
	float RoundSummaryDisplayTime = 5.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Rule")
	int32 WordleMaxTurnCount = 6;
	
	UPROPERTY(EditDefaultsOnly, Category = "Rule")
	
	int32 NumberBaseballMaxTurnCount = 10;
	
	UPROPERTY()
	int32 CurrentRoundIndex = 0;
	
	UPROPERTY()
	int32 CurrentTurnPlayerIndex = INDEX_NONE;
	
	FTimerHandle RecruitTimerHandle;
	FTimerHandle TurnTimerHandle;
	FTimerHandle RoundSummaryTimerHandle;
};
