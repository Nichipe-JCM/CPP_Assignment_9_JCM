// Copyright 2026 Joo CheolMin. All Rights Reserved

#include "CPP_Assignment_9_JCM/Public/Game/GCGameMode.h"

#include "Game/GCGameState.h"
#include "Player/GCPlayerController.h"
#include "Player/GCPlayerState.h"

AGCGameMode::AGCGameMode()
{
	PlayerControllerClass = AGCPlayerController::StaticClass();
	PlayerStateClass = AGCPlayerState::StaticClass();
	GameStateClass = AGCGameState::StaticClass();
}

void AGCGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	LoadWordleWordLists();
	ResetRoomState();
}

void AGCGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	AGCPlayerController* NewGCPC = Cast<AGCPlayerController>(NewPlayer);
	if (!IsValid(NewGCPC)) return;
}

void AGCGameMode::Logout(AController* ExitingController)
{
	AGCPlayerState* ExitingPS = nullptr;
	
	if (IsValid(ExitingController))
	{
		ExitingPS = ExitingController->GetPlayerState<AGCPlayerState>();
	}
	
	FString ExitName = TEXT("Player");
	if (IsValid(ExitingPS) && !ExitingPS->GetChatNickname().IsEmpty())
	{
		ExitName = ExitingPS->GetChatNickname();
	}
	
	ActiveParticipants.Remove(ExitingPS);
	
	Super::Logout(ExitingController);
	
	AGCGameState* GCGameState = GetGameState<AGCGameState>();
	if (IsValid(GCGameState))
	{
		if (GCGameState->GetCurrentRoomPhase() == ERoomPhase::SelectingGame && ExitingPS == GameSelectionStarter)
		{
			BroadcastSystemMessage(TEXT("게임 선택 중이던 플레이어가 퇴장하여 선택이 취소되었습니다."));
			ResetRoomState();
			return;
		}

		if (GCGameState->GetCurrentRoomPhase() == ERoomPhase::Playing && ActiveParticipants.Num() <= 0)
		{
			BroadcastSystemMessage(TEXT("게임이 종료되었습니다. (참가자 부족)"));
			EndCurrentGame();
			return;
		}
		
		if (CurrentTurnPlayerIndex >= ActiveParticipants.Num())
		{
			CurrentTurnPlayerIndex = ActiveParticipants.Num() > 0 ? 0 : INDEX_NONE;
		}
	}
	
	SyncGameState();
	BroadcastSystemMessage(FString::Printf(TEXT("%s 님이 퇴장했습니다."), *ExitName));
}

void AGCGameMode::HandleNicknameSubmit(AGCPlayerController* SenderPC, const FString& Nickname)
{
	if (!IsValid(SenderPC)) return;
	
	AGCPlayerState* SenderPS = SenderPC->GetPlayerState<AGCPlayerState>();
	if (!IsValid(SenderPS)) return;
	
	FString FinalNickname = Nickname;
	FinalNickname.TrimStartAndEndInline();
	
	if (FinalNickname.IsEmpty())
	{
		FinalNickname = GenerateDefaultNickname();
	}

	if (IsNicknameAlreadyInUse(FinalNickname, SenderPS))
	{
		SenderPC->ClientHandleNicknameSubmitResult(false, TEXT("동일한 닉네임이 존재합니다"));
		return;
	}
	
	SenderPS->SetChatNickname(FinalNickname);
	
	BroadcastSystemMessage(FString::Printf(TEXT("%s 님이 입장했습니다."), *FinalNickname));
	SenderPC->ClientHandleNicknameSubmitResult(true, TEXT(""));
}

bool AGCGameMode::IsNicknameAlreadyInUse(const FString& Nickname, const AGCPlayerState* RequestPlayerState) const
{
	const FString TrimmedNickname = Nickname.TrimStartAndEnd();
	if (TrimmedNickname.IsEmpty())
	{
		return false;
	}

	const AGCGameState* GCGameState = GetGameState<AGCGameState>();
	if (!IsValid(GCGameState))
	{
		return false;
	}

	for (APlayerState* PlayerStateBase : GCGameState->PlayerArray)
	{
		const AGCPlayerState* OtherPlayerState = Cast<AGCPlayerState>(PlayerStateBase);
		if (!IsValid(OtherPlayerState) || OtherPlayerState == RequestPlayerState)
		{
			continue;
		}

		const FString OtherNickname = OtherPlayerState->GetChatNickname().TrimStartAndEnd();
		if (OtherNickname.IsEmpty())
		{
			continue;
		}

		if (OtherNickname.Equals(TrimmedNickname, ESearchCase::IgnoreCase))
		{
			return true;
		}
	}

	return false;
}

