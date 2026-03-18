// Copyright 2026 Joo CheolMin. All Rights Reserved
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GCNicknameEntryWidgetBase.generated.h"

class UEditableTextBox;
class UButton;

UCLASS()
class CPP_ASSIGNMENT_9_JCM_API UGCNicknameEntryWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
protected:
	UPROPERTY(BlueprintReadOnly, Category="NicknameEntry", meta=(BindWidget))
	UEditableTextBox* NicknameTextBox;

	UPROPERTY(BlueprintReadOnly, Category="NicknameEntry", meta=(BindWidget))
	UButton* ConfirmButton;
	
	UFUNCTION()
	void OnClickedConfirmedButton();
	
	UFUNCTION()
	void OnNicknameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
};
