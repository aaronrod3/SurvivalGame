// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalGameGameInstance.h"
#include "SurvivalGame/Character/PlayerController/SurvivalPlayerController.h"

// Defines the log category 
// w/out this the linker would fail
DEFINE_LOG_CATEGORY(LogSurvivalGameGameInstance);


// Initialize Game Instance object
void USurvivalGameGameInstance::Init()
{
	// log when game instance is initialized
	UE_LOG(LogSurvivalGameGameInstance, Log, TEXT("Survival Game Instance Initialized"));
	
	// calls the base USurvivalGameInstance to ensure the engine performs its default initilization
	Super::Init();
}


// Shutdown Game instance object
void USurvivalGameGameInstance::Shutdown()
{
	// log when game instance is shutdown
	UE_LOG(LogSurvivalGameGameInstance, Log, TEXT("Survival Game Instance Shutdown"));
	
	// calls the base USurvivalGameInstance to ensure the engine performs its default shutdown
	Super::Shutdown();
}

// class constructor
USurvivalGameGameInstance::USurvivalGameGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}


// get primary controller
ASurvivalPlayerController* USurvivalGameGameInstance::GetPrimaryPlayerController() const
{
	// safely cast the primary controller from the base type to your custom controller type
	return Cast<ASurvivalPlayerController>(Super::GetPrimaryPlayerController(false));
	
}
