FString AGCGameMode::GenerateDefaultNickname()
{
	while (true)
	{
		const FString CandidateNickname = FString::Printf(TEXT("플레이어%d"), NextDefaultNicknameNumber++);

		if (!IsNicknameAlreadyInUse(CandidateNickname))
		{
			return CandidateNickname;
		}
	}
}

void AGCGameMode::HandleChatInput(AGCPlayerController* SenderPC, const FString& InputText)
{
	if (!IsValid(SenderPC)) return;
	
	AGCPlayerState* SenderPS = SenderPC->GetPlayerState<AGCPlayerState>();
	AGCGameState* GCGameState = GetGameState<AGCGameState>();
	
	if (!IsValid(SenderPS) || !IsValid(GCGameState)) return;
	
	FString FinalInput = InputText;
	FinalInput.TrimStartAndEndInline();
	
	if (FinalInput.IsEmpty()) return;
	
	const ERoomPhase CurrentPhase = GCGameState->GetCurrentRoomPhase();
	
	if (CurrentPhase == ERoomPhase::Playing && SenderPS->IsMinigameParticipant())
	{
		if (!IsCurrentTurnPlayer(SenderPC))
		{
			SenderPC->ClientReceivePrivateTurnResult(TEXT("지금은 당신의 턴이 아닙니다."));
			return;
		}

		EndTurnByInput(SenderPC, FinalInput);
		return;
	}

	if (FinalInput.StartsWith(TEXT("!")))
	{
		ProcessCommand(SenderPC, FinalInput);
		return;
	}

	ProcessNormalChat(SenderPC, FinalInput);
}

void AGCGameMode::ProcessCommand(AGCPlayerController* SenderPC, const FString& CommandText)
{
	if (!IsValid(SenderPC)) return;
	
	AGCPlayerState* SenderPS = SenderPC->GetPlayerState<AGCPlayerState>();
	AGCGameState* GCGameState = GetGameState<AGCGameState>();
	
	if (!IsValid(SenderPS) || !IsValid(GCGameState)) return;
	
	const ERoomPhase CurrentPhase = GCGameState->GetCurrentRoomPhase();

	if (CommandText.Equals(TEXT("!게임")))
	{
		if (CurrentPhase == ERoomPhase::SelectingGame)
		{
			SenderPC->ClientReceivePrivateTurnResult(TEXT("다른 플레이어가 현재 게임을 선택 중입니다."));
			return;
		}

		if (CurrentPhase != ERoomPhase::Lobby)
		{
			SenderPC->ClientReceivePrivateTurnResult(TEXT("지금은 게임 선택을 시작할 수 없습니다."));
			return;
		}

		StartGameSelection(SenderPC);
		return;
	}

	if (CommandText.Equals(TEXT("!워들")))
	{
		if (CurrentPhase != ERoomPhase::SelectingGame)
		{
			SenderPC->ClientReceivePrivateTurnResult(TEXT("먼저 !게임 으로 게임 선택 상태로 들어가세요."));
			return;
		}

		if (!IsGameSelectionStarter(SenderPS))
		{
			SenderPC->ClientReceivePrivateTurnResult(TEXT("게임 선택을 시작한 플레이어만 워들을 선택할 수 있습니다."));
			return;
		}

		SelectMiniGame(EMiniGameType::Wordle);
		AddParticipant(SenderPS);
		StartRecruitment();

		BroadcastSystemMessage(FString::Printf(TEXT("%s 님이 워들 모집을 시작했습니다. !참가 를 입력해 참가신청을 해보세요."), *SenderPS->GetChatNickname()));
		return;
	}

	if (CommandText.Equals(TEXT("!숫자야구")))
	{
		if (CurrentPhase != ERoomPhase::SelectingGame)
		{
			SenderPC->ClientReceivePrivateTurnResult(TEXT("먼저 !게임 으로 게임 선택 상태로 들어가세요."));
			return;
		}

		if (!IsGameSelectionStarter(SenderPS))
		{
			SenderPC->ClientReceivePrivateTurnResult(TEXT("게임 선택을 시작한 플레이어만 숫자야구를 선택할 수 있습니다."));
			return;
		}

		SelectMiniGame(EMiniGameType::NumberBaseball);
		AddParticipant(SenderPS);
		StartRecruitment();

		BroadcastSystemMessage(FString::Printf(TEXT("%s 님이 숫자야구 모집을 시작했습니다. !참가 를 입력해 참가신청을 해보세요."), *SenderPS->GetChatNickname()));
		return;
	}

	if (CommandText.Equals(TEXT("!참가")))
	{
		const FRecruitInfo& RecruitInfo = GCGameState->GetRecruitInfo();

		if (!RecruitInfo.bIsRecruiting)
		{
			SenderPC->ClientReceivePrivateTurnResult(TEXT("지금은 참가 모집 중이 아닙니다."));
			return;
		}

		if (IsParticipant(SenderPS))
		{
			SenderPC->ClientReceivePrivateTurnResult(TEXT("이미 참가한 상태입니다."));
			return;
		}

		AddParticipant(SenderPS);
		BroadcastSystemMessage(FString::Printf(TEXT("%s 님이 참가했습니다."), *SenderPS->GetChatNickname()));
		return;
	}

	SenderPC->ClientReceivePrivateTurnResult(TEXT("알 수 없는 명령어입니다."));
}

