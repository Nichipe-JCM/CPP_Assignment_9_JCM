// Copyright 2026 Joo CheolMin. All Rights Reserved
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/ChatTypes.h"
#include "GCChatRoomWidgetBase.generated.h"

class UScrollBox;
class UEditableTextBox;
class UButton;
class UTextBlock;

UCLASS()
class CPP_ASSIGNMENT_9_JCM_API UGCChatRoomWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void BP_RefreshChatMessages(const TArray<FChatMessageData>& InMessages);
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateRoomStatusText(const FString& InStatusText);
	
protected:
	UPROPERTY(meta=(BindWidget))
	UScrollBox* ChatScrollBox;

	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* ChatInputTextBox;

	UPROPERTY(meta=(BindWidget))
	UButton* SendButton;

	UPROPERTY(meta=(BindWidgetOptional))
	UTextBlock* RoomStatusText;

	UFUNCTION()
	void OnClickedSendButton();
};
