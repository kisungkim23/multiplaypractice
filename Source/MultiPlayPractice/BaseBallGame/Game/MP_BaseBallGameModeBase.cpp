#include "BaseBallGame/Game/MP_BaseBallGameModeBase.h"
#include "BaseBallGame/Game/MP_BaseBallGameStateBase.h"
#include "BaseBallGame/Player/MP_BaseBallPlayerController.h"
#include "EngineUtils.h"
#include "BaseBallGame/Player/MP_BaseBallPlayerState.h"

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

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		const int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
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
		if (CurrentTurnIndex == 0)
		{
			CurrentGuessCount++;
		}

		FString PlayerSecretNumberString = SecretNumberStrings[InChattingPlayerController];
		//여기에 String 체크
		FString JudgeResultString = JudgeResult(PlayerSecretNumberString, GuessNumberString);
		for (TActorIterator<AMP_BaseBallPlayerController> It(GetWorld()); It; ++It)
		{
			AMP_BaseBallPlayerController* MPPlayerController = *It;
			if (IsValid(MPPlayerController) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString + FString::Printf(TEXT("  Count : %d"), MPPlayerState->CurrentGuessCount);
				MPPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
			}
			//AMP_BaseBallPlayerState* MPPlayerState = MPPlayerController->GetPlayerState<AMP_BaseBallPlayerState>();
		}

		int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
		if (!JudgeGame(InChattingPlayerController, StrikeCount))
		{
			const int32 NumPlayers = AllPlayerControllers.Num();
			CurrentTurnIndex = (CurrentTurnIndex + 1) % NumPlayers;
			SetPlayerTurn(AllPlayerControllers[CurrentTurnIndex]->GetPlayerState<AMP_BaseBallPlayerState>());
		}
		else
		{
			ResetGame();
		}
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
		MPPlayerState->CurrentGuessCount = 0;
	}
}

void AMP_BaseBallGameModeBase::StartGame()
{
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
			FString PlayerSecretNumber = GenerateSecretNumber();
			SecretNumberStrings.Emplace(MPPlayerController, PlayerSecretNumber);
			MPPlayerController->ClientRPCPrintChatMessageString(PlayerSecretNumber);
			MPPlayerState->SetGamePlay();
		}

		const int32 NumPlayers = AllPlayerControllers.Num();
		CurrentTurnIndex = 0;
		CurrentGuessCount = 0;
		SetPlayerTurn(AllPlayerControllers[CurrentTurnIndex]->GetPlayerState<AMP_BaseBallPlayerState>());
	}
}

bool AMP_BaseBallGameModeBase::JudgeGame(AMP_BaseBallPlayerController* InChattingPlayerController, int InStrikeCount)
{
	AMP_BaseBallPlayerState* MPPS = InChattingPlayerController->GetPlayerState<AMP_BaseBallPlayerState>();
	if (MPPS->PlayerGameState == EGameState::None)
	{
		for (const auto& MPPlayerController : AllPlayerControllers)
		{
			AMP_BaseBallPlayerState* MPPlayerState = MPPlayerController->GetPlayerState<AMP_BaseBallPlayerState>();
			if (IsValid(MPPlayerState) == true)
			{
				if (MPPlayerState->PlayerGameState != EGameState::None)
					return false;
			}
		}

		return true;
	}

	bool CheckEndGame = true;

	if (3 == InStrikeCount)
	{
		MPPS->SetEndPlay();
		for (const auto& MPPlayerController : AllPlayerControllers)
		{
			if (IsValid(MPPS) == true)
			{
				FString CombinedMessageString = MPPS->PlayerNameString + TEXT(" has End the game.");
				MPPlayerController->NotificationText = FText::FromString(CombinedMessageString);
			}

			AMP_BaseBallPlayerState* MPPlayerState = MPPlayerController->GetPlayerState<AMP_BaseBallPlayerState>();
			if (IsValid(MPPlayerState) == true)
			{
				if (CheckEndGame && MPPlayerState->PlayerGameState != EGameState::None)
					CheckEndGame = false;
			}
		}
	}

	return CheckEndGame;
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
			FString CombinedMessageString = TEXT(" Return ");
			MPPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
			return;
		}
	}

	StartGame();
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
			AMP_BaseBallPlayerState* BaseBallPlayerState = MPPlayerController->GetPlayerState<AMP_BaseBallPlayerState>();
			if (IsValid(BaseBallPlayerState))
			{
				FString CombinedMessageString = BaseBallPlayerState->PlayerNameString + TEXT("'s Turn");
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

void AMP_BaseBallGameModeBase::IncreaseGuessCount(AMP_BaseBallPlayerController* InChattingPlayerController)
{
	AMP_BaseBallPlayerState* BaseBallPlayerState = InChattingPlayerController->GetPlayerState<AMP_BaseBallPlayerState>();
	if (IsValid(BaseBallPlayerState) == true)
	{
		BaseBallPlayerState->CurrentGuessCount++;
	}
}
