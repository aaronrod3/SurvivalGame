// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryStatics.h"

#include "SurvivalGame/Character/Inventory/InventoryComponent.h"


UInventoryComponent* UInventoryStatics::GetInventoryComponent(const APlayerController* PlayerController)
{
	if (!IsValid(PlayerController)) return nullptr;
	
	UInventoryComponent* InventoryComponent = PlayerController->FindComponentByClass<UInventoryComponent>();
	return InventoryComponent;
}

EItem_Category UInventoryStatics::GetItemCategoryFromItemComp(UItemComponent* ItemComp)
{
	if (!IsValid(ItemComp)) return EItem_Category::None;
	
	return ItemComp->GetItemManifest().GetItemCategory();
}

