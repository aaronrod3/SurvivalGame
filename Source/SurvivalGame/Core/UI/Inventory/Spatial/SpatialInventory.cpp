// Fill out your copyright notice in the Description page of Project Settings.


#include "SpatialInventory.h"
#include "SurvivalGame/Character/Inventory/InventoryComponent.h"
#include "SurvivalGame/Core/Data/Items/Manifest/ItemManifest.h"
#include "SurvivalGame/Core/Data/Items/ItemComponent.h"
#include "SurvivalGame/Core/UI/Inventory/Types/QuickSlotTypes.h"
#include "SurvivalGame/Core/Data/Items/InventoryItem.h"
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
	
	Grid_Rig_Slots->SetSpatialInventory(this);
	Grid_Belt_Slots->SetSpatialInventory(this);
	Grid_Weapon_Primary->SetSpatialInventory(this);
	Grid_Weapon_Holster->SetSpatialInventory(this);
	Grid_Weapon_Secondary->SetSpatialInventory(this);
	Grid_Tool->SetSpatialInventory(this);
	
	// Bind weapon grid auto-populate callbacks
	BindWeaponGridCallbacks();
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



/*
 * QUICK SLOT - Auto Populate (1-4)
 */

void USpatialInventory::BindWeaponGridCallbacks()
{
	if (IsValid(Grid_Weapon_Primary) && IsValid(Grid_Weapon_Primary->GetInventoryComponent()))
	{
		Grid_Weapon_Primary->GetInventoryComponent()->OnItemAdded.AddDynamic(this, &ThisClass::OnWeaponPrimaryItemAdded);
	}
	
	if (IsValid(Grid_Weapon_Holster) && IsValid(Grid_Weapon_Holster->GetInventoryComponent()))
	{
		Grid_Weapon_Holster->GetInventoryComponent()->OnItemAdded.AddDynamic(this, &ThisClass::OnWeaponHolsterItemAdded);
	}
	
	if (IsValid(Grid_Weapon_Secondary) && IsValid(Grid_Weapon_Secondary->GetInventoryComponent()))
	{
		Grid_Weapon_Secondary->GetInventoryComponent()->OnItemAdded.AddDynamic(this, &ThisClass::OnWeaponSecondaryItemAdded);
	}
	
	if (IsValid(Grid_Tool) && IsValid(Grid_Tool->GetInventoryComponent()))
	{
		Grid_Tool->GetInventoryComponent()->OnItemAdded.AddDynamic(this, &ThisClass::OnToolItemAdded);
	}
}

void USpatialInventory::OnWeaponPrimaryItemAdded(UInventoryItem* Item)
{
	AutoAssignWeaponSlot(Item, Grid_Weapon_Primary, EQuickSlotType::Weapon_Primary);
}

void USpatialInventory::OnWeaponHolsterItemAdded(UInventoryItem* Item)
{
	AutoAssignWeaponSlot(Item, Grid_Weapon_Holster, EQuickSlotType::Weapon_Holster);
}

void USpatialInventory::OnWeaponSecondaryItemAdded(UInventoryItem* Item)
{
	AutoAssignWeaponSlot(Item, Grid_Weapon_Secondary, EQuickSlotType::Weapon_Secondary);
}

void USpatialInventory::OnToolItemAdded(UInventoryItem* Item)
{
	AutoAssignWeaponSlot(Item, Grid_Tool, EQuickSlotType::Tool);
}

void USpatialInventory::AutoAssignWeaponSlot(UInventoryItem* Item, UInventoryGrid* SourceGrid, EQuickSlotType TargetSlot)
{
	if (!IsValid(Item) || !IsValid(SourceGrid)) return;
	
	const int32 SourceIndex = SourceGrid->FindUpperLeftIndexForItem(Item);
	
	if (SourceIndex == INDEX_NONE) return;
	
	FQuickSlotReference Ref;
	Ref.SlotType = TargetSlot;
	Ref.ItemReference = Item;
	Ref.SourceGrid = SourceGrid;
	Ref.StorageGridIndex = SourceIndex;
	Ref.bIsOccupied = true;
	
	AssignQuickSlot(Ref);
}

