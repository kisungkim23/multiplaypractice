// Fill out your copyright notice in the Description page of Project Settings.


#include "FindGame/Player/MP_FindGameCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "AbilitySystemComponent.h"
#include "FindGame/Attribute/BasicAttributeSet.h"
#include <EnhancedInputComponent.h>
#include "MP_FindGamePlayerController.h"

// Sets default values
AMP_FindGameCharacter::AMP_FindGameCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

// Called when the game starts or when spawned
void AMP_FindGameCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(AbilitySystemComponent))
	{
		BasicAttributeSet = AbilitySystemComponent->GetSet<UBasicAttributeSet>();
	}
}

// Called every frame
void AMP_FindGameCharacter::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMP_FindGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMP_FindGamePlayerController* PlayerController = Cast<AMP_FindGamePlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AMP_FindGameCharacter::Move
				);
				EnhancedInput->BindAction(PlayerController->MoveAction,
					ETriggerEvent::Completed,
					this,
					&AMP_FindGameCharacter::StopMove
				);
			}
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AMP_FindGameCharacter::Look
				);
			}
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&AMP_FindGameCharacter::StartJump
				);
				EnhancedInput->BindAction(PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&AMP_FindGameCharacter::StopJump
				);
			}
			if (PlayerController->AttackAction)
			{
				EnhancedInput->BindAction(PlayerController->AttackAction,
					ETriggerEvent::Triggered,
					this,
					&AMP_FindGameCharacter::CustomAttack
				);
			}
		}
	}
}

UAbilitySystemComponent* AMP_FindGameCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMP_FindGameCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (!Controller)
	{
		return;
	}

	const FRotator ControlRot = Controller->GetControlRotation();
	const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

	const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	if (!FMath::IsNearlyZero(MovementVector.Y))
	{
		AddMovementInput(ForwardDir, MovementVector.Y);
	}

	if (!FMath::IsNearlyZero(MovementVector.X))
	{
		AddMovementInput(RightDir, MovementVector.X);
	}
}

void AMP_FindGameCharacter::StopMove()
{
}

void AMP_FindGameCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxis = Value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(LookAxis.X))
	{
		AddControllerYawInput(LookAxis.X);
	}

	if (!FMath::IsNearlyZero(LookAxis.Y))
	{
		AddControllerPitchInput(LookAxis.Y);
	}
}

void AMP_FindGameCharacter::StartJump()
{
	Jump();
}

void AMP_FindGameCharacter::StopJump()
{
	StopJumping();
}

void AMP_FindGameCharacter::CustomAttack()
{
}

