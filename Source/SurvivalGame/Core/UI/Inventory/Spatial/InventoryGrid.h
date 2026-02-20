// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "SurvivalGame/Core/Data/Items/Manifest/ItemManifest.h"
#include "SurvivalGame/Core/UI/Inventory/Grid/GridSlots.h"
#include "SurvivalGame/Core/UI/Inventory/HoverItem/HoverItem.h"
#include "SurvivalGame/Core/UI/Inventory/ItemPopUp/ItemPopUp.h"
#include "SurvivalGame/Core/UI/Inventory/SlottedItems/SlottedItem.h"
#include "SurvivalGame/Core/UI/Inventory/Types/GridTypes.h"
#include "InventoryGrid.generated.h"

class UInventoryComponent;
class UInventoryItem;
class UItemComponent;
class USlottedItem;
struct FGameplayTag;
enum class EGridSlotState : uint8;


UCLASS()
class SURVIVALGAME_API UInventoryGrid : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	
	
	EItem_Category GetItemCategory() const { return Item_Category; };
	FSlotAvailabilityResult HasRoomForItem(const UItemComponent* ItemComponent);
	
	
	UFUNCTION()
	void AddItem(UInventoryItem* Item);
	void DropItem();
	
	void ShowCursor();
	void HideCursor();
	void SetOwningCanvas(UCanvasPanel* OwningCanvas);
	
	/**
	* Get the storage grid type for specialized storage grids
	*/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	EStorageGridType GetStorageGridType() const { return StorageGridType; }

	/**
	 * Check if this grid accepts a specific item based on placement rules
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool CanAcceptItem(const UInventoryItem* Item) const;

	/**
	 * Get reference to parent spatial inventory
	 */
	void SetSpatialInventory(class USpatialInventory* SpatialInv);
	
	/**
	 * Attempt to add item with priority routing
	 * Returns true if item was successfully placed
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool TryAddItemWithRouting(UInventoryItem* Item);

	/**
	 * Get routing priority for this grid type and item
	 * Lower number = higher priority
	 */
	int32 GetRoutingPriority(const UInventoryItem* Item) const;

