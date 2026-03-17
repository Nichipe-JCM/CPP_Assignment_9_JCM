

#pragma once

#include "CoreMinimal.h"
#include "ChatTypes.generated.h"

UENUM(BlueprintType)
enum class ERoomPhase : uint8
{
	Lobby			UMETA(DisplayName = "Lobby"),
	SelectingGame	UMETA(DisplayName = "SelectingGame"),
	Recruiting		UMETA(DisplayName = "Recruiting"),
	Playing			UMETA(DisplayName = "Playing"),
	Ending			UMETA(DisplayName = "Ending")
};

UENUM(BlueprintType)
enum class EMiniGameType : uint8
{
	None			UMETA(DisplayName = "None"),
	Wordle			UMETA(DisplayName = "Wordle"),
	NumberBaseball	UMETA(DisplayName = "NumberBaseball")
};

UENUM(BlueprintType)
enum class EChatMessageType : uint8
{
	User	UMETA(DisplayName = "User"),
	System	UMETA(DisplayName = "System"),
	Game	UMETA(DisplayName = "Game")
};

UENUM(BlueprintType)
enum class EWordleLetterState : uint8
{
	Unknown UMETA(DisplayName = "Unknown"),
	Gray	UMETA(DisplayName = "Gray"),
	Yellow	UMETA(DisplayName = "Yellow"),
	Green	UMETA(DisplayName = "Green")
};

USTRUCT(BlueprintType)
struct FChatMessageData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString SenderName;

	UPROPERTY(BlueprintReadOnly)
	FString Message;

	UPROPERTY(BlueprintReadOnly)
	EChatMessageType MessageType = EChatMessageType::User;
};

USTRUCT(BlueprintType)
struct FRecruitInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EMiniGameType TargetGame = EMiniGameType::None;

	UPROPERTY(BlueprintReadOnly)
	bool bIsRecruiting = false;

	UPROPERTY(BlueprintReadOnly)
	float RemainingTime = 0.0f;
};

USTRUCT(BlueprintType)
struct FWordleTurnResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString PlayerName;

	UPROPERTY(BlueprintReadOnly)
	FString GuessWord;

	UPROPERTY(BlueprintReadOnly)
	TArray<EWordleLetterState> LetterStates;
};