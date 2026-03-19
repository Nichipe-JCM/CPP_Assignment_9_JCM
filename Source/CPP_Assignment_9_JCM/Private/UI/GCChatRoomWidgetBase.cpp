// Copyright 2026 Joo CheolMin. All Rights Reserved

#include "CPP_Assignment_9_JCM/Public/UI/GCChatRoomWidgetBase.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Game/GCGameState.h"
#include "Player/GCPlayerController.h"

void UGCChatRoomWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (IsValid(SendButton))
	{
		SendButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedSendButton);
	}
	
	if (IsValid(ChatInputTextBox))
	{
		ChatInputTextBox->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
	RefreshFromGameState();
}

void UGCChatRoomWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	RefreshFromGameState();
}

void UGCChatRoomWidgetBase::OnClickedSendButton()
{
	if (!IsValid(ChatInputTextBox)) return;
	
	AGCPlayerController* GCPC = Cast<AGCPlayerController>(GetOwningPlayer());
	if (!IsValid(GCPC)) return;
	
	FString InputText = ChatInputTextBox->GetText().ToString();
	InputText.TrimStartAndEndInline();
	
	if (InputText.IsEmpty()) return;
	
	GCPC->ServerSubmitChatInput(InputText);
	
	ChatInputTextBox->SetText(FText::GetEmpty());
}

void UGCChatRoomWidgetBase::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod != ETextCommit::OnEnter) return;
	OnClickedSendButton();
}

void UGCChatRoomWidgetBase::NotifyPrivateSystemMessage(const FString& Message)
{
	if (Message.IsEmpty()) return;

	const AGCGameState* GCGS = GetWorld() ? GetWorld()->GetGameState<AGCGameState>() : nullptr;
	const int32 PublicMessageAnchor = IsValid(GCGS) ? GCGS->GetChatMessages().Num() : 0;

	FChatMessageData NewMessage;
	NewMessage.SenderName = TEXT("System");
	NewMessage.Message = Message;
	NewMessage.MessageType = EChatMessageType::System;
	LocalPrivateSystemMessages.Add(NewMessage);
	LocalPrivateSystemMessageAnchors.Add(PublicMessageAnchor);

	RefreshFromGameState();
}

void UGCChatRoomWidgetBase::ResetPrivateSystemMessages()
{
	LocalPrivateSystemMessages.Empty();
	LocalPrivateSystemMessageAnchors.Empty();
	CachedPrivateSystemMessageCount = INDEX_NONE;
	RefreshFromGameState();
}

void UGCChatRoomWidgetBase::RefreshFromGameState()
{
	AGCGameState* GCGS = GetWorld() ? GetWorld()->GetGameState<AGCGameState>() : nullptr;
	if (!IsValid(GCGS)) return;

	const TArray<FChatMessageData>& Messages = GCGS->GetChatMessages();
	if (CachedChatMessageCount != Messages.Num() || CachedPrivateSystemMessageCount != LocalPrivateSystemMessages.Num())
	{
		CachedChatMessageCount = Messages.Num();
		CachedPrivateSystemMessageCount = LocalPrivateSystemMessages.Num();

		TArray<FChatMessageData> CombinedMessages = BuildCombinedChatMessages(Messages);
		BP_RefreshChatMessages(CombinedMessages);
	}

	const FString NewStatusText = BuildRoomStatusText();
	if (CachedRoomStatusText != NewStatusText)
	{
		CachedRoomStatusText = NewStatusText;
		BP_UpdateRoomStatusText(NewStatusText);

		if (IsValid(RoomStatusText))
		{
			RoomStatusText->SetText(FText::FromString(NewStatusText));
		}
	}
}

FString UGCChatRoomWidgetBase::BuildRoomStatusText() const
{
	const AGCGameState* GCGS = GetWorld() ? GetWorld()->GetGameState<AGCGameState>() : nullptr;
	if (!IsValid(GCGS)) return TEXT("Room State : Unknown");

	FString PhaseText;

	switch (GCGS->GetCurrentRoomPhase())
	{
	case ERoomPhase::Lobby:
		PhaseText = TEXT("Lobby");
		break;

	case ERoomPhase::SelectingGame:
		PhaseText = TEXT("Selecting");
		break;

	case ERoomPhase::Recruiting:
		PhaseText = TEXT("Recruiting");
		break;

	case ERoomPhase::Playing:
		PhaseText = TEXT("Playing");
		break;

	case ERoomPhase::Ending:
		PhaseText = TEXT("Ending");
		break;

	default:
		PhaseText = TEXT("Unknown");
		break;
	}

	return FString::Printf(TEXT("Room State : %s"), *PhaseText);
}

TArray<FChatMessageData> UGCChatRoomWidgetBase::BuildCombinedChatMessages(const TArray<FChatMessageData>& PublicMessages) const
{
	TArray<FChatMessageData> CombinedMessages;
	CombinedMessages.Reserve(PublicMessages.Num() + LocalPrivateSystemMessages.Num());

	for (int32 PublicIndex = 0; PublicIndex <= PublicMessages.Num(); ++PublicIndex)
	{
		for (int32 LocalIndex = 0; LocalIndex < LocalPrivateSystemMessages.Num(); ++LocalIndex)
		{
			const int32 Anchor = LocalPrivateSystemMessageAnchors.IsValidIndex(LocalIndex)
				? FMath::Clamp(LocalPrivateSystemMessageAnchors[LocalIndex], 0, PublicMessages.Num())
				: PublicMessages.Num();

			if (Anchor == PublicIndex)
			{
				CombinedMessages.Add(LocalPrivateSystemMessages[LocalIndex]);
			}
		}

		if (PublicIndex < PublicMessages.Num())
		{
			CombinedMessages.Add(PublicMessages[PublicIndex]);
		}
	}

	return CombinedMessages;
}