/*
 * QUICK SLOT - Consumables (5-0)
 */

bool USpatialInventory::TryAssignConsumableQuickSlot(UInventoryItem* Item, UInventoryGrid* SourceGrid, int32 SourceIndex, EQuickSlotType TargetSlot)
{
	if (!IsValid(Item) || !IsValid(SourceGrid)) return false;
	
	if (TargetSlot == EQuickSlotType::None) return false;
	
	// Slots 1-4 are weapon only, not assignable via drag
	if (TargetSlot == EQuickSlotType::Weapon_Primary || 
		TargetSlot == EQuickSlotType::Weapon_Holster || 
		TargetSlot == EQuickSlotType::Weapon_Secondary || 
		TargetSlot == EQuickSlotType::Tool)
	{
		return false;
	}
	
	if (!IsConsumableQuickSlottable(Item, SourceGrid)) return false;
	
	FQuickSlotReference Ref;
	Ref.SlotType = TargetSlot;
	Ref.ItemReference = Item;
	Ref.SourceGrid = SourceGrid;
	Ref.StorageGridIndex = SourceIndex;
	Ref.bIsOccupied = true;
	
	AssignQuickSlot(Ref);
	return true;
}

bool USpatialInventory::IsConsumableQuickSlottable(const UInventoryItem* Item, const UInventoryGrid* SourceGrid) const
{
	if (!IsValid(Item) || !IsValid(SourceGrid)) return false;
	
	// Must come from rig or belt only
	if (!IsConsumableSourceGrid(SourceGrid)) return false;
	
	const FItemManifest& Manifest = Item->GetItemManifest();
	const FItemPlacementRules& Rules = Manifest.GetPlacementRules();
	
	return IsConsumableCategory(Rules.EquipmentSlot);
}

bool USpatialInventory::IsConsumableSourceGrid(const UInventoryGrid* SourceGrid) const
{
	return SourceGrid == Grid_Rig_Slots || SourceGrid == Grid_Belt_Slots;
}

bool USpatialInventory::IsConsumableCategory(EItem_Category Category) const
{
	switch (Category)
	{
	case EItem_Category::Usable_Medical:
	case EItem_Category::Usable_Food:
		return true;
	default: 
		return false;
	}
}


/*
 * Quick slot - Core Assign / Clear
 */

void USpatialInventory::AssignQuickSlot(const FQuickSlotReference& Reference)
{
	QuickSlotReferences.Add(Reference.SlotType, Reference);
	OnQuickSlotUpdated.Broadcast(Reference.SlotType, Reference.ItemReference.Get());
}


