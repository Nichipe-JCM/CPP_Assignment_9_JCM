// Copyright 2026 Joo CheolMin. All Rights Reserved

#include "CPP_Assignment_9_JCM/Public/UI/GCNicknameEntryWidgetBase.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
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

	HideNicknameError();
}

void UGCNicknameEntryWidgetBase::ShowNicknameError(const FString& ErrorMessage)
{
	if (!IsValid(ErrorText)) return;

	ErrorText->SetText(FText::FromString(ErrorMessage));
	ErrorText->SetVisibility(ESlateVisibility::Visible);
}

void UGCNicknameEntryWidgetBase::HideNicknameError()
{
	if (!IsValid(ErrorText)) return;

	ErrorText->SetText(FText::GetEmpty());
	ErrorText->SetVisibility(ESlateVisibility::Collapsed);
}

void UGCNicknameEntryWidgetBase::OnClickedConfirmedButton()
{
	if (!IsValid(NicknameTextBox)) return;

	AGCPlayerController* GCPC = GetOwningPlayer<AGCPlayerController>();
	if (!IsValid(GCPC)) return;

	FString Nickname = NicknameTextBox->GetText().ToString();
	Nickname.TrimStartAndEndInline();

	HideNicknameError();
	GCPC->ServerSubmitNickname(Nickname);
}

void UGCNicknameEntryWidgetBase::OnNicknameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod != ETextCommit::OnEnter) return;
	OnClickedConfirmedButton();
}
