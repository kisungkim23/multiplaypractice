// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MP_BaseBallGameModeBase.generated.h"

class AMP_BaseBallPlayerController;
class AMP_BaseBallPlayerState;
/**
 *
 */
UCLASS()
class MULTIPLAYPRACTICE_API AMP_BaseBallGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	virtual void BeginPlay() override;

	void PrintChatMessageString(AMP_BaseBallPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void ResetGame();

	void StartGame();

	bool JudgeGame(AMP_BaseBallPlayerController* InChattingPlayerController, int InStrikeCount);

	void CheckStartGame();

	void CountDown();

	void SetPlayerTurn(AMP_BaseBallPlayerState* PlayerState);

	void IncreaseGuessCount(AMP_BaseBallPlayerController* InChattingPlayerController);

protected:
	TMap< TObjectPtr<AMP_BaseBallPlayerController>, FString> SecretNumberStrings;

	TArray<TObjectPtr<AMP_BaseBallPlayerController>> AllPlayerControllers;

	FTimerHandle TurnCountHandler;

	int32 CurrentTurnIndex;

	int32 CurrentGuessCount;
};