bool USpatialInventory::AssignItemToQuickSlot(UInventoryItem* Item, UInventoryGrid* SourceGrid, int32 StorageIndex, EQuickSlotType SlotType)
{
	if (!Item || !SourceGrid || StorageIndex == INDEX_NONE) return false;
	
	if (!CanAssignToQuickSlot(Item, SlotType)) return false;
	
	FQuickSlotReference Ref;
	Ref.SlotType = SlotType;
	Ref.ItemReference = Item;
	Ref.SourceGrid = SourceGrid;
	Ref.StorageGridIndex = StorageIndex;
	Ref.bIsOccupied = true;
	
	AssignQuickSlot(Ref);
	
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

const FQuickSlotReference* USpatialInventory::GetQuickSlotReference(EQuickSlotType SlotType) const
{
	return QuickSlotReferences.Find(SlotType);
}

UInventoryItem* USpatialInventory::GetItemFromQuickSlot(EQuickSlotType SlotType) const
{
	if (const FQuickSlotReference* SlotReference = QuickSlotReferences.Find(SlotType))
	{
		if (SlotReference->IsValid())
		{
			return SlotReference->ItemReference.Get();
		}
	}
	return nullptr;
}



/*
 * Quick Slot Reference Sync (item move/removed from grid)
 */


void USpatialInventory::OnItemRemovedFromGrid(UInventoryGrid* SourceGrid, int32 SourceIndex)
{
	UpdateQuickSlotReference(SourceGrid, SourceIndex, INDEX_NONE);
}

void USpatialInventory::UpdateQuickSlotReference(UInventoryGrid* SourceGrid, int32 OldIndex, int32 NewIndex)
{
	if (!IsValid(SourceGrid)) return;
	
	for (TPair<EQuickSlotType, FQuickSlotReference>& Pair : QuickSlotReferences)
	{
		FQuickSlotReference& SlotRef = Pair.Value;
		
		if (SlotRef.SourceGrid != SourceGrid) continue;
		if (SlotRef.StorageGridIndex != OldIndex) continue;
		
		// Item was removed by grid: try to find a replacement of the same type elsewhere
		if (NewIndex != INDEX_NONE)
		{
			TWeakObjectPtr<UInventoryGrid> ReplacementGrid = nullptr;
			int32 ReplacementIndex = INDEX_NONE;
			
			UInventoryItem* ReplacementItem = FindReplacementItem(SlotRef.ItemReference.Get(), ReplacementGrid, ReplacementIndex);
			
			if (IsValid(ReplacementItem) && ReplacementGrid.IsValid() && ReplacementIndex != INDEX_NONE)
			{
				SlotRef.ItemReference = ReplacementItem;
				SlotRef.SourceGrid = ReplacementGrid.Get();
				SlotRef.StorageGridIndex = ReplacementIndex;
				SlotRef.bIsOccupied = true;
				
				OnQuickSlotUpdated.Broadcast(Pair.Key, ReplacementItem);
			}
			else
			{
				SlotRef.Clear();
				OnQuickSlotUpdated.Broadcast(Pair.Key, nullptr);
			}
			return;
		}
		
		// item moved with same grid: update index
		SlotRef.StorageGridIndex = NewIndex;
		SlotRef.bIsOccupied = true;
		
		OnQuickSlotUpdated.Broadcast(Pair.Key, SlotRef.ItemReference.Get());
		return;
	}
}


/*
 * Quick Slot - Assignment mode, no longer needed once drag/drop is fully implemented
 */


void USpatialInventory::BeginQuickSlotAssignment(EQuickSlotType TargetSlot)
{
	bInQuickSlotAssignmentMode = true;
	PendingQuickSlotType = TargetSlot;
	// TODO: Update cursor or UI to show assignment mode
}

void USpatialInventory::CancelQuickSlotAssignment()
{
	bInQuickSlotAssignmentMode = false;
	PendingQuickSlotType = EQuickSlotType::None; 
}


/*
 * Quick Slot - Validation
 */

bool USpatialInventory::CanAssignToQuickSlot(const UInventoryItem* Item, EQuickSlotType SlotType) const
{
	if (!Item) return false;
	
	const FItemManifest& Manifest = Item->GetItemManifest();
	const FItemPlacementRules& Rules = Manifest.GetPlacementRules();
	const EItem_Category Category = Rules.EquipmentSlot;
	
	switch (SlotType)
	{
	case EQuickSlotType::Weapon_Primary:
		return Category == EItem_Category::Weapon_Primary;
	case EQuickSlotType::Weapon_Holster:
		return Category == EItem_Category::Weapon_Holster;
	case EQuickSlotType::Weapon_Secondary:
		return Category == EItem_Category::Weapon_Primary
				|| Category == EItem_Category::Weapon_Secondary;
	case EQuickSlotType::Tool:
		return Category == EItem_Category::Tool;
	default:
		// Slots 5-0 must be consumable
		return IsConsumableCategory(Category);
	}
}


/*
 * Quick Slot - replacement search
 */

UInventoryItem* USpatialInventory::FindReplacementItem(const UInventoryItem* ConsumedItem, TWeakObjectPtr<UInventoryGrid>& OutGrid, int32& OutIndex)
{
	if (!ConsumedItem) return nullptr;
	
	// TODO: iterate grids and find matching item type
	// const FGameplayTag& ItemType = ConsumedItem->GetItemManifest().GetItemType();
	
	return nullptr;
}