void AGCGameMode::ProcessNormalChat(AGCPlayerController* SenderPC, const FString& ChatText)
{
	if (!IsValid(SenderPC)) return;
	
	AGCPlayerState* SenderPS = SenderPC->GetPlayerState<AGCPlayerState>();
	AGCGameState* GCGameState = GetGameState<AGCGameState>();
	
	if (!IsValid(SenderPS) || !IsValid(GCGameState)) return;
	
	if (!SenderPS->CanSendNormalChat())
	{
		SenderPC->ClientReceivePrivateTurnResult(TEXT("지금은 일반 채팅을 보낼 수 없습니다."));
		return;
	}
	
	FChatMessageData NewMessage;
	NewMessage.SenderName = SenderPS->GetChatNickname();
	NewMessage.Message = ChatText;
	NewMessage.MessageType = EChatMessageType::User;
	
	GCGameState->AddChatMessage(NewMessage);
}

void AGCGameMode::BroadcastSystemMessage(const FString& Message)
{
	AGCGameState* GCGameState = GetGameState<AGCGameState>();
	
	if (!IsValid(GCGameState)) return;
	
	FChatMessageData NewMessage;
	NewMessage.SenderName = TEXT("System");
	NewMessage.Message = Message;
	NewMessage.MessageType = EChatMessageType::System;
	
	GCGameState->AddChatMessage(NewMessage);
}

void AGCGameMode::ResetRoomState()
{
	GetWorldTimerManager().ClearTimer(TurnTimerHandle);
	GetWorldTimerManager().ClearTimer(RecruitTimerHandle);
	GetWorldTimerManager().ClearTimer(RoundSummaryTimerHandle);
	
	CurrentRoundIndex = 0;
	CurrentTurnPlayerIndex = INDEX_NONE;
	AnswerNumberString.Empty();
	CurrentWordleAnswer.Empty();
	GameSelectionStarter = nullptr;
	
	ClearParticipants();
	
	AGCGameState* GCGameState = GetGameState<AGCGameState>();
	if (!IsValid(GCGameState)) return;
	
	FRecruitInfo NewRecruitInfo;
	GCGameState->SetCurrentRoomPhase(ERoomPhase::Lobby);
	GCGameState->SetCurrentMiniGameType(EMiniGameType::None);
	GCGameState->SetRecruitInfo(NewRecruitInfo);
	GCGameState->SetCurrentRoundIndex(0);
	GCGameState->SetCurrentTurnPlayer(nullptr);
	GCGameState->ClearPublicTurnSummary();

	SyncGameState();
}

void AGCGameMode::StartGameSelection(AGCPlayerController* SenderPC)
{
	if (!IsValid(SenderPC))	return;

	AGCGameState* GCGameState = GetGameState<AGCGameState>();
	if (!IsValid(GCGameState)) return;

	GCGameState->SetCurrentRoomPhase(ERoomPhase::SelectingGame);
	GameSelectionStarter = SenderPC->GetPlayerState<AGCPlayerState>();
	SyncGameState();

	SenderPC->ClientReceivePrivateTurnResult(TEXT("게임 선택을 시작합니다. !워들 또는 !숫자야구 를 입력하세요."));
}

void AGCGameMode::SelectMiniGame(EMiniGameType InGameType)
{
	AGCGameState* GCGameState = GetGameState<AGCGameState>();
	if (!IsValid(GCGameState)) return;

	GCGameState->SetCurrentMiniGameType(InGameType);
	SyncGameState();
}

