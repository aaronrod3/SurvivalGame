// Fill out your copyright notice in the Description page of Project Settings.


#include "SpatialInventory.h"

#include "SurvivalGame/Character/Inventory/InventoryComponent.h"
#include "SurvivalGame/Core/Data/Items/Manifest/ItemManifest.h"
#include "SurvivalGame/Core/Data/Items/ItemComponent.h"
#include "SurvivalGame/Core/UI/Utilities/InventoryStatics.h"


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
	// TODO: From the active grid thats being used, call DropItem();
	return FReply::Handled();
	
	
	
}

FSlotAvailabilityResult USpatialInventory::HasRoomForItem(UItemComponent* ItemComponent) const
{
	const FItemManifest& ItemManifest = ItemComponent->GetItemManifest();
	const FItemPlacementRules& Rules = ItemManifest.GetPlacementRules();
	
	// Priority 1: if equipment, try equipment slot first
	if (Rules.EquipmentSlot != EItem_Category::None)
	{
		UInventoryGrid* EquipmentGrid = GetGridForEquipmentSlot(Rules.EquipmentSlot);
		if (EquipmentGrid)
		{
			FSlotAvailabilityResult Result = EquipmentGrid->HasRoomForItem(ItemComponent);
			if (Result.TotalRoomToFill > 0) return Result;
		}
	}
	
	// Priority 2: try storage grids
	if (Rules.bCanGoInStorage)
	{
		for (UInventoryGrid* StorageGrid : GetAllStorageGrids())
		{
			if (!StorageGrid) continue;
            
			FSlotAvailabilityResult Result = StorageGrid->HasRoomForItem(ItemComponent);
			if (Result.TotalRoomToFill > 0)
			{
				return Result;  // Storage slot has room
			}
		}
	}
	
	// No room found
	return FSlotAvailabilityResult();
}

// Get equipment grid by slot type
UInventoryGrid* USpatialInventory::GetGridForEquipmentSlot(EItem_Category EquipmentSlot) const
{
	switch (EquipmentSlot)
	{
		case EItem_Category::Head:     return Grid_Head;
		case EItem_Category::Backpack: return Grid_Backpack;
		case EItem_Category::Rig:      return Grid_Rig;
		case EItem_Category::Belt:     return Grid_Belt;
			// ... etc
		default: return nullptr;
	}
}

// Get all storage grids
TArray<UInventoryGrid*> USpatialInventory::GetAllStorageGrids() const
{
	return {
		Grid_Rig_Slots,
		Grid_Backpack_Slots,
		Grid_Belt_Slots,
		Grid_Pocket_Slots
	};
}















