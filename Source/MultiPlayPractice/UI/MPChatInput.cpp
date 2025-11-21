// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MPChatInput.h"

#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Player/MP_BaseBallPlayerController.h"

void UMPChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}

	if (Button_ReadyButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnSetReady) == false)
	{
		Button_ReadyButton->OnClicked.AddDynamic(this, &ThisClass::OnSetReady);
	}
}

void UMPChatInput::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
	if (Button_ReadyButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnSetReady) == true)
	{
		Button_ReadyButton->OnClicked.RemoveDynamic(this, &ThisClass::OnSetReady);
	}
}

void UMPChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APlayerController* OwningPlayerController = GetOwningPlayer();
		if (IsValid(OwningPlayerController) == true)
		{
			AMP_BaseBallPlayerController* OwningMPPlayerController = Cast<AMP_BaseBallPlayerController>(OwningPlayerController);
			if (IsValid(OwningMPPlayerController) == true)
			{
				OwningMPPlayerController->SetChatMessageString(Text.ToString());

				EditableTextBox_ChatInput->SetText(FText());
			}
		}
	}
}

void UMPChatInput::OnSetReady()
{
	APlayerController* OwningPlayerController = GetOwningPlayer();
	if (IsValid(OwningPlayerController) == true)
	{
		AMP_BaseBallPlayerController* OwningMPPlayerController = Cast<AMP_BaseBallPlayerController>(OwningPlayerController);
		if (IsValid(OwningMPPlayerController) == true)
		{
			OwningMPPlayerController->SetGameReady();
		}
	}
}