void AGCGameMode::StartRecruitment()
{
	AGCGameState* GCGameState = GetGameState<AGCGameState>();
	if (!IsValid(GCGameState)) return;

	constexpr float RecruitDuration = 10.0f;
	constexpr float RecruitUpdateInterval = 0.1f;

	FRecruitInfo NewRecruitInfo;
	NewRecruitInfo.TargetGame = GCGameState->GetCurrentMiniGameType();
	NewRecruitInfo.bIsRecruiting = true;
	NewRecruitInfo.RemainingTime = RecruitDuration;

	GCGameState->SetRecruitInfo(NewRecruitInfo);
	GCGameState->SetCurrentRoomPhase(ERoomPhase::Recruiting);

	GetWorldTimerManager().ClearTimer(RecruitTimerHandle);
	GetWorldTimerManager().SetTimer(
		RecruitTimerHandle,
		FTimerDelegate::CreateLambda([this, RecruitUpdateInterval]()
		{
			AGCGameState* InnerGameState = GetGameState<AGCGameState>();
			if (!IsValid(InnerGameState))
			{
				GetWorldTimerManager().ClearTimer(RecruitTimerHandle);
				return;
			}

			FRecruitInfo RecruitInfo = InnerGameState->GetRecruitInfo();
			if (!RecruitInfo.bIsRecruiting)
			{
				GetWorldTimerManager().ClearTimer(RecruitTimerHandle);
				return;
			}

			RecruitInfo.RemainingTime = FMath::Max(0.0f, RecruitInfo.RemainingTime - RecruitUpdateInterval);
			InnerGameState->SetRecruitInfo(RecruitInfo);
			SyncGameState();

			if (RecruitInfo.RemainingTime <= KINDA_SMALL_NUMBER)
			{
				GetWorldTimerManager().ClearTimer(RecruitTimerHandle);
				FinishRecruitment();
			}
		}),
		RecruitUpdateInterval,
		true
	);

	SyncGameState();
}

void AGCGameMode::FinishRecruitment()
{
	GetWorldTimerManager().ClearTimer(RecruitTimerHandle);

	AGCGameState* GCGameState = GetGameState<AGCGameState>();
	if (!IsValid(GCGameState)) return;

	FRecruitInfo NewRecruitInfo = GCGameState->GetRecruitInfo();
	NewRecruitInfo.bIsRecruiting = false;
	NewRecruitInfo.RemainingTime = 0.0f;
	GCGameState->SetRecruitInfo(NewRecruitInfo);

	if (ActiveParticipants.Num() <= 0)
	{
		BroadcastSystemMessage(TEXT("참가자가 없어 모집을 종료합니다."));
		ResetRoomState();
		return;
	}

	BroadcastSystemMessage(TEXT("모집이 종료되었습니다."));
	StartMiniGame();
}

void AGCGameMode::AddParticipant(AGCPlayerState* InPlayerState)
{
	if (!IsValid(InPlayerState)) return;

	if (IsParticipant(InPlayerState)) return;

	ActiveParticipants.Add(InPlayerState);
	InPlayerState->SetIsMinigameParticipant(true);

	SyncGameState();
}

bool AGCGameMode::IsParticipant(const AGCPlayerState* InPlayerState) const
{
	if (!IsValid(InPlayerState)) return false;
	
	return ActiveParticipants.Contains(InPlayerState);
}

void AGCGameMode::ClearParticipants()
{
	for (AGCPlayerState* Participant : ActiveParticipants)
	{
		if (!IsValid(Participant)) continue;

		Participant->ResetMiniGamePlayerState();
	}

	ActiveParticipants.Empty();
}

void AGCGameMode::StartMiniGame()
{
	AGCGameState* GCGameState = GetGameState<AGCGameState>();
	if (!IsValid(GCGameState)) return;

	if (ActiveParticipants.Num() <= 0)
	{
		ResetRoomState();
		return;
	}

	const EMiniGameType CurrentGameType = GCGameState->GetCurrentMiniGameType();

	if (CurrentGameType == EMiniGameType::Wordle && AnswerWords.Num() <= 0)
	{
		BroadcastSystemMessage(TEXT("Wordle 데이터가 없어 게임을 시작할 수 없습니다."));
		ResetRoomState();
		return;
	}

	CurrentRoundIndex = 1;
	CurrentTurnPlayerIndex = 0;

	for (AGCPlayerState* Participant : ActiveParticipants)
	{
		if (!IsValid(Participant))
		{
			continue;
		}

		Participant->SetIsMinigameParticipant(true);
		Participant->SetCanSendNormalChat(false);
		Participant->SetHasClearedCurrentGame(false);
		Participant->SetUsedTurnCount(0);
	}

	GCGameState->SetCurrentRoomPhase(ERoomPhase::Playing);
	GCGameState->ClearPublicTurnSummary();

	switch (CurrentGameType)
	{
	case EMiniGameType::Wordle:
		StartWordleGame();
		break;

	case EMiniGameType::NumberBaseball:
		StartNumberBaseballGame();
		break;

	default:
		ResetRoomState();
		return;
	}

	SyncGameState();
	StartTurn();
}

