// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MP_BaseBallGameModeBase.h"
#include "MP_BaseBallGameStateBase.h"
#include "Player/MP_BaseBallPlayerController.h"
#include "EngineUtils.h"
#include "Player/MP_BaseBallPlayerState.h"

void AMP_BaseBallGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	AMP_BaseBallPlayerController* MP_BaseBallPlayerController = Cast<AMP_BaseBallPlayerController>(NewPlayer);
	if (IsValid(MP_BaseBallPlayerController) == true)
	{
		MP_BaseBallPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));

		AllPlayerControllers.Add(MP_BaseBallPlayerController);

		AMP_BaseBallPlayerState* PS = MP_BaseBallPlayerController->GetPlayerState<AMP_BaseBallPlayerState>();
		if (IsValid(PS) == true)
		{
			PS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		AMP_BaseBallGameStateBase* MPGameStateBase = GetGameState<AMP_BaseBallGameStateBase>();
		if (IsValid(MPGameStateBase) == true)
		{
			MPGameStateBase->MulticastRPCBroadcastLoginMessage(PS->PlayerNameString);
		}
	}
}

FString AMP_BaseBallGameModeBase::GenerateSecretNumber()
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

bool AMP_BaseBallGameModeBase::IsGuessNumberString(const FString& InNumberString)
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

FString AMP_BaseBallGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
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

void AMP_BaseBallGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
}

void AMP_BaseBallGameModeBase::PrintChatMessageString(AMP_BaseBallPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{

	AMP_BaseBallGameStateBase* MPGameStateBase = GetGameState<AMP_BaseBallGameStateBase>();
	if (!IsValid(MPGameStateBase) == true)
	{
		return;
	}

	AMP_BaseBallPlayerState* MPPlayerState = InChattingPlayerController->GetPlayerState<AMP_BaseBallPlayerState>();
	FString ChatMessageString = InChatMessageString;
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);
	if (MPGameStateBase->GetCurrentPlayer() == MPPlayerState && IsGuessNumberString(GuessNumberString) == true)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);
		for (TActorIterator<AMP_BaseBallPlayerController> It(GetWorld()); It; ++It)
		{
			AMP_BaseBallPlayerController* MPPlayerController = *It;
			if (IsValid(MPPlayerController) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;
				MPPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
			}
		}

		int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
		JudgeGame(InChattingPlayerController, StrikeCount);
		const int32 NumPlayers = AllPlayerControllers.Num();
		CurrentTurnIndex = (CurrentTurnIndex + 1) % NumPlayers;
		SetPlayerTurn(AllPlayerControllers[CurrentTurnIndex]->GetPlayerState<AMP_BaseBallPlayerState>());
	}
	else
	{
		for (TActorIterator<AMP_BaseBallPlayerController> It(GetWorld()); It; ++It)
		{
			AMP_BaseBallPlayerController* MPPlayerController = *It;
			if (IsValid(MPPlayerController) == true)
			{
				MPPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}


void AMP_BaseBallGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();
	GetWorld()->GetTimerManager().ClearTimer(TurnCountHandler);
	for (const auto& MPPlayerController : AllPlayerControllers)
	{
		if (!IsValid(MPPlayerController))
		{
			continue;
		}

		AMP_BaseBallPlayerState* MPPlayerState = MPPlayerController->GetPlayerState<AMP_BaseBallPlayerState>();
		if (!IsValid(MPPlayerState))
		{
			continue;
		}

		MPPlayerState->SetEndPlay();
		GetWorld()->GetTimerManager().ClearTimer(TurnCountHandler);
		MPPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
	}
}

void AMP_BaseBallGameModeBase::JudgeGame(AMP_BaseBallPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		AMP_BaseBallPlayerState* MPPS = InChattingPlayerController->GetPlayerState<AMP_BaseBallPlayerState>();
		for (const auto& MPPlayerController : AllPlayerControllers)
		{
			if (IsValid(MPPS) == true)
			{
				FString CombinedMessageString = MPPS->PlayerNameString + TEXT(" has won the game.");
				MPPlayerController->NotificationText = FText::FromString(CombinedMessageString);
			}
		}

		ResetGame();
	}
}

