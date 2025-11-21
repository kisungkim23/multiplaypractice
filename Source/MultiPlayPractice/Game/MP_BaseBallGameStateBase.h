// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MP_BaseBallGameStateBase.generated.h"

class AMP_BaseBallPlayerState;

UENUM(BlueprintType)
enum class EGameState : uint8
{
	None,
	Ready,
	Play
};
/**
/**
 * 
 */
UCLASS()
class MULTIPLAYPRACTICE_API AMP_BaseBallGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	void PlayGame();

	void SetReady();

	void GameEnd();

	int CountDown();

	int ResetCoundDownValue();

	void SetCurrentPlayer(AMP_BaseBallPlayerState* InCurrentTurnPlayer);

	AMP_BaseBallPlayerState* GetCurrentPlayer() const;

protected:

	UPROPERTY(Replicated)
	FString TurnPlayerName;

	UPROPERTY(Replicated)
	EGameState GameState;

	UPROPERTY(Replicated)
	int CountDownValue;

	UPROPERTY(Replicated)
	TObjectPtr<AMP_BaseBallPlayerState> CurrentTurnPlayer;
};