void AGCGameMode::StartTurn()
{
	AGCPlayerState* TurnPlayerState = GetCurrentTurnPlayerState();
	if (!IsValid(TurnPlayerState)) return;

	AController* OwnerController = Cast<AController>(TurnPlayerState->GetOwner());
	AGCPlayerController* TurnPC = Cast<AGCPlayerController>(OwnerController);

	if (IsValid(TurnPC))
	{
		TurnPC->ClientNotifyTurnStarted(TurnTimeLimit);
	}

	SyncGameState();

	GetWorldTimerManager().ClearTimer(TurnTimerHandle);
	GetWorldTimerManager().SetTimer(
		TurnTimerHandle,
		this,
		&AGCGameMode::EndTurnByTimeout,
		TurnTimeLimit,
		false
	);
}

void AGCGameMode::EndTurnByInput(AGCPlayerController* SenderPC, const FString& InputText)
{
	if (!IsValid(SenderPC)) return;

	if (!IsCurrentTurnPlayer(SenderPC))
	{
		SenderPC->ClientReceivePrivateTurnResult(TEXT("지금은 당신의 턴이 아닙니다."));
		return;
	}

	AGCGameState* GCGameState = GetGameState<AGCGameState>();
	if (!IsValid(GCGameState)) return;
	if (GCGameState->GetCurrentRoomPhase() != ERoomPhase::Playing) return;
	
	const EMiniGameType CurrentGameType = GCGameState->GetCurrentMiniGameType();

	if (CurrentGameType == EMiniGameType::Wordle)
	{
		FString FinalGuess = InputText;
		FinalGuess.TrimStartAndEndInline();
		FinalGuess = FinalGuess.ToLower();

		if (!ValidateWordleGuess(FinalGuess))
		{
			SenderPC->ClientReceivePrivateTurnResult(TEXT("유효한 5글자 단어가 아닙니다."));
			return;
		}

		GetWorldTimerManager().ClearTimer(TurnTimerHandle);
		SenderPC->ClientNotifyTurnEnded();
		ResolveWordleTurn(SenderPC, FinalGuess);
	}
	else if (CurrentGameType == EMiniGameType::NumberBaseball)
	{
		FString FinalGuess = InputText;
		FinalGuess.TrimStartAndEndInline();

		if (!IsGuessNumberString(FinalGuess))
		{
			SenderPC->ClientReceivePrivateTurnResult(TEXT("중복 없는 3자리 숫자를 입력하세요."));
			return;
		}

		GetWorldTimerManager().ClearTimer(TurnTimerHandle);
		SenderPC->ClientNotifyTurnEnded();
		ResolveNumberBaseballTurn(SenderPC, FinalGuess);
	}
	else
	{
		return;
	}

	AdvanceTurn();
}

void AGCGameMode::EndTurnByTimeout()
{
	AGCPlayerState* TurnPlayerState = GetCurrentTurnPlayerState();
	AGCGameState* GCGameState = GetGameState<AGCGameState>();

	if (!IsValid(TurnPlayerState) || !IsValid(GCGameState)) return;

	TurnPlayerState->SetUsedTurnCount(TurnPlayerState->GetUsedTurnCount() + 1);

	AController* OwnerController = Cast<AController>(TurnPlayerState->GetOwner());
	AGCPlayerController* TurnPC = Cast<AGCPlayerController>(OwnerController);

	if (IsValid(TurnPC))
	{
		TurnPC->ClientNotifyTurnTimedOut();
	}

	GCGameState->AddPublicTurnSummaryLine(
		FString::Printf(TEXT("%s : 시간 초과"), *TurnPlayerState->GetChatNickname())
	);

	AdvanceTurn();
}

void AGCGameMode::AdvanceTurn()
{
	if (ActiveParticipants.Num() <= 0) return;

	++CurrentTurnPlayerIndex;

	if (CurrentTurnPlayerIndex >= ActiveParticipants.Num())
	{
		FinishRound();
		return;
	}

	SyncGameState();
	StartTurn();
}

