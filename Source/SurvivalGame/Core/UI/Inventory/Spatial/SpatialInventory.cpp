// Fill out your copyright notice in the Description page of Project Settings.


#include "SpatialInventory.h"
#include "SurvivalGame/Character/Inventory/InventoryComponent.h"
#include "SurvivalGame/Core/Data/Items/Manifest/ItemManifest.h"
#include "SurvivalGame/Core/Data/Items/InventoryItem.h"
#include "SurvivalGame/Core/Data/Items/ItemComponent.h"
#include "SurvivalGame/Core/UI/Inventory/Spatial/InventoryGrid.h"


void USpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Grid_Head->SetOwningCanvas(CanvasPanel);
	Grid_Ears->SetOwningCanvas(CanvasPanel);
	Grid_Face->SetOwningCanvas(CanvasPanel);
	Grid_Eye->SetOwningCanvas(CanvasPanel);
	Grid_Armband->SetOwningCanvas(CanvasPanel);
	Grid_Shirt->SetOwningCanvas(CanvasPanel);
	Grid_Pants->SetOwningCanvas(CanvasPanel);
	Grid_Rig->SetOwningCanvas(CanvasPanel);
	Grid_Rig_Slots->SetOwningCanvas(CanvasPanel);
	Grid_Backpack->SetOwningCanvas(CanvasPanel);
	Grid_Backpack_Slots->SetOwningCanvas(CanvasPanel);
	Grid_Belt->SetOwningCanvas(CanvasPanel);
	Grid_Belt_Slots->SetOwningCanvas(CanvasPanel);
	Grid_Pocket_Slots->SetOwningCanvas(CanvasPanel);
	
	
}

FReply USpatialInventory::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Handled();
	
}

FSlotAvailabilityResult USpatialInventory::HasRoomForItem(UItemComponent* ItemComponent) const
{
	//TODO: Set up switch statement to go through item categories and place items accordingly
	
	
	// No room found
	return FSlotAvailabilityResult();
}