private:
	
	TWeakObjectPtr<UInventoryComponent> InventoryComponent;
	TWeakObjectPtr<UCanvasPanel> OwningCanvasPanel;
	
	void ConstructGrid();
	FSlotAvailabilityResult HasRoomForItem(const UInventoryItem* Item);
	FSlotAvailabilityResult HasRoomForItem(const FItemManifest& ItemManifest);
	
	void AddItemToIndices(const FSlotAvailabilityResult& Result, UInventoryItem* NewItem);
	
	bool MatchesPlacementRules(const UInventoryItem* Item) const;
	
	
	
	FVector2D GetDrawSize(const FGridFragment* GridFragment) const;
	void SetSlottedItemImage(const USlottedItem* SlottedItem, const FGridFragment* GridFragment, const FImageFragment* ImageFragment) const;
	void AddItemAtIndex(UInventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount);
	USlottedItem* CreateSlottedItem(
		UInventoryItem* Item, 
		const bool bStackable, 
		const int32 StackAmount, 
		const FGridFragment* GridFragment, 
		const FImageFragment* ImageFragment, 
		const int32 Index
		);
	void AddSlottedItemToCanvas(const int32 Index, const FGridFragment* GridFragment, USlottedItem* SlottedItem) const;
	void UpdateGridSlot(UInventoryItem* NewItem, const int32 Index, bool bStackableItem, const int32 StackAmount);
	bool IsIndexClaimed(const TSet<int32>& CheckedIndices, const int32 Index) const;
	bool HasRoomAtIndex(
		const UGridSlots* GridSlot,
		const FIntPoint& Dimensions,
		const TSet<int32>& CheckedIndices,
		TSet<int32>& OutTentativelyClaimed,
		const FGameplayTag& ItemType,
		const int32 MaxStackSize
		);
	bool CheckSlotConstraints(
		const UGridSlots* GridSlot, 
		const UGridSlots* SubGridSlot, 
		const TSet<int32>& CheckedIndices, 
		TSet<int32>& OutTentativelyClaimed,
		const FGameplayTag& ItemType,
		const int32 MaxStackSize
		) const;
	FIntPoint GetItemDimensions(const FItemManifest& ItemManifest) const;
	bool HasValidItem(const UGridSlots* GridSlot) const;
	bool IsUpperLeftSlot(const UGridSlots* GridSlot, const UGridSlots* SubGridSlot) const;
	bool DoesItemTypeMatch(const UInventoryItem* SubItem, const FGameplayTag& ItemType) const;
	bool IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const;
	int32 DetermineFillAmountForSlot(const bool bStackable, const int32 MaxStackSize, const int32 AmountToFill, const UGridSlots* GridSlot) const;
	int32 GetStackAmount(const UGridSlots* GridSlot) const;
	bool IsRightClick(const FPointerEvent& MouseEvent) const;
	bool IsLeftClick(const FPointerEvent& MouseEvent) const;
	void PickUp(UInventoryItem* ClickedInventoryItem, const int32 GridIndex);
	void AssignHoverItem(UInventoryItem* InventoryItem);
	void AssignHoverItem(UInventoryItem* InventoryItem, const int32 GridIndex, const int32 PreviousGridIndex);
	void RemoveItemFromGrid(UInventoryItem* InventoryItem, const int32 GridIndex);
	void UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition);
	FIntPoint CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;
	ETileQuadrant CalculateTileQuadrant(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;
	void OnTileParametersUpdated(const FTileParameters& Parameters);
	FIntPoint CalculateStartingCoordinate(const FIntPoint& Coordinate, const FIntPoint& Dimensions, const ETileQuadrant Quadrant) const;
	FSpaceQueryResult CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions);
	bool CursorExistedCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize, const FVector2D& Location);
	void HighlightSlots(const int32 Index, const FIntPoint& Dimensions);
	void UnHighlightSlots(const int32 Index, const FIntPoint& Dimensions);
	void ChangeHoverType(const int32 Index, const FIntPoint& Dimensions, EGridSlotState GridSlotState);
	void PutDownOnIndex(const int32 Index);
	void ClearHoverItem();
	UUserWidget* GetVisibleCursorWidget();
	UUserWidget* GetHiddenCursorWidget();
	bool IsSameStackable(const UInventoryItem* ClickedInventoryItem) const;
	void SwapWithHoverItem(UInventoryItem* ClickedInventoryItem, const int32 GridIndex);
	bool ShouldSwapStackCounts(const int32 RoomInClickedSlot, const int32 HoveredStackCount, const int32 MaxStackSize) const;
	void SwapStackCounts(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index);
	bool ShouldConsumeHoverItemStacks(const int32 HoveredItemStackCount, const int32 RoomInClickedSlot) const;
	void ConsumeHoverItemStacks(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index);
	bool ShouldFillInStack(const int32 RoomInClickedSlot, const int32 HoveredStackCount) const;
	void FillInStack(const int32 FillAmount, const int32 Remainder, const int32 Index);
	void CreateItemPopUp(const int32 GridIndex);
	
	/**
	* Reference to parent spatial inventory for quick slot management
	*/
	UPROPERTY()
	TWeakObjectPtr<class USpatialInventory> SpatialInventory;
	
	
	
	// =============================
	// GRID CONFIGURATION PROPERTIES
	// =============================
    
	/**
	 * Grid Restriction Type
	 * 
	 * Determines how this grid filters items:
	 * - Equipment: Only accepts items with matching RequiredEquipmentType
	 * - Storage: Accepts any item with bCanGoInStorage = true
	 * - Specialized: Accepts items in AllowedItemTypes array
	 * 
	 * Configure in Blueprint:
	 * - Equipment grids (Grid_Head): Set to "Equipment"
	 * - Storage grids (Grid_Backpack_Slots): Set to "Storage"
	 */
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	EGridRestrictionType RestrictionType = EGridRestrictionType::Storage;
	
	/**
	* Required Equipment Type
	* 
	* For Equipment grids: Which equipment type this grid accepts.
	* 
	* Example:
	* - Grid_Head: Set to "Head" (only accepts helmets)
	* - Grid_Backpack: Set to "Backpack" (only accepts backpacks)
	* 
	* Only visible/used when RestrictionType = Equipment
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", 
		meta = (AllowPrivateAccess = "true", 
			EditCondition = "RestrictionType == EGridRestrictionType::Equipment", EditConditionHides))
	EItem_Category RequiredEquipmentType = EItem_Category::None;
	
	/**
	 * Allowed Item Types
	 * 
	 * For Specialized grids: Which item categories this grid accepts.
	 * 
	 * Example:
	 * - Medical Pouch: Add "Medical", "Consumable"
	 * - Ammo Pouch: Add "Ammo", "Magazine"
	 * 
	 * Only visible/used when RestrictionType = Specialized
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory",
			  meta = (AllowPrivateAccess = "true",
					  EditCondition = "RestrictionType == EGridRestrictionType::Specialized",
					  EditConditionHides))
	TArray<EItem_Category> AllowedItemTypes;
	
	
	/**
	* Check if item matches equipment restrictions
	 */
	bool MatchesEquipmentRestriction(const UInventoryItem* Item) const;

	/**
	 * Check if item matches storage restrictions
	 */
	bool MatchesStorageRestriction(const UInventoryItem* Item) const;

	/**
	 * Check if item is an equippable type
	 */
	//UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsEquippableItem(EItem_Category Category) const;

	/**
	 * Check if item is a usable type
	 */
	bool IsUsableItem(EItem_Category Category) const;

	/**
	 * Check if item is misc type
	 */
	bool IsMiscItem(EItem_Category Category) const;
	
	// DEPRECATED - Use RestrictionType, RequiredEquipmentType, and StorageGridType instead
	// Will be removed after full refactor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	EItem_Category Item_Category;

	/**
	 * Storage Grid Type
	 * 
	 * For Storage grids: Identifies what type of storage container this is.
	 * Used for specialized storage restrictions (e.g., medical pouches, ammo pouches)
	 * 
	 * Only visible/used when RestrictionType = Storage
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory",
			  meta = (AllowPrivateAccess = "true",
					  EditCondition = "RestrictionType == EGridRestrictionType::Storage",
					  EditConditionHides))
	EStorageGridType StorageGridType = EStorageGridType::None;
	
	
	
	// =============================
	// POPUP PROPERTIES
	// =============================
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UItemPopUp> ItemPopUpClass;
	
	UPROPERTY()
	TObjectPtr<UItemPopUp> ItemPopUp;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	FVector2D ItemPopUpOffset;
	
	
	// =============================
	// CURSOR PROPERTIES
	// =============================
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UUserWidget> VisibleCursorWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UUserWidget> HiddenCursorWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> VisibleCursorWidget;
	UPROPERTY()
	TObjectPtr<UUserWidget> HiddenCursorWidget;
	
	
	// =============================
	// DELEGATES
	// =============================
	
	UFUNCTION()
	void AddStacks(const FSlotAvailabilityResult& Result);
	
	UFUNCTION()
	void OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent);
	
	UFUNCTION()
	void OnGridSlotClicked(int32 GridIndex, const FPointerEvent& MouseEvent);
	
	UFUNCTION()
	void OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent);
	
	UFUNCTION()
	void OnGridSlotUnhovered(int32 GridIndex, const FPointerEvent& MouseEvent);
	
	UFUNCTION()
	void OnPopUpMenuSplit(int32 SplitAmount, int32 Index);
	
	UFUNCTION()
	void OnPopUpMenuDrop(int32 Index);
	
	UFUNCTION()
	void OnPopUpMenuConsume(int32 Index);
	
	UFUNCTION()
	void OnPopUpMenuAssign(int32 Index);

	
	
	
	// =============================
	// GRID SLOT PROPERTIES
	// =============================
	
	
	UPROPERTY()
	TArray<TObjectPtr<UGridSlots>> GridSlots;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGridSlots> GridSlotClass;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<USlottedItem> SlottedItemClass;
	
	UPROPERTY()
	TMap<int32, TObjectPtr<USlottedItem>> SlottedItems;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 Rows;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 Columns;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	float SlotSize;
	
	
	// =============================
	// HOVER ITEM PROPERTIES
	// =============================
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UHoverItem> HoverItemClass;
	
	UPROPERTY()
	TObjectPtr<UHoverItem> HoverItem;
	
	
	// =============================
	// TILE TRACKING PROPERTIES
	// =============================
	
	FTileParameters TileParameters; 
	FTileParameters LastTileParameters;
	
	// Index where an item would be placed if clicked on valid grid location
	int32 ItemDropIndex{INDEX_NONE};
	
	FSpaceQueryResult CurrentQueryResult;
	
	bool bMouseWithinCanvas;
	bool bLastMouseWithinCanvas;
	
	int32 LastHighlightedIndex;
	
	FIntPoint LastHighlightedDimensions;
	
};


















