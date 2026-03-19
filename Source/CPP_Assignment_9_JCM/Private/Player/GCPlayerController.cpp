// Copyright 2026 Joo CheolMin. All Rights Reserved

#include "CPP_Assignment_9_JCM/Public/Player/GCPlayerController.h"

#include "CPP_Assignment_9_JCM/Public/Game/GCGameMode.h"
#include "CPP_Assignment_9_JCM/Public/UI/GCChatRoomWidgetBase.h"
#include "CPP_Assignment_9_JCM/Public/UI/GCNicknameEntryWidgetBase.h"
#include "CPP_Assignment_9_JCM/Public/UI/GCGameOverlayWidgetBase.h"

#include "Engine/Engine.h"

AGCPlayerController::AGCPlayerController()
{
	bShowMouseCursor = true;
}

void AGCPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (IsLocalController()) CreateInitialUI();
}

void AGCPlayerController::CreateInitialUI()
{
	if (!IsLocalController()) return;
	
	if (IsValid(NicknameEntryWidgetClass) && !IsValid(NicknameEntryWidgetInstance))
	{
		NicknameEntryWidgetInstance = CreateWidget<UGCNicknameEntryWidgetBase>(this, NicknameEntryWidgetClass);
		if (IsValid(NicknameEntryWidgetInstance))
		{
			NicknameEntryWidgetInstance->AddToViewport(10);
		}
	}
	
	if (IsValid(ChatRoomWidgetClass) && !IsValid(ChatRoomWidgetInstance))
	{
		ChatRoomWidgetInstance = CreateWidget<UGCChatRoomWidgetBase>(this, ChatRoomWidgetClass);
		if (IsValid(ChatRoomWidgetInstance))
		{
			ChatRoomWidgetInstance->AddToViewport(0);
		}
	}

	if (IsValid(GameOverlayWidgetClass) && !IsValid(GameOverlayWidgetInstance))
	{
		GameOverlayWidgetInstance = CreateWidget<UGCGameOverlayWidgetBase>(this, GameOverlayWidgetClass);
		if (IsValid(GameOverlayWidgetInstance))
		{
			GameOverlayWidgetInstance->AddToViewport(20);
		}
	}
	
	ShowNicknameEntryUI();
}

void AGCPlayerController::ShowNicknameEntryUI()
{
	if (IsValid(NicknameEntryWidgetInstance))
	{
		NicknameEntryWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}

	if (IsValid(ChatRoomWidgetInstance))
	{
		ChatRoomWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}

	if (IsValid(GameOverlayWidgetInstance))
	{
		GameOverlayWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
	}

	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void AGCPlayerController::ShowChatRoomUI()
{
	if (IsValid(NicknameEntryWidgetInstance))
	{
		NicknameEntryWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (IsValid(ChatRoomWidgetInstance))
	{
		ChatRoomWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}

	if (IsValid(GameOverlayWidgetInstance))
	{
		GameOverlayWidgetInstance->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	FInputModeGameAndUI InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void AGCPlayerController::ServerSubmitNickname_Implementation(const FString& Nickname)
{
	AGCGameMode* GCGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AGCGameMode>() : nullptr;
	if (!IsValid(GCGameMode)) return;
	
	GCGameMode->HandleNicknameSubmit(this, Nickname);
}

void AGCPlayerController::ClientHandleNicknameSubmitResult_Implementation(bool bSuccess, const FString& ErrorMessage)
{
	if (!IsValid(NicknameEntryWidgetInstance))
	{
		return;
	}

	if (bSuccess)
	{
		NicknameEntryWidgetInstance->HideNicknameError();
		ShowChatRoomUI();
		return;
	}

	NicknameEntryWidgetInstance->ShowNicknameError(ErrorMessage);
	ShowNicknameEntryUI();
}

void AGCPlayerController::ServerSubmitChatInput_Implementation(const FString& InputText)
{
	AGCGameMode* GCGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AGCGameMode>() : nullptr;
	if (!IsValid(GCGameMode)) return;
	
	GCGameMode->HandleChatInput(this, InputText);
}

void AGCPlayerController::ClientReceivePrivateSystemMessage_Implementation(const FString& Message)
{
	if (!IsValid(ChatRoomWidgetInstance)) return;

	ChatRoomWidgetInstance->NotifyPrivateSystemMessage(Message);
}

void AGCPlayerController::ClientReceivePrivateTurnResult_Implementation(const FString& ResultText)
{
	if (!IsValid(GameOverlayWidgetInstance)) return;
	
	GameOverlayWidgetInstance->NotifyPrivateTurnResult(ResultText);
}

void AGCPlayerController::ClientNotifyTurnStarted_Implementation(float InTurnTimeLimit)
{
	if (!IsValid(GameOverlayWidgetInstance)) return;
	
	GameOverlayWidgetInstance->NotifyTurnStarted(InTurnTimeLimit);
}

void AGCPlayerController::ClientNotifyTurnEnded_Implementation()
{
	if (!IsValid(GameOverlayWidgetInstance)) return;
	
	GameOverlayWidgetInstance->NotifyTurnEnded();
}

void AGCPlayerController::ClientNotifyTurnTimedOut_Implementation()
{
	if (!IsValid(GameOverlayWidgetInstance)) return;
	
	GameOverlayWidgetInstance->NotifyTurnTimedOut();
}

void AGCPlayerController::ClientResetPrivateGameRecords_Implementation()
{
	if (IsValid(ChatRoomWidgetInstance))
	{
		ChatRoomWidgetInstance->ResetPrivateSystemMessages();
	}

	if (IsValid(GameOverlayWidgetInstance))
	{
		GameOverlayWidgetInstance->ResetPrivateGameRecords();
	}
}
