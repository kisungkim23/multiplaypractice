// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBallGame/Game/MP_BaseBallGameStateBase.h"


#include "Kismet/GameplayStatics.h"
#include "BaseBallGame/Player/MP_BaseBallPlayerController.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

void AMP_BaseBallGameStateBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TurnPlayerName);
	DOREPLIFETIME(ThisClass, GameState);
	DOREPLIFETIME(ThisClass, CountDownValue);
	DOREPLIFETIME(ThisClass, CurrentTurnPlayer);
}

void AMP_BaseBallGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			AMP_BaseBallPlayerController* MPPC = Cast<AMP_BaseBallPlayerController>(PC);
			if (IsValid(MPPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				MPPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}

FString AMP_BaseBallGameStateBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool AMP_BaseBallGameStateBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {

		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;

	} while (false);

	return bCanPlay;
}

FString AMP_BaseBallGameStateBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void AMP_BaseBallGameStateBase::SetReady()
{
	if (GameState == EGameState::None)
		GameState = EGameState::Ready;
}

void AMP_BaseBallGameStateBase::PlayGame()
{
	if (GameState == EGameState::Ready)
		GameState = EGameState::Play;
}

void AMP_BaseBallGameStateBase::GameEnd()
{
	GameState = EGameState::None;
}

int AMP_BaseBallGameStateBase::CountDown()
{
	return --CountDownValue;
}

int AMP_BaseBallGameStateBase::ResetCoundDownValue()
{
	CountDownValue = 60.f;
	return CountDownValue;
}

void AMP_BaseBallGameStateBase::SetCurrentPlayer(AMP_BaseBallPlayerState* InCurrentTurnPlayer)
{
	CurrentTurnPlayer = InCurrentTurnPlayer;
}

AMP_BaseBallPlayerState* AMP_BaseBallGameStateBase::GetCurrentPlayer() const
{
	return CurrentTurnPlayer;
}
