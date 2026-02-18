// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SurvivalGame/Core/UI/HUD/MainHUDWidget.h"
#include "SurvivalPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UMainHUDWidget;
class UInventoryComponent;

UCLASS(Blueprintable, BlueprintType)
class SURVIVALGAME_API ASurvivalPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASurvivalPlayerController();
	
	virtual void Tick( float DeltaTime ) override;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	/*** INPUT ***/
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;
	
	// Interact Action
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> PrimaryInteractAction;
	
	// Toggle inventory action now owned by playercontroller
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleInventoryAction;
	
	
	/*** INVENTORY ***/
	// Weak pointer reference to inventory
	TWeakObjectPtr<UInventoryComponent> InventoryComponent;
	
	// Interact
	void PrimaryInteract();
	
	// Toggle Inventory
	UFUNCTION(BlueprintCallable, Category = "Input")
	void ToggleInventory();
	
	/*** UI ***/
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UMainHUDWidget> HUDWidgetClass;
	UPROPERTY()
	TObjectPtr<UMainHUDWidget> HUDWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	double TraceLength;
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TEnumAsByte<ECollisionChannel> TraceChannel;
	
	// current/last actor hit by line trace
	TWeakObjectPtr<AActor> ThisActor;
	TWeakObjectPtr<AActor> LastActor;
	
	
	// create hud
	void CreateHUDWidget();
	void TraceForItem();
	
};