void AGCGameMode::FinishRound()
{
	BroadcastSystemMessage(FString::Printf(TEXT("%d 라운드가 종료되었습니다."), CurrentRoundIndex));

	GetWorldTimerManager().ClearTimer(RoundSummaryTimerHandle);
	GetWorldTimerManager().SetTimer(
		RoundSummaryTimerHandle,
		FTimerDelegate::CreateLambda([this]()
		{
			if (!IsValid(this)) return;

			TArray<FString> WinnerNames;

			for (const AGCPlayerState* Participant : ActiveParticipants)
			{
				if (IsValid(Participant) && Participant->HasClearedCurrentGame())
				{
					WinnerNames.Add(Participant->GetChatNickname());
				}
			}

			if (WinnerNames.Num() == 1)
			{
				EndCurrentGame(FString::Printf(TEXT("%s 님이 우승했습니다."), *WinnerNames[0]));
				return;
			}

			if (WinnerNames.Num() >= 2)
			{
				const FString WinnerList = FString::Join(WinnerNames, TEXT(", "));
				EndCurrentGame(FString::Printf(TEXT("%s 님이 공동우승했습니다."), *WinnerList));
				return;
			}

			++CurrentRoundIndex;

			if (CurrentRoundIndex > GetMaxTurnCountForCurrentGame())
			{
				EndCurrentGame(TEXT("모든 턴이 종료될 때까지 정답자가 없어 무승부로 처리합니다."));
				return;
			}

			CurrentTurnPlayerIndex = 0;

			AGCGameState* GCGameState = GetGameState<AGCGameState>();
			if (IsValid(GCGameState))
			{
				GCGameState->ClearPublicTurnSummary();
			}

			SyncGameState();
			BroadcastSystemMessage(FString::Printf(TEXT("%d 라운드를 시작합니다."), CurrentRoundIndex));
			StartTurn();
		}),
		RoundSummaryDisplayTime,
		false
	);
}

void AGCGameMode::EndCurrentGame(const FString& EndMessage)
{
	BroadcastSystemMessage(EndMessage);

	const FString AnswerRevealMessage = BuildCurrentAnswerRevealMessage();
	if (!AnswerRevealMessage.IsEmpty())
	{
		BroadcastSystemMessage(AnswerRevealMessage);
	}

	ResetRoomState();
}

bool AGCGameMode::IsGameSelectionStarter(const AGCPlayerState* PlayerState) const
{
	return IsValid(PlayerState) && PlayerState == GameSelectionStarter;
}

bool AGCGameMode::IsCurrentTurnPlayer(const AGCPlayerController* SenderPC) const
{
	if (!IsValid(SenderPC)) return false;

	AGCPlayerState* CurrentTurnPlayerState = GetCurrentTurnPlayerState();
	if (!IsValid(CurrentTurnPlayerState)) return false;

	return SenderPC->GetPlayerState<AGCPlayerState>() == CurrentTurnPlayerState;
}

AGCPlayerState* AGCGameMode::GetCurrentTurnPlayerState() const
{
	if (!ActiveParticipants.IsValidIndex(CurrentTurnPlayerIndex)) return nullptr;

	return ActiveParticipants[CurrentTurnPlayerIndex];
}

int32 AGCGameMode::GetMaxTurnCountForCurrentGame() const
{
	const AGCGameState* GCGameState = GetGameState<AGCGameState>();
	if (!IsValid(GCGameState)) return 0;

	if (GCGameState->GetCurrentMiniGameType() == EMiniGameType::Wordle) return WordleMaxTurnCount;

	return NumberBaseballMaxTurnCount;
}

