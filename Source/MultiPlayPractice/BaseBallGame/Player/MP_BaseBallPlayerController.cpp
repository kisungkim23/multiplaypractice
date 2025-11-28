// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBallGame/Player/MP_BaseBallPlayerController.h"


#include "BaseBallGame/UI/MPChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BaseBallGame/Game/MP_BaseBallGameModeBase.h"
#include "BaseBallGame/Player/MP_BaseBallPlayerState.h"
#include "MultiPlayPractice.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

AMP_BaseBallPlayerController::AMP_BaseBallPlayerController()
{
	bReplicates = true;
}

void AMP_BaseBallPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UMPChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}

}

void AMP_BaseBallPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	if (IsLocalController() == true)
	{
		AMP_BaseBallPlayerState* MPPS = GetPlayerState<AMP_BaseBallPlayerState>();
		if (IsValid(MPPS) == true)
		{
			FString CombinedMessageString = MPPS->PlayerNameString + TEXT(": ") + InChatMessageString;

			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void AMP_BaseBallPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	MultiPlayPracticeFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void AMP_BaseBallPlayerController::SetGameReady()
{
	if (IsLocalController())
	{
		ServerSetGameReady();
	}
}

void AMP_BaseBallPlayerController::ServerSetGameReady_Implementation()
{
	AMP_BaseBallPlayerState* MPPS = GetPlayerState<AMP_BaseBallPlayerState>();
	if (IsValid(MPPS) == true)
	{
		MPPS->SetPlayerReady();
	}
	NotificationText = FText::FromString("Ready!");

	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		AMP_BaseBallGameModeBase* MPGM = Cast<AMP_BaseBallGameModeBase>(GM);
		if (IsValid(MPGM) == true)
		{
			MPGM->CheckStartGame();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Test"));
	}
}

void AMP_BaseBallPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
	DOREPLIFETIME(ThisClass, TurnCount);
}

void AMP_BaseBallPlayerController::SetTurnCount(int inCount)
{
	TurnCount = inCount;
}

void AMP_BaseBallPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		AMP_BaseBallGameModeBase* MPGM = Cast<AMP_BaseBallGameModeBase>(GM);
		if (IsValid(MPGM) == true)
		{
			MPGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}

void AMP_BaseBallPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}
