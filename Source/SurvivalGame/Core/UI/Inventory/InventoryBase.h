// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurvivalGame/Core/UI/Inventory/Types/GridTypes.h"
#include "InventoryBase.generated.h"

class UItemComponent;
class APickupBase;
/**
 * 
 */
UCLASS()
class SURVIVALGAME_API UInventoryBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual FSlotAvailabilityResult HasRoomForItem(UItemComponent* ItemComponent) const {return FSlotAvailabilityResult();}
	
	
private:


};