void AGCGameMode::LoadWordleWordLists()
{
	AnswerWords.Empty();
	AllowedWords.Empty();

	if (AnswerWordsJsonRelativePath.IsEmpty() || AllowedWordsJsonRelativePath.IsEmpty()) return;

	const FString AnswerPath = FPaths::ProjectContentDir() / AnswerWordsJsonRelativePath;
	const FString AllowedPath = FPaths::ProjectContentDir() / AllowedWordsJsonRelativePath;

	FString AnswerJsonString;
	FString AllowedJsonString;

	if (!FFileHelper::LoadFileToString(AnswerJsonString, *AnswerPath)) return;
	if (!FFileHelper::LoadFileToString(AllowedJsonString, *AllowedPath)) return;

	TSharedRef<TJsonReader<>> AnswerReader = TJsonReaderFactory<>::Create(AnswerJsonString);
	TSharedRef<TJsonReader<>> AllowedReader = TJsonReaderFactory<>::Create(AllowedJsonString);

	TArray<TSharedPtr<FJsonValue>> AnswerJsonArray;
	TArray<TSharedPtr<FJsonValue>> AllowedJsonArray;
	
	int32 LoadedAnswerCount = 0;
	int32 LoadedAllowedCount = 0;

	if (FJsonSerializer::Deserialize(AnswerReader, AnswerJsonArray))
	{
		for (const TSharedPtr<FJsonValue>& JsonValue : AnswerJsonArray)
		{
			if (JsonValue.IsValid())
			{
				const FString LowerWord = JsonValue->AsString().ToLower();
				AnswerWords.Add(LowerWord);
				AllowedWords.Add(LowerWord);
				++LoadedAnswerCount;
			}
		}
	}

	if (FJsonSerializer::Deserialize(AllowedReader, AllowedJsonArray))
	{
		for (const TSharedPtr<FJsonValue>& JsonValue : AllowedJsonArray)
		{
			if (JsonValue.IsValid())
			{
				AllowedWords.Add(JsonValue->AsString().ToLower());
				++LoadedAllowedCount;
			}
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Loaded %d answer words and %d allowed words for Wordle."), LoadedAnswerCount, LoadedAllowedCount);
}

bool AGCGameMode::IsAllowedWord(const FString& Word) const
{
	return AllowedWords.Contains(Word.ToLower());
}

FString AGCGameMode::PickRandomWordleAnswer() const
{
	if (AnswerWords.Num() <= 0) return TEXT("");

	const int32 RandomIndex = FMath::RandRange(0, AnswerWords.Num() - 1);
	return AnswerWords[RandomIndex];
}

void AGCGameMode::StartWordleGame()
{
	CurrentWordleAnswer = PickRandomWordleAnswer();
	UE_LOG(LogTemp, Log, TEXT("[Wordle] Current Answer: %s"), *CurrentWordleAnswer);
}

bool AGCGameMode::ValidateWordleGuess(const FString& GuessWord) const
{
	if (GuessWord.Len() != 5) return false;

	return IsAllowedWord(GuessWord);
}

void AGCGameMode::ResolveWordleTurn(AGCPlayerController* SenderPC, const FString& GuessWord)
{
	if (!IsValid(SenderPC)) return;

	AGCPlayerState* SenderPS = SenderPC->GetPlayerState<AGCPlayerState>();
	AGCGameState* GCGameState = GetGameState<AGCGameState>();

	if (!IsValid(SenderPS) || !IsValid(GCGameState)) return;

	SenderPS->SetUsedTurnCount(SenderPS->GetUsedTurnCount() + 1);

	const TArray<EWordleLetterState> LetterStates = JudgeWordleStates(CurrentWordleAnswer, GuessWord);

	SenderPC->ClientReceivePrivateTurnResult(MakeWordlePrivateResultText(GuessWord, LetterStates));
	GCGameState->AddPublicTurnSummaryLine(MakeWordlePublicSummary(SenderPS->GetChatNickname(), LetterStates));

	if (GuessWord == CurrentWordleAnswer)
	{
		SenderPS->SetHasClearedCurrentGame(true);
		BroadcastSystemMessage(FString::Printf(TEXT("%s 님이 정답을 맞혔습니다."), *SenderPS->GetChatNickname()));
	}
}

TArray<EWordleLetterState> AGCGameMode::JudgeWordleStates(const FString& Answer, const FString& Guess) const
{
	TArray<EWordleLetterState> Result;
	Result.Init(EWordleLetterState::Gray, Guess.Len());

	TMap<TCHAR, int32> RemainingCounts;

	for (int32 Index = 0; Index < Answer.Len(); ++Index)
	{
		if (Guess[Index] == Answer[Index])
		{
			Result[Index] = EWordleLetterState::Green;
		}
		else
		{
			RemainingCounts.FindOrAdd(Answer[Index])++;
		}
	}

	for (int32 Index = 0; Index < Guess.Len(); ++Index)
	{
		if (Result[Index] == EWordleLetterState::Green) continue;

		int32* FoundCount = RemainingCounts.Find(Guess[Index]);
		if (FoundCount != nullptr && *FoundCount > 0)
		{
			Result[Index] = EWordleLetterState::Yellow;
			--(*FoundCount);
		}
	}

	return Result;
}

FString AGCGameMode::MakeWordlePrivateResultText(const FString& GuessWord, const TArray<EWordleLetterState>& States) const
{
	FString ResultText = TEXT("Wordle Result : ");

	for (int32 Index = 0; Index < States.Num() && Index < GuessWord.Len(); ++Index)
	{
		if (Index > 0)
		{
			ResultText += TEXT(" ");
		}

		const TCHAR UpperCharacter = FChar::ToUpper(GuessWord[Index]);
		ResultText += FString::Printf(TEXT("%c("), UpperCharacter);

		const EWordleLetterState State = States[Index];
		switch (State)
		{
		case EWordleLetterState::Green:
			ResultText += TEXT("G");
			break;

		case EWordleLetterState::Yellow:
			ResultText += TEXT("Y");
			break;

		case EWordleLetterState::Gray:
			ResultText += TEXT("X");
			break;

		default:
			ResultText += TEXT("?");
			break;
		}

		ResultText += TEXT(")");
	}

	return ResultText;
}

FString AGCGameMode::MakeWordlePublicSummary(const FString& PlayerName, const TArray<EWordleLetterState>& States) const
{
	int32 GreenCount = 0;
	int32 YellowCount = 0;
	int32 GrayCount = 0;

	for (const EWordleLetterState State : States)
	{
		switch (State)
		{
		case EWordleLetterState::Green:
			++GreenCount;
			break;

		case EWordleLetterState::Yellow:
			++YellowCount;
			break;

		case EWordleLetterState::Gray:
			++GrayCount;
			break;

		default:
			break;
		}
	}

	return FString::Printf(
		TEXT("%s : Green %d / Yellow %d / Gray %d"),
		*PlayerName,
		GreenCount,
		YellowCount,
		GrayCount
	);
}

FString AGCGameMode::BuildCurrentAnswerRevealMessage() const
{
	const AGCGameState* GCGameState = GetGameState<AGCGameState>();
	if (!IsValid(GCGameState))
	{
		return TEXT("");
	}

	switch (GCGameState->GetCurrentMiniGameType())
	{
	case EMiniGameType::Wordle:
		return CurrentWordleAnswer.IsEmpty()
			? TEXT("")
			: FString::Printf(TEXT("이번 워들의 정답은 %s 였습니다."), *CurrentWordleAnswer.ToUpper());

	case EMiniGameType::NumberBaseball:
		return AnswerNumberString.IsEmpty()
			? TEXT("")
			: FString::Printf(TEXT("이번 숫자야구의 정답은 %s 였습니다."), *AnswerNumberString);

	default:
		return TEXT("");
	}
}

void AGCGameMode::StartNumberBaseballGame()
{
	AnswerNumberString = GenerateAnswerNumber();
	UE_LOG(LogTemp, Log, TEXT("[NumberBaseball] Current Answer: %s"), *AnswerNumberString);
}

FString AGCGameMode::GenerateAnswerNumber() const
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; i++)
	{
		Numbers.Add(i);
	}
	
	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0;});
	
	FString Result;
	for (int32 i = 0; i < 3; i++)
	{
		int32 index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[index]));
		Numbers.RemoveAt(index);
	}
	return Result;
}

