// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryGrid.h"
#include "SurvivalGame/Core/UI/Inventory/InventoryBase.h"
#include "SurvivalGame/Core/UI/Inventory/Types/QuickSlotTypes.h"
#include "SpatialInventory.generated.h"

class UCanvasPanel;

// Delegate to broadcast when quick slot is updated (for UI updates)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuickSlotUpdated, EQuickSlotType, SlotType, UInventoryItem*, Item);

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API USpatialInventory : public UInventoryBase
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	virtual FSlotAvailabilityResult HasRoomForItem(UItemComponent* ItemComponent) const override;
	
	// Helper methods
	UInventoryGrid* GetGridForEquipmentSlot(EItem_Category Slot) const;
	TArray<UInventoryGrid*> GetAllStorageGrids() const;
	
	/**
	* Quick Slot Management
	*/
	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	bool TryAssignQuickSlotFromGrid(UInventoryItem* Item, UInventoryGrid* SourceGrid, int32 SourceIndex, EQuickSlotType TargetSlot);
	
	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	bool AssignItemToQuickSlot(UInventoryItem* Item, UInventoryGrid* SourceGrid, int32 StorageIndex, EQuickSlotType SlotType);

	

	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	UInventoryItem* GetItemFromQuickSlot(EQuickSlotType SlotType) const;

	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	void UpdateQuickSlotReference(UInventoryGrid* Grid, int32 OldIndex, int32 NewIndex);

	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	void OnItemRemovedFromGrid(UInventoryGrid* Grid, int32 GridIndex);
	
	/**
	* Enable assignment mode - next item clicked will be assigned to this slot
	*/
	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	void BeginQuickSlotAssignment(EQuickSlotType TargetSlot);

	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	void CancelQuickSlotAssignment();

	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	bool IsInAssignmentMode() const { return bInQuickSlotAssignmentMode; }

	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	EQuickSlotType GetPendingAssignmentSlot() const { return PendingQuickSlotType; }
	
	
	// Called by quickslot bar widget OnDrop (consumable slots 5-0)
	UFUNCTION(BlueprintCallable, Category = "Inventory | QuickSlots")
	bool TryAssignConsumableQuickSlot(UInventoryItem* Item, UInventoryGrid* SourceGrid, int32 SourceIndex, EQuickSlotType TargetSlot);
	
	// Read a slot reference
	const FQuickSlotReference& GetQuickSlotReference(EQuickSlotType SlotType) const;
	

	
	
private:
	
	/**
	* Check if an item can be assigned to a specific quick slot
	*/
	bool CanAssignToQuickSlot(const UInventoryItem* Item, EQuickSlotType SlotType) const;

	/**
	 * Find a replacement item of the same type when a stack is consumed
	 */
	UInventoryItem* FindReplacementItem(const UInventoryItem* ConsumedItem, TWeakObjectPtr<UInventoryGrid>& OutGrid, int32& OutIndex);
	
	// widget switcher, may not be used since everything will be on same screen
	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<UWidgetSwitcher> Switcher;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Head;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Ears;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Face;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Eye;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Armband;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Shirt;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Pants;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Weapon_Primary;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Weapon_Holster;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Weapon_Secondary;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Tool;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Rig;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Rig_Slots;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Backpack;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Backpack_Slots;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Belt;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Belt_Slots;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryGrid> Grid_Pocket_Slots;
	
	
	// =============================
	// QUICK SLOT PROPERTIES
	// =============================

	// Map of quick slot references
	UPROPERTY()
	TMap<EQuickSlotType, FQuickSlotReference> QuickSlotReferences;
	
	// Auto Populate bindings
	void BindWeaponGridCallbacks();
	
	// Fires when item lands in a weapon/tool grid (auto populate 1-4)
	UFUNCTION()
	void OnWeaponPrimaryItemAdded(UInventoryItem* Item);
	UFUNCTION()
	void OnWeaponHolsterItemAdded(UInventoryItem* Item);
	UFUNCTION()
	void OnWeaponSecondaryItemAdded(UInventoryItem* Item);
	UFUNCTION()
	void OnToolItemAdded(UInventoryItem* Item);

	// Shared auto-populate logic
	void AutoAssignWeaponSlot(UInventoryItem* Item, UInventoryGrid* SourceGrid, EQuickSlotType TargetSlot);
	
	// Consumable validation
	bool IsConsumableQuickSlottable(const UInventoryItem* Item, const UInventoryGrid* SourceGrid) const;
	bool IsConsumableSourceGrid(const UInventoryGrid* SourceGrid) const;
	bool IsConsumableCategory(EItem_Category Category) const;

	// Core assign/clear
	void AssignQuickSlot(const FQuickSlotReference& Reference);
	void ClearQuickSlot(EQuickSlotType SlotType);
	
	// Is the player in quick slot assignment mode?
	UPROPERTY()
	bool bInQuickSlotAssignmentMode = false;
	
	// The quick slot waiting for item assignment
	UPROPERTY()
	EQuickSlotType PendingQuickSlotType = EQuickSlotType::Slot_5;
	
	UPROPERTY(BlueprintAssignable, Category = "QuickSlot")
	FOnQuickSlotUpdated OnQuickSlotUpdated;
	
	// add stash later
	
};
