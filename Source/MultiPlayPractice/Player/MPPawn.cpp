// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MPPawn.h"
#include <MultiPlayPractice.h>

// Sets default values
AMPPawn::AMPPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AMPPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetModeString = MultiPlayPracticeFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CXPawn::BeginPlay() %s [%s]"), *MultiPlayPracticeFunctionLibrary::GetNetModeString(this), *NetModeString);
	MultiPlayPracticeFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
	
}

void AMPPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetModeString = MultiPlayPracticeFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CXPawn::PossessedBy() %s [%s]"), *MultiPlayPracticeFunctionLibrary::GetNetModeString(this), *NetModeString);
	MultiPlayPracticeFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}
