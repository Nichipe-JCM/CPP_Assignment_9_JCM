// Copyright 2026 Joo CheolMin. All Rights Reserved

#include "CPP_Assignment_9_JCM/Public/Player/GCPlayerState.h"
#include "Net/UnrealNetwork.h"

AGCPlayerState::AGCPlayerState()
{
	ChatNickname = TEXT("Player");
}

void AGCPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AGCPlayerState, ChatNickname);
	DOREPLIFETIME(AGCPlayerState, bIsMinigameParticipant);
	DOREPLIFETIME(AGCPlayerState, bCanSendNormalChat);
	DOREPLIFETIME(AGCPlayerState, bHasClearedCurrentGame);
	DOREPLIFETIME(AGCPlayerState, UsedTurnCount);
}

void AGCPlayerState::ResetMiniGamePlayerState()
{
	bIsMinigameParticipant = false;
	bCanSendNormalChat = true;
	bHasClearedCurrentGame = false;
	UsedTurnCount = 0;
}
