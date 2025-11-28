// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MP_BaseBallPlayerController.generated.h"

class UMPChatInput;
class UUserWidget;
/**
 *
 */
UCLASS()
class MULTIPLAYPRACTICE_API AMP_BaseBallPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMP_BaseBallPlayerController();

	virtual void BeginPlay() override;

	void SetChatMessageString(const FString& InChatMessageString);

	void PrintChatMessageString(const FString& InChatMessageString);

	void SetGameReady();

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerSetGameReady();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void SetTurnCount(int inCount);
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMPChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UMPChatInput> ChatInputWidgetInstance;

	FString ChatMessageString;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;

public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int TurnCount;

};
