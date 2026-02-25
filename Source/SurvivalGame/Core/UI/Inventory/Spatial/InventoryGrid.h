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
	
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent.Get(); }
	
	EItem_Category GetItemCategory() const { return Item_Category; };
	FSlotAvailabilityResult HasRoomForItem(const UItemComponent* ItemComponent);
	
	
	UFUNCTION()
	void AddItem(UInventoryItem* Item);
	void DropItem();
	
	void ShowCursor();
	void HideCursor();
	void SetOwningCanvas(UCanvasPanel* OwningCanvas);
	
	
private:
	
	TWeakObjectPtr<UInventoryComponent> InventoryComponent;
	TWeakObjectPtr<UCanvasPanel> OwningCanvasPanel;
	
	void ConstructGrid();
	FSlotAvailabilityResult HasRoomForItem(const UInventoryItem* Item);
	FSlotAvailabilityResult HasRoomForItem(const FItemManifest& ItemManifest);
	void AddItemToIndices(const FSlotAvailabilityResult& Result, UInventoryItem* NewItem);
	bool MatchesCategory(const UInventoryItem* Item) const;
	
	
	
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
	
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	EItem_Category Item_Category;
	
	
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


















