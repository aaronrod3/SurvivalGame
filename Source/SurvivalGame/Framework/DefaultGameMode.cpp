// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameMode.h"
#include "SurvivalGame/Character/PlayerController/MyPlayerController.h"
#include "SurvivalGame/Framework/SurvivalGameGameInstance.h"
#include "UObject/ConstructorHelpers.h" // optional if later set up a BP Pawn

ADefaultGameMode::ADefaultGameMode()
{
	
	// Assign player controller class
	PlayerControllerClass = AMyPlayerController::StaticClass();
}

void ADefaultGameMode::StartPlay()
{
	Super::StartPlay();

	check(GEngine != nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Game Started"));
	
	if (USurvivalGameGameInstance* GameInstance =
		GetGameInstance<USurvivalGameGameInstance>())
	{
		UE_LOG(LogSurvivalGameGameInstance, Log,
			   TEXT("StartPlay called on SurvivalGameGameMode"));
	}
}
