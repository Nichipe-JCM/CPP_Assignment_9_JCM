// Copyright 2026 Joo CheolMin. All Rights Reserved

#include "CPP_Assignment_9_JCM/Public/Game/GCGameState.h"
#include "Net/UnrealNetwork.h"

AGCGameState::AGCGameState()
{
}

void AGCGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AGCGameState, CurrentRoomPhase);
	DOREPLIFETIME(AGCGameState, CurrentMiniGameType);
	DOREPLIFETIME(AGCGameState, RecruitInfo);
	DOREPLIFETIME(AGCGameState, CurrentRoundIndex);
	DOREPLIFETIME(AGCGameState, CurrentTurnPlayer);
	DOREPLIFETIME(AGCGameState, TurnRemainingTime);
	DOREPLIFETIME(AGCGameState, ChatMessages);
	DOREPLIFETIME(AGCGameState, CurrentParticipants);
	DOREPLIFETIME(AGCGameState, PublicTurnSummaryLines);
	
}

void AGCGameState::AddChatMessage(const FChatMessageData& NewMessage)
{
	ChatMessages.Add(NewMessage);
}

void AGCGameState::ClearChatMessages()
{
	ChatMessages.Empty();
}

void AGCGameState::ClearPublicTurnSummary()
{
	PublicTurnSummaryLines.Empty();
}