void AMP_BaseBallGameModeBase::CheckStartGame()
{
	for (const auto& MPPlayerController : AllPlayerControllers)
	{
		if (!IsValid(MPPlayerController))
		{
			continue;
		}

		AMP_BaseBallPlayerState* MPPlayerState = MPPlayerController->GetPlayerState<AMP_BaseBallPlayerState>();
		if (!IsValid(MPPlayerState))
		{
			continue;
		}     

		if (MPPlayerState->GetPlayerGameState() != EGameState::Ready)
		{
			FString CombinedMessageString =  TEXT(" Return ") ;
			MPPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
			return;
		}
	}

	AMP_BaseBallGameStateBase* MPGameStateBase = GetGameState<AMP_BaseBallGameStateBase>();
	if (IsValid(MPGameStateBase) == true)
	{
		for (const auto& MPPlayerController : AllPlayerControllers)
		{
			if (!IsValid(MPPlayerController))
			{
				continue;
			}

			AMP_BaseBallPlayerState* MPPlayerState = MPPlayerController->GetPlayerState<AMP_BaseBallPlayerState>();
			if (!IsValid(MPPlayerState))
			{
				continue;
			}

			MPPlayerState->SetGamePlay();
		}

		const int32 NumPlayers = AllPlayerControllers.Num();
		CurrentTurnIndex = 0;
		SetPlayerTurn(AllPlayerControllers[CurrentTurnIndex]->GetPlayerState<AMP_BaseBallPlayerState>());
	}

}

void AMP_BaseBallGameModeBase::CountDown()
{
	AMP_BaseBallGameStateBase* MPGameStateBase = GetGameState<AMP_BaseBallGameStateBase>();
	if (IsValid(MPGameStateBase) == true)
	{
		int Count = MPGameStateBase->CountDown();

		UE_LOG(LogTemp, Warning, TEXT("Count %d"), Count);
		if (Count >= 0)
		{
			for (const auto& MPPlayerController : AllPlayerControllers)
			{
				if (!IsValid(MPPlayerController))
				{
					continue;
				}

				MPPlayerController->SetTurnCount(Count);
			}
		}
		else
		{
			const int32 NumPlayers = AllPlayerControllers.Num();
			CurrentTurnIndex = (CurrentTurnIndex + 1) % NumPlayers;
			SetPlayerTurn(AllPlayerControllers[CurrentTurnIndex]->GetPlayerState<AMP_BaseBallPlayerState>());
		}
	}
}

void AMP_BaseBallGameModeBase::SetPlayerTurn(AMP_BaseBallPlayerState* PlayerState)
{
	AMP_BaseBallGameStateBase* MPGameStateBase = GetGameState<AMP_BaseBallGameStateBase>();
	if (IsValid(MPGameStateBase) == true)
	{
		MPGameStateBase->SetCurrentPlayer(PlayerState);

		int Count = MPGameStateBase->ResetCoundDownValue();
		for (const auto& MPPlayerController : AllPlayerControllers)
		{
			if (!IsValid(MPPlayerController))
			{
				continue;
			}

			MPPlayerController->SetTurnCount(Count);
			auto CurrentPlayer = MPGameStateBase->GetCurrentPlayer();
			if (!IsValid(CurrentPlayer))
			{
				FString CombinedMessageString = CurrentPlayer->PlayerNameString + TEXT("'s Turn");
				MPPlayerController->NotificationText = FText::FromString(CombinedMessageString);
			}
		}

		GetWorld()->GetTimerManager().ClearTimer(TurnCountHandler);
		GetWorld()->GetTimerManager().SetTimer(
			TurnCountHandler,
			FTimerDelegate::CreateUObject(this, &AMP_BaseBallGameModeBase::CountDown),
			1.0f,
			true
		);
	}
}
