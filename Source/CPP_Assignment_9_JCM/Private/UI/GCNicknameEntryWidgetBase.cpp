// Copyright 2026 Joo CheolMin. All Rights Reserved

#include "CPP_Assignment_9_JCM/Public/UI/GCNicknameEntryWidgetBase.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Player/GCPlayerController.h"


void UGCNicknameEntryWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (IsValid(ConfirmButton))
	{
		ConfirmButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedConfirmedButton);
	}
	
	if (IsValid(NicknameTextBox))
	{
		NicknameTextBox->OnTextCommitted.AddDynamic(this, &ThisClass::OnNicknameTextCommitted);
	}
}

void UGCNicknameEntryWidgetBase::OnClickedConfirmedButton()
{
	if (!IsValid(NicknameTextBox)) return;

	AGCPlayerController* GCPC = GetOwningPlayer<AGCPlayerController>();
	if (!IsValid(GCPC)) return;

	FString Nickname = NicknameTextBox->GetText().ToString();
	Nickname.TrimStartAndEndInline();

	if (Nickname.IsEmpty()) return;

	GCPC->ServerSubmitNickname(Nickname);
	GCPC->ShowChatRoomUI();
}

void UGCNicknameEntryWidgetBase::OnNicknameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod != ETextCommit::OnEnter) return;
	OnClickedConfirmedButton();
}
