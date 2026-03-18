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
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void BP_RefreshChatMessages(const TArray<FChatMessageData>& InMessages);

	void NotifyPrivateSystemMessage(const FString& Message);
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateRoomStatusText(const FString& InStatusText);
	
protected:
	UPROPERTY(BlueprintReadOnly, Category="ChatRoom", meta=(BindWidget))
	UScrollBox* ChatScrollBox;

	UPROPERTY(BlueprintReadOnly, Category="ChatRoom", meta=(BindWidget))
	UEditableTextBox* ChatInputTextBox;

	UPROPERTY(BlueprintReadOnly, Category="ChatRoom", meta=(BindWidget))
	UButton* SendButton;

	UPROPERTY(BlueprintReadOnly, Category="ChatRoom", meta=(BindWidgetOptional))
	UTextBlock* RoomStatusText;

	UFUNCTION()
	void OnClickedSendButton();
	
	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	
protected:
	void RefreshFromGameState();
	FString BuildRoomStatusText() const;
	
protected:
	int32 CachedChatMessageCount = INDEX_NONE;
	int32 CachedPrivateSystemMessageCount = 0;
	FString CachedRoomStatusText; 
	TArray<FChatMessageData> LocalPrivateSystemMessages;
};
