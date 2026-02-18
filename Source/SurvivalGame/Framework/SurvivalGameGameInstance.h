// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SurvivalGameGameInstance.generated.h"

// Forward declare classes
class ASurvivalPlayerController;
class UObject; 

DECLARE_LOG_CATEGORY_EXTERN(LogSurvivalGameGameInstance, Log, All);

UCLASS()
class SURVIVALGAME_API USurvivalGameGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// Called to get initialize game instance object
	USurvivalGameGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	// called to retrieve the primary player controller
	ASurvivalPlayerController* GetPrimaryPlayerController() const;
	
protected:
	// called to initialize game instance on game startup
	virtual void Init() override;
	
	// called to shutdown game instance on game exit
	virtual void Shutdown() override;
	
};