bool AGCGameMode::IsGuessNumberString(const FString& InNumberString) const
{
	if (InNumberString.Len() != 3) return false;

	TSet<TCHAR> UsedDigits;

	for (const TCHAR Ch : InNumberString)
	{
		if (!FChar::IsDigit(Ch) || Ch == '0') return false;

		if (UsedDigits.Contains(Ch)) return false;

		UsedDigits.Add(Ch);
	}

	return true;
}

void AGCGameMode::ResolveNumberBaseballTurn(AGCPlayerController* SenderPC, const FString& GuessNumberString)
{
	if (!IsValid(SenderPC)) return;

	AGCPlayerState* SenderPS = SenderPC->GetPlayerState<AGCPlayerState>();
	AGCGameState* GCGameState = GetGameState<AGCGameState>();

	if (!IsValid(SenderPS) || !IsValid(GCGameState)) return;

	SenderPS->SetUsedTurnCount(SenderPS->GetUsedTurnCount() + 1);

	int32 Strike = 0;
	int32 Ball = 0;

	for (int32 Index = 0; Index < GuessNumberString.Len(); ++Index)
	{
		if (GuessNumberString[Index] == AnswerNumberString[Index]) Strike++;
		else if (AnswerNumberString.Contains(FString::Chr(GuessNumberString[Index]))) Ball++;
	}

	SenderPC->ClientReceivePrivateTurnResult(
		FString::Printf(TEXT("%d Strike, %d Ball"), Strike, Ball)
	);

	GCGameState->AddPublicTurnSummaryLine(
		FString::Printf(TEXT("%s : %d Strike, %d Ball"), *SenderPS->GetChatNickname(), Strike, Ball)
	);

	if (Strike == 3)
	{
		SenderPS->SetHasClearedCurrentGame(true);
		BroadcastSystemMessage(FString::Printf(TEXT("%s 님이 정답을 맞혔습니다."), *SenderPS->GetChatNickname()));
	}
}

void AGCGameMode::SyncGameState()
{
	AGCGameState* GCGameState = GetGameState<AGCGameState>();
	if (!IsValid(GCGameState)) return;

	GCGameState->SetCurrentRoundIndex(CurrentRoundIndex);
	GCGameState->SetCurrentTurnPlayer(GetCurrentTurnPlayerState());
	GCGameState->SetCurrentParticipants(ActiveParticipants);
}