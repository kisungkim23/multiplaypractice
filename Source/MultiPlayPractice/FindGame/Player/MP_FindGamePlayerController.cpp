// Fill out your copyright notice in the Description page of Project Settings.


#include "FindGame/Player/MP_FindGamePlayerController.h"
#include "EnhancedInputSubSystems.h"

AMP_FindGamePlayerController::AMP_FindGamePlayerController()
	:InputMappingContext(nullptr),
	MoveAction(nullptr),
	LookAction(nullptr),
	JumpAction(nullptr),
	AttackAction(nullptr)
{
}

void AMP_FindGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				EnhancedSubsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

}
