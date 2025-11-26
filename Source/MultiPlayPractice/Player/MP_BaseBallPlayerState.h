// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/MP_BaseBallGameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "MP_BaseBallPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYPRACTICE_API AMP_BaseBallPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AMP_BaseBallPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void SetPlayerReady();
	void SetGamePlay();
	void SetEndPlay();

	EGameState GetPlayerGameState() const;

	FString GetPlayerInfoString();

public:
	UPROPERTY(Replicated)
	FString PlayerNameString;

	UPROPERTY(Replicated)
	EGameState PlayerGameState;

	UPROPERTY(Replicated)
	int32 CurrentGuessCount;
};
