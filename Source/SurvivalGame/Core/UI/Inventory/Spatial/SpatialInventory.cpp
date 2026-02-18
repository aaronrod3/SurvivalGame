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



bool USpatialInventory::AssignItemToQuickSlot(UInventoryItem* Item, UInventoryGrid* SourceGrid, int32 StorageIndex, EQuickSlotType SlotType)
{
	if (!Item || !SourceGrid || StorageIndex == INDEX_NONE)
	{
		return false;
	}
	
	// Check if item can be assigned to this slot
	if (!CanAssignToQuickSlot(Item, SlotType))
	{
		return false;
	}
	
	// Get or create slot reference
	FQuickSlotReference& SlotRef = QuickSlotReferences.FindOrAdd(SlotType);
	
	// If slot is occupied with a different item, clear it first
	if (SlotRef.IsValid() && SlotRef.ItemReference != Item)
	{
		SlotRef.Clear();
	}
	
	// Assign new item
	SlotRef.ItemReference = Item;
	SlotRef.SourceGrid = SourceGrid;
	SlotRef.StorageGridIndex = StorageIndex;
	SlotRef.bIsOccupied = true;
	SlotRef.SlotType = SlotType;
	
	// Broadcast update
	OnQuickSlotUpdated.Broadcast(SlotType, Item);
	
	// Exit assignment mode if active
	if (bInQuickSlotAssignmentMode)
	{
		CancelQuickSlotAssignment();
	}
	
	return true;
}

void USpatialInventory::ClearQuickSlot(EQuickSlotType SlotType)
{
	if (FQuickSlotReference* SlotRef = QuickSlotReferences.Find(SlotType))
	{
		SlotRef->Clear();
		OnQuickSlotUpdated.Broadcast(SlotType, nullptr);
	}
}

UInventoryItem* USpatialInventory::GetItemFromQuickSlot(EQuickSlotType SlotType) const
{
	if (const FQuickSlotReference* SlotRef = QuickSlotReferences.Find(SlotType))
	{
		if (SlotRef->IsValid())
		{
			return SlotRef->ItemReference.Get();
		}
	}
	return nullptr;
}

void USpatialInventory::UpdateQuickSlotReference(UInventoryGrid* Grid, int32 OldIndex, int32 NewIndex)
{
	// Update all quick slots that reference this grid and index
	for (TPair<EQuickSlotType, FQuickSlotReference>& Pair : QuickSlotReferences)
	{
		FQuickSlotReference& SlotRef = Pair.Value;
		
		if (SlotRef.SourceGrid == Grid && SlotRef.StorageGridIndex == OldIndex)
		{
			if (NewIndex == INDEX_NONE)
			{
				// Item was removed - try to find replacement
				if (UInventoryItem* Replacement = FindReplacementItem(SlotRef.ItemReference.Get(), SlotRef.SourceGrid, SlotRef.StorageGridIndex))
				{
					SlotRef.ItemReference = Replacement;
					OnQuickSlotUpdated.Broadcast(Pair.Key, Replacement);
				}
				else
				{
					// No replacement found, clear slot
					SlotRef.Clear();
					OnQuickSlotUpdated.Broadcast(Pair.Key, nullptr);
				}
			}
			else
			{
				// Item moved to new index
				SlotRef.StorageGridIndex = NewIndex;
				OnQuickSlotUpdated.Broadcast(Pair.Key, SlotRef.ItemReference.Get());
			}
		}
	}
}

void USpatialInventory::OnItemRemovedFromGrid(UInventoryGrid* Grid, int32 GridIndex)
{
	UpdateQuickSlotReference(Grid, GridIndex, INDEX_NONE);
}

void USpatialInventory::BeginQuickSlotAssignment(EQuickSlotType TargetSlot)
{
	bInQuickSlotAssignmentMode = true;
	PendingQuickSlotType = TargetSlot;
	
	// TODO: Update cursor or UI to show assignment mode
}

void USpatialInventory::CancelQuickSlotAssignment()
{
	bInQuickSlotAssignmentMode = false;
	PendingQuickSlotType = EQuickSlotType::Slot_1;
	
	// TODO: Reset cursor or UI
}

bool USpatialInventory::CanAssignToQuickSlot(const UInventoryItem* Item, EQuickSlotType SlotType) const
{
	if (!Item)
	{
		return false;
	}
	
	const FItemManifest& Manifest = Item->GetItemManifest();
	const FItemPlacementRules& Rules = Manifest.GetPlacementRules();
	const EItem_Category Category = Rules.EquipmentSlot;
	
	// Weapon/Equipment slots can only accept their specific types
	switch (SlotType)
	{
		case EQuickSlotType::Weapon_Primary:
			return Category == EItem_Category::Weapon_Primary;
			
		case EQuickSlotType::Weapon_Holster:
			return Category == EItem_Category::Weapon_Holster;
			
		case EQuickSlotType::Weapon_Secondary:
			// Secondary slot accepts both Primary and Secondary weapons
			return Category == EItem_Category::Weapon_Primary || Category == EItem_Category::Weapon_Secondary;
			
		case EQuickSlotType::Tool:
			return Category == EItem_Category::Tool;
			
		// Numbered slots accept only usable items with quick slot permission
		default:
			return Rules.bCanGoInQuickSlot;
	}
}

UInventoryItem* USpatialInventory::FindReplacementItem(const UInventoryItem* ConsumedItem, UInventoryGrid*& OutGrid, int32& OutIndex)
{
	if (!ConsumedItem)
	{
		return nullptr;
	}
	
	// Get the item type we're looking for
	const FGameplayTag& ItemType = ConsumedItem->GetItemManifest().GetItemType();
	
	// Search through all grids for another item of the same type
	// TODO: Implement based on your inventory component structure
	// You'll need to iterate through your grids and find matching items
	
	// Placeholder implementation:
	// for (UInventoryGrid* Grid : AllGrids)
	// {
	//     for (int32 Index = 0; Index < Grid->GetSlotCount(); ++Index)
	//     {
	//         if (UInventoryItem* Item = Grid->GetItemAtIndex(Index))
	//         {
	//             if (Item->GetItemManifest().GetItemType() == ItemType)
	//             {
	//                 OutGrid = Grid;
	//                 OutIndex = Index;
	//                 return Item;
	//             }
	//         }
	//     }
	// }
	
	return nullptr;
}











