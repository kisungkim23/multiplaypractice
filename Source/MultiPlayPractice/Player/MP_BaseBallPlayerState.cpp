// Fill out your copyright notice in the Description page of Project Settings.


#include "MP_BaseBallPlayerState.h"

#include "Net/UnrealNetwork.h"

AMP_BaseBallPlayerState::AMP_BaseBallPlayerState()
	: PlayerNameString(TEXT("None"))
{
	bReplicates = true;
}

void AMP_BaseBallPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, PlayerGameState);
}

void AMP_BaseBallPlayerState::SetPlayerReady()
{
	if(PlayerGameState == EGameState::None)
		PlayerGameState = EGameState::Ready;
}

void AMP_BaseBallPlayerState::SetGamePlay()
{
	PlayerGameState = EGameState::Play;
}

void AMP_BaseBallPlayerState::SetEndPlay()
{
	PlayerGameState = EGameState::None;
}

EGameState AMP_BaseBallPlayerState::GetPlayerGameState() const
{
	return PlayerGameState;
}
