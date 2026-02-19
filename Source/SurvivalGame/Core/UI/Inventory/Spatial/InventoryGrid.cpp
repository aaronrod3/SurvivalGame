// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryGrid.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "SurvivalGame/Character/Inventory/InventoryComponent.h"
#include "SurvivalGame/Core/Data/Items/InventoryItem.h"
#include "SurvivalGame/Core/Data/Items/Fragments/ItemFragment.h"
#include "SurvivalGame/Core/Data/Items/Fragments/FragmentTags.h"
#include "SurvivalGame/Core/Data/Items/Manifest/ItemManifest.h"
#include "SurvivalGame/Core/UI/Inventory/Grid/GridSlots.h"
#include "SurvivalGame/Core/UI/Inventory/SlottedItems/SlottedItem.h"
#include "SurvivalGame/Core/UI/Utilities/InventoryStatics.h"
#include "SurvivalGame/Core/UI/Utilities/WidgetUtilities.h"
#include "SurvivalGame/Core/UI/Inventory/ItemPopUp/ItemPopUp.h"
#include "SpatialInventory.h"


void UInventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ConstructGrid();
	ShowCursor();
	
	InventoryComponent = UInventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &ThisClass::AddItem);
	InventoryComponent->OnStackChange.AddDynamic(this, &ThisClass::AddStacks);
}

void UInventoryGrid::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
	
	const FVector2D CanvasPosition = UWidgetUtilities::GetWidgetPosition(CanvasPanel);
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	
	if (CursorExistedCanvas(CanvasPosition, UWidgetUtilities::GetWidgetSize(CanvasPanel), MousePosition))
	{
		return;
	}
	
	UpdateTileParameters(CanvasPosition, MousePosition);
}

void UInventoryGrid::UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition)
{
	// if mouse not in canvas panel, return
	if (!bMouseWithinCanvas) return;
	
	// calculate the tile quadrant, tile index, coordinates
	const FIntPoint HoveredCoordinates = CalculateHoveredCoordinates(CanvasPosition, MousePosition);
	
	LastTileParameters = TileParameters;
	TileParameters.TileCoordinates = HoveredCoordinates;
	TileParameters.TileIndex = UWidgetUtilities::GetIndexFromPosition(HoveredCoordinates, Columns);
	TileParameters.TileQuadrant = CalculateTileQuadrant(CanvasPosition, MousePosition);
	
	// handle the highlight/unhighlight of grid slots
	OnTileParametersUpdated(TileParameters);
}

void UInventoryGrid::OnTileParametersUpdated(const FTileParameters& Parameters)
{
	if (!IsValid(HoverItem)) return;
	
	// Get hover item dimensions
	const FIntPoint Dimensions = HoverItem->GetGridDimensions();
	
	// calculate the starting coordinate for highlighting
	const FIntPoint StartingCoordinate = CalculateStartingCoordinate(Parameters.TileCoordinates, Dimensions, Parameters.TileQuadrant);
	ItemDropIndex = UWidgetUtilities::GetIndexFromPosition(StartingCoordinate, Columns);
	
	CurrentQueryResult = CheckHoverPosition(StartingCoordinate, Dimensions);
	
	if (CurrentQueryResult.bHasSpace)
	{
		HighlightSlots(ItemDropIndex, Dimensions);
		return;
	}
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	
	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		// if theres a single stack, add or swap
		const FGridFragment* GridFragment = GetFragment<FGridFragment>(CurrentQueryResult.ValidItem.Get(), FragmentTags::GridFragment);
		if (!GridFragment) return;
		
		ChangeHoverType(CurrentQueryResult.UpperLeftIndex, GridFragment->GetGridSize(), EGridSlotState::GrayedOut);
	}
}

FIntPoint UInventoryGrid::CalculateStartingCoordinate(const FIntPoint& Coordinate, const FIntPoint& Dimensions, const ETileQuadrant Quadrant) const
{
	const int32 HasEvenWidth = Dimensions.X % 2 == 0 ? 1 : 0;
	const int32 HasEvenHeight = Dimensions.Y % 2 == 0 ? 1 : 0;
	
	FIntPoint StartingCoordinate;
	switch (Quadrant)
	{
	case ETileQuadrant::TopLeft:
			StartingCoordinate.X = Coordinate.X - FMath::FloorToInt(.5f * Dimensions.X);
			StartingCoordinate.Y = Coordinate.Y - FMath::FloorToInt(.5f * Dimensions.Y);
		break;
	case ETileQuadrant::TopRight:
			StartingCoordinate.X = Coordinate.X - FMath::FloorToInt(.5f * Dimensions.X) + HasEvenWidth;
			StartingCoordinate.Y = Coordinate.Y - FMath::FloorToInt(.5f * Dimensions.Y);
		break;
	case ETileQuadrant::BottomLeft:
			StartingCoordinate.X = Coordinate.X - FMath::FloorToInt(.5f * Dimensions.X);
			StartingCoordinate.Y = Coordinate.Y - FMath::FloorToInt(.5f * Dimensions.Y) + HasEvenHeight;
		break;
	case ETileQuadrant::BottomRight:
			StartingCoordinate.X = Coordinate.X - FMath::FloorToInt(.5f * Dimensions.X) + HasEvenWidth;
			StartingCoordinate.Y = Coordinate.Y - FMath::FloorToInt(.5f * Dimensions.Y) + HasEvenHeight;
		break;
	default: 
		UE_LOG(LogTemp, Error, TEXT("Invalid tile quadrant"));
		return FIntPoint(-1, -1);
	}
	return StartingCoordinate;
	
}

FIntPoint UInventoryGrid::CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const
{
	// calculate the tile quadrant, tile index, coordinates
	return FIntPoint{
		static_cast<int32>(FMath::FloorToInt((MousePosition.X - CanvasPosition.X) / SlotSize)),
		static_cast<int32>(FMath::FloorToInt((MousePosition.Y - CanvasPosition.Y) / SlotSize))
	};
}

ETileQuadrant UInventoryGrid::CalculateTileQuadrant(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const
{
	// Calculate relative position withing the current tile
	const float TileLocalX = FMath::Fmod(MousePosition.X - CanvasPosition.X, SlotSize);
	const float TileLocalY = FMath::Fmod(MousePosition.Y - CanvasPosition.Y, SlotSize);
	
	// determine the quadrant the mouse is in
	const bool bIsTop = TileLocalY < SlotSize / 2.f;
	const bool bIsLeft = TileLocalX < SlotSize / 2.f;

	ETileQuadrant HoveredTileQuadrant{ETileQuadrant::None};
	if (bIsTop && bIsLeft) HoveredTileQuadrant = ETileQuadrant::TopLeft;
	else if (bIsTop && !bIsLeft) HoveredTileQuadrant = ETileQuadrant::TopRight;
	else if (!bIsTop && bIsLeft) HoveredTileQuadrant = ETileQuadrant::BottomLeft;
	else if (!bIsTop && !bIsLeft) HoveredTileQuadrant = ETileQuadrant::BottomRight;
	
	return HoveredTileQuadrant;
}

FSpaceQueryResult UInventoryGrid::CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions)
{
	FSpaceQueryResult Result;
	
	// is it in bounds
	if (!IsInGridBounds(UWidgetUtilities::GetIndexFromPosition(Position, Columns), Dimensions)) return Result;
	
	Result.bHasSpace = true;
	
	// check if indices have the same upper left index
	TSet<int32> OccupiedUpperLeftIndices;
	UInventoryStatics::ForEach2D(GridSlots, UWidgetUtilities::GetIndexFromPosition(Position, Columns), Dimensions, Columns, [&](const UGridSlots* GridSlot)
	{
		if (GridSlot->GetInventoryItem().IsValid())
		{
			OccupiedUpperLeftIndices.Add(GridSlot->GetUpperLeftIndex());
			Result.bHasSpace = false;
		}
	});
	
	
	// if so, is there only one item in the way, swap?
	if (OccupiedUpperLeftIndices.Num() == 1) // single item at pos, valid for swapping/stacking
	{
		const int32 Index = *OccupiedUpperLeftIndices.CreateConstIterator();
		Result.ValidItem = GridSlots[Index]->GetInventoryItem().Get();
		Result.UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	}
	
	return Result;
}

bool UInventoryGrid::CursorExistedCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize, const FVector2D& Location)
{
	bLastMouseWithinCanvas = bMouseWithinCanvas;
	bMouseWithinCanvas = UWidgetUtilities::IsWithinBounds(BoundaryPos, BoundarySize, Location);
	if (!bMouseWithinCanvas && bLastMouseWithinCanvas)
	{
		UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
		return true;
	}
	return false;
}

void UInventoryGrid::HighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	if (!bMouseWithinCanvas) return;
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	UInventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UGridSlots* GridSlot)
	{
		GridSlot->SetOccupiedTexture();
	});
	LastHighlightedDimensions = Dimensions;
	LastHighlightedIndex = Index;
	
}

void UInventoryGrid::UnHighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	UInventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UGridSlots* GridSlot)
	{
		if (GridSlot->IsAvailable())
		{
			GridSlot->SetUnoccupiedTexture();
		}
		else
		{
			GridSlot->SetOccupiedTexture();
		}
	});
}

void UInventoryGrid::ChangeHoverType(const int32 Index, const FIntPoint& Dimensions, EGridSlotState GridSlotState)
{
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	
	UInventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [State = GridSlotState](UGridSlots* GridSlot)
	{
		switch (State)
		{
		case EGridSlotState::Occupied:
			GridSlot->SetOccupiedTexture();
			break;
		case EGridSlotState::Unoccupied:
			GridSlot->SetUnoccupiedTexture();
			break;
		case EGridSlotState::GrayedOut:
			GridSlot->SetGrayedOutTexture();
			break;
		case EGridSlotState::Selected:
			GridSlot->SetSelectedTexture();
			break;
		}
	});
	
	LastHighlightedDimensions = Dimensions;
	LastHighlightedIndex = Index;
}



// Start Inventory Grid functions

void UInventoryGrid::ConstructGrid()
{
	GridSlots.Reserve(Rows * Columns);
	
	for (int32 j = 0; j < Rows; j++)
	{
		for (int32 i = 0; i < Columns; i++)
		{
			UGridSlots* GridSlot = CreateWidget<UGridSlots>(this, GridSlotClass);
			CanvasPanel->AddChild(GridSlot);
			
			const FIntPoint TilePosition(i, j);
			GridSlot->SetTileIndex(UWidgetUtilities::GetIndexFromPosition(FIntPoint(i, j), Columns));
			
			UCanvasPanelSlot* GridCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(GridSlot);
			
			GridCPS->SetSize(FVector2D(SlotSize));
			GridCPS->SetPosition(TilePosition * SlotSize);
			
			GridSlots.Add(GridSlot);	
			GridSlot->GridSlotClicked.AddDynamic(this, &ThisClass::OnGridSlotClicked);
			GridSlot->GridSlotHovered.AddDynamic(this, &ThisClass::OnGridSlotHovered);
			GridSlot->GridSlotUnhovered.AddDynamic(this, &ThisClass::OnGridSlotUnhovered);
			
		}
	}
	
}

bool UInventoryGrid::MatchesPlacementRules(const UInventoryItem* Item) const
{
	if (!Item) return false;
	
	const FItemManifest& Manifest = Item->GetItemManifest();
	const FItemPlacementRules& Rules = Manifest.GetPlacementRules();
	const EItem_Category ItemCategory = Rules.EquipmentSlot;
	
	// Route based on grid restriction type
	switch (RestrictionType)
	{
	case EGridRestrictionType::Equipment:
		return MatchesEquipmentRestriction(Item);
	case EGridRestrictionType::Storage:
		return MatchesStorageRestriction(Item);
	case EGridRestrictionType::QuickSlot:
		// Quick slots dont accept direct placement, only references
		return false;
	case EGridRestrictionType::None:
	default:
		// No restrictions - accept anything
		return true;
	}
}


FSlotAvailabilityResult UInventoryGrid::HasRoomForItem(const UItemComponent* ItemComponent)
{
	return HasRoomForItem(ItemComponent->GetItemManifest());
}

FSlotAvailabilityResult UInventoryGrid::HasRoomForItem(const UInventoryItem* Item)
{
	return HasRoomForItem(Item->GetItemManifest());	
}

FSlotAvailabilityResult UInventoryGrid::HasRoomForItem(const FItemManifest& ItemManifest)
{
	FSlotAvailabilityResult Result;
	
	// Determine if the item is stackable
	const FStackableFragment* StackableFragment = ItemManifest.GetFragmentOfType<FStackableFragment>();
	Result.bStackable = StackableFragment != nullptr;
	
	// Detemine how many stacks to add
	const int32 MaxStackSize = StackableFragment ? StackableFragment->GetMaxStackSize() : 1;
	int32 AmountToFill = StackableFragment ? StackableFragment->GetStackCount() : 1;
	
	TSet<int32> CheckedIndices;
	// For each grid slot
	for (const auto& GridSlot : GridSlots)
	{
		// if we dont have any more to fill, break out of the loop early
		if (AmountToFill == 0 ) break;
		
		// is this index claimed yet
		if (IsIndexClaimed(CheckedIndices, GridSlot->GetIndex())) continue;
		
		// is the item in grid bounds
		if (!IsInGridBounds(GridSlot->GetIndex(), GetItemDimensions(ItemManifest))) continue;
		
		// can item fit here(is it out of grid bounds
		TSet<int32> TentativelyClaimed;
		if (!HasRoomAtIndex(GridSlot, GetItemDimensions(ItemManifest), CheckedIndices, TentativelyClaimed, ItemManifest.GetItemType(), MaxStackSize))
		{
			continue;
		}
		
		// how much to fill
		const int32 AmountToFillInSlot = DetermineFillAmountForSlot(Result.bStackable, MaxStackSize, AmountToFill, GridSlot);
		if (AmountToFillInSlot == 0) continue;
		
		CheckedIndices.Append(TentativelyClaimed);
		
		// update the amount left to fill
		Result.TotalRoomToFill += AmountToFillInSlot;
		Result.SlotAvailabilities.Emplace(
			FSlotAvailability(
				HasValidItem(GridSlot) ? GridSlot->GetUpperLeftIndex() : GridSlot->GetIndex(),
				Result.bStackable ? AmountToFillInSlot : 0,
				HasValidItem(GridSlot)
				)	
		);
		
		AmountToFill -= AmountToFillInSlot;
		
		// how much is the remainder
		Result.Remainder = AmountToFill;
		
		if (AmountToFill == 0) return Result;
	}
	
	return Result;
}




void UInventoryGrid::AddItem(UInventoryItem* Item)
{
	if (!MatchesPlacementRules(Item)) return;
	
	FSlotAvailabilityResult Result = HasRoomForItem(Item);
	AddItemToIndices(Result, Item);
	
}

void UInventoryGrid::AddItemToIndices(const FSlotAvailabilityResult& Result, UInventoryItem* NewItem)
{
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		AddItemAtIndex(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
		UpdateGridSlot(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
	}
	
	
}


void UInventoryGrid::AddItemAtIndex(UInventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount)
{
	// hard safety, never allow two widgets to exist at the index
	if (TObjectPtr<USlottedItem>* ExistingItem = SlottedItems.Find(Index) )
	{
		if (IsValid(ExistingItem->Get()))
		{
			ExistingItem->Get()->RemoveFromParent();
		}
		SlottedItems.Remove(Index);
	}
	
	
	
	// Get grid fragments
	const FGridFragment* GridFragment = GetFragment<FGridFragment>(Item, FragmentTags::GridFragment);
	// get image fragment
	const FImageFragment* ImageFragment = GetFragment<FImageFragment>(Item, FragmentTags::IconFragment);
	if (!GridFragment || !ImageFragment) return;
	
	USlottedItem* SlottedItem = CreateSlottedItem(Item, bStackable, StackAmount, GridFragment, ImageFragment, Index);
	
	// Add item to canvas panel
	AddSlottedItemToCanvas(Index, GridFragment, SlottedItem);
	
	// store new widget in a container
	SlottedItems.Add(Index, SlottedItem);
}

bool UInventoryGrid::TryAddItemWithRouting(UInventoryItem* Item)
{
	if (!Item || !CanAcceptItem(Item))
	{
		return false;
	}
	
	// Check if we have room
	FSlotAvailabilityResult Result = HasRoomForItem(Item);
	if (Result.TotalRoomToFill <= 0)
	{
		return false;
	}
	
	// Add the item
	AddItemToIndices(Result, Item);
	return true;
}

int32 UInventoryGrid::GetRoutingPriority(const UInventoryItem* Item) const
{
	if (!Item)
	{
		return INT_MAX;
	}
	
	const FItemManifest& Manifest = Item->GetItemManifest();
	const FItemPlacementRules& Rules = Manifest.GetPlacementRules();
	const EItem_Category ItemCategory = Rules.EquipmentSlot;
	
	// Priority routing:
	// 1. Equipment items go to empty equipment slots first
	// 2. Then to storage
	// 3. Usable/Misc items go straight to storage (skip equipment slots)
	
	if (RestrictionType == EGridRestrictionType::Equipment)
	{
		// Equipment grid
		if (IsEquippableItem(ItemCategory))
		{
			// Does this item match this equipment slot?
			if (ItemCategory == RequiredEquipmentType ||
				(RequiredEquipmentType == EItem_Category::Weapon_Secondary && 
				 ItemCategory == EItem_Category::Weapon_Primary))
			{
				// Priority 1: Matching equipment slot
				return 1;
			}
		}
		
		// Not matching equipment - very low priority
		return 100;
	}
	else if (RestrictionType == EGridRestrictionType::Storage)
	{
		// Storage grid
		if (IsEquippableItem(ItemCategory))
		{
			// Priority 2: Storage for equipment items (fallback)
			return 2;
		}
		else
		{
			// Priority 1: Storage for non-equipment items (primary destination)
			return 1;
		}
	}
	
	// Default: low priority
	return 50;
}


USlottedItem* UInventoryGrid::CreateSlottedItem(UInventoryItem* Item, 
												const bool bStackable, 
												const int32 StackAmount, 
												const FGridFragment* GridFragment, 
												const FImageFragment* ImageFragment, 
												const int32 Index)
{
	USlottedItem* SlottedItem = CreateWidget<USlottedItem>(GetOwningPlayer(), SlottedItemClass); 
	SlottedItem->SetInventoryItem(Item);
	SetSlottedItemImage(SlottedItem, GridFragment, ImageFragment);
	SlottedItem->SetGridIndex(Index);
	SlottedItem->SetIsStackable(bStackable);
	const int32 StackUpdateAmount = bStackable ? StackAmount : 0;
	SlottedItem->UpdateStackCount(StackUpdateAmount);
	SlottedItem->OnSlottedItemClicked.AddDynamic(this, &ThisClass::OnSlottedItemClicked);
	
	return SlottedItem;
}

void UInventoryGrid::AddSlottedItemToCanvas(const int32 Index, const FGridFragment* GridFragment, USlottedItem* SlottedItem) const
{
	CanvasPanel->AddChild(SlottedItem);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(SlottedItem);
	CanvasSlot->SetSize(GetDrawSize(GridFragment));
	const FVector2D DrawPos = UWidgetUtilities::GetPositionFromIndex(Index, Columns) * SlotSize;
	const FVector2D DrawPosWithPadding = DrawPos + FVector2D(GridFragment->GetGridPadding());
	CanvasSlot->SetPosition(DrawPosWithPadding);
}

void UInventoryGrid::UpdateGridSlot(UInventoryItem* NewItem, const int32 Index, bool bStackableItem, const int32 StackAmount)
{
	check(GridSlots.IsValidIndex(Index));
	
	if (bStackableItem)
	{
		GridSlots[Index]->SetStackCount(StackAmount);
	}
	
	const FGridFragment* GridFragment = GetFragment<FGridFragment>(NewItem, FragmentTags::GridFragment);
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);
	
	UInventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UGridSlots* GridSlot)
	{
		GridSlot->SetInventoryItem(NewItem);
		GridSlot->SetUpperLeftIndex(Index);
		GridSlot->SetOccupiedTexture();
		GridSlot->SetAvailable(false);
	});
}

bool UInventoryGrid::IsIndexClaimed(const TSet<int32>& CheckedIndices, const int32 Index) const
{
	return CheckedIndices.Contains(Index);
}

bool UInventoryGrid::HasRoomAtIndex(const UGridSlots* GridSlot,
									const FIntPoint& Dimensions,
									const TSet<int32>& CheckedIndices,
									TSet<int32>& OutTentativelyClaimed,
									const FGameplayTag& ItemType,
									const int32 MaxStackSize)
{
	// is there room at this index (are other items in the way)
	bool bHasRoomAtIndex = true;
	
	UInventoryStatics::ForEach2D(GridSlots, GridSlot->GetIndex(), Dimensions, Columns, [&](const UGridSlots* SubGridSlot)
	{
		if (CheckSlotConstraints(GridSlot, SubGridSlot, CheckedIndices, OutTentativelyClaimed, ItemType, MaxStackSize))
		{
			OutTentativelyClaimed.Add(SubGridSlot->GetIndex());
		}
		else
		{
			bHasRoomAtIndex = false;
		}
	});
	return bHasRoomAtIndex;
}

bool UInventoryGrid::CheckSlotConstraints(const UGridSlots* GridSlot, 
											const UGridSlots* SubGridSlot, 
											const TSet<int32>& CheckedIndices, 
											TSet<int32>& OutTentativelyClaimed,
											const FGameplayTag& ItemType,
											const int32 MaxStackSize) const
	{
	
	// index claimed
	if (IsIndexClaimed(CheckedIndices, SubGridSlot->GetIndex())) return false;
	
	// has valid item 
	if (!HasValidItem(SubGridSlot))
	{
		OutTentativelyClaimed.Add(SubGridSlot->GetIndex());
		return true;
	}
	
	// is upper left slot
	if (!IsUpperLeftSlot(GridSlot, SubGridSlot)) return false;
	
	// if so is it stackable
	const UInventoryItem* SubItem = SubGridSlot->GetInventoryItem().Get();
	if (!SubItem->IsStackable()) return false;
	
	// is this the same type of item were trying to add
	if (!DoesItemTypeMatch(SubItem, ItemType)) return false;
	
	// if stackable, is it at max stack capacity already
	if (GridSlot->GetStackCount() >= MaxStackSize) return false;
	
	
	return true;
}

FIntPoint UInventoryGrid::GetItemDimensions(const FItemManifest& ItemManifest) const
{
	const FGridFragment* GridFragment = ItemManifest.GetFragmentOfType<FGridFragment>();
	return GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);
}

bool UInventoryGrid::HasValidItem(const UGridSlots* GridSlot) const
{
	return GridSlot->GetInventoryItem().IsValid();
}

bool UInventoryGrid::IsUpperLeftSlot(const UGridSlots* GridSlot, const UGridSlots* SubGridSlot) const
{
	return SubGridSlot->GetUpperLeftIndex() == GridSlot->GetIndex();
}

bool UInventoryGrid::DoesItemTypeMatch(const UInventoryItem* SubItem, const FGameplayTag& ItemType) const
{
	return SubItem->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
}

bool UInventoryGrid::IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const
{
	if (StartIndex < 0 || StartIndex >= GridSlots.Num()) return false;
	const int32 EndColumn = (StartIndex % Columns) + ItemDimensions.X;
	const int32 EndRow = (StartIndex / Columns) + ItemDimensions.Y;
	return EndColumn <= Columns && EndRow <= Rows;
}

int32 UInventoryGrid::DetermineFillAmountForSlot(const bool bStackable, 
												const int32 MaxStackSize,
												const int32 AmountToFill, 
												const UGridSlots* GridSlot) const
{
	// calculate room in the slot
	const int32 RoomInSlot = MaxStackSize - GetStackAmount(GridSlot);
	
	// if stackable, need the minimum between AmountToFill and RoomInSlot
	return bStackable ? FMath::Min(AmountToFill, RoomInSlot) : 1;
	
}

int32 UInventoryGrid::GetStackAmount(const UGridSlots* GridSlot) const
{
	int32 CurrentSlotStackAmount = GridSlot->GetStackCount();
	// if at a slot that doesnt hold the stack count, get actual stack count
	if (const int32 UpperLeftIndex = GridSlot->GetUpperLeftIndex(); UpperLeftIndex != INDEX_NONE)
	{
		UGridSlots* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
		CurrentSlotStackAmount = UpperLeftGridSlot->GetStackCount();
	}
	return CurrentSlotStackAmount;
}

bool UInventoryGrid::IsRightClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
}

bool UInventoryGrid::IsLeftClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
}

void UInventoryGrid::PickUp(UInventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	//Assign hover item
	AssignHoverItem(ClickedInventoryItem, GridIndex, GridIndex);
	
	// remove clicked item from grid
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
}

void UInventoryGrid::DropItem()
{
	if (!IsValid(HoverItem)) return;
	if (!IsValid(HoverItem->GetInventoryItem())) return;
	
	InventoryComponent->Server_DropItem(HoverItem->GetInventoryItem(), HoverItem->GetStackCount());
	
	ClearHoverItem();
	ShowCursor();
}

void UInventoryGrid::AssignHoverItem(UInventoryItem* InventoryItem, const int32 GridIndex,
                                     const int32 PreviousGridIndex)
{
	AssignHoverItem(InventoryItem);
	HoverItem->SetPreviousGridIndex(PreviousGridIndex);
	HoverItem->UpdateStackCount(InventoryItem->IsStackable() ? GridSlots[GridIndex]->GetStackCount() : 0);
}

void UInventoryGrid::RemoveItemFromGrid(UInventoryItem* InventoryItem, const int32 GridIndex)
{
	const FGridFragment* GridFragment = GetFragment<FGridFragment>(InventoryItem, FragmentTags::GridFragment);
	if (!GridFragment) return;
	
	UInventoryStatics::ForEach2D(GridSlots, GridIndex, GridFragment->GetGridSize(), Columns, [&](UGridSlots* GridSlot)
	{
		GridSlot->SetInventoryItem(nullptr);
		GridSlot->SetUpperLeftIndex(INDEX_NONE);
		GridSlot->SetUnoccupiedTexture();
		GridSlot->SetAvailable(true);
		GridSlot->SetStackCount(0);
	});
	
	if (SlottedItems.Contains(GridIndex))
	{
		TObjectPtr<USlottedItem> FoundSlottedItem;
		SlottedItems.RemoveAndCopyValue(GridIndex, FoundSlottedItem);
		FoundSlottedItem->RemoveFromParent();
	}
	
	// Notify spatial inventory that item was removed (for quick slot updates)
	if (SpatialInventory.IsValid())
	{
		SpatialInventory->OnItemRemovedFromGrid(this, GridIndex);
	}
}



void UInventoryGrid::AssignHoverItem(UInventoryItem* InventoryItem)
{
	if (!IsValid(HoverItem))
	{
		HoverItem = CreateWidget<UHoverItem>(GetOwningPlayer(), HoverItemClass);
	}
	
	// Get grid fragments
	const FGridFragment* GridFragment = GetFragment<FGridFragment>(InventoryItem, FragmentTags::GridFragment);
	// get image fragment
	const FImageFragment* ImageFragment = GetFragment<FImageFragment>(InventoryItem, FragmentTags::IconFragment);
	if (!GridFragment || !ImageFragment) return;
	
	const FVector2D DrawSize = GetDrawSize(GridFragment);
	
	FSlateBrush IconBrush;
	IconBrush.SetResourceObject(ImageFragment->GetIcon());
	IconBrush.DrawAs = ESlateBrushDrawType::Image; 
	IconBrush.ImageSize = DrawSize * UWidgetLayoutLibrary::GetViewportScale(this);
	
	HoverItem->SetImageBrush(IconBrush);
	HoverItem->SetGridDimensions(GridFragment->GetGridSize());
	HoverItem->SetInventoryItem(InventoryItem);
	HoverItem->SetIsStackable(InventoryItem->IsStackable());
	
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, HoverItem);
}



void UInventoryGrid::AddStacks(const FSlotAvailabilityResult& Result)
{
	if (!MatchesPlacementRules(Result.Item.Get())) return;
	
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		// guard against indices computed by a different grid
		if (!GridSlots.IsValidIndex(Availability.Index)) continue;
		
		if (Availability.bItemAtIndex)
		{
			const auto& GridSlot = GridSlots[Availability.Index];
			const auto& SlottedItem = SlottedItems.FindChecked(Availability.Index);
			SlottedItem->UpdateStackCount(GridSlot->GetStackCount() + Availability.AmountToFill);
			GridSlot->SetStackCount(GridSlot->GetStackCount() + Availability.AmountToFill);
		}
		else
		{
			AddItemAtIndex(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
			UpdateGridSlot(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
		}
	}
}

void UInventoryGrid::OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	// check if in quick slot assignment mode
	if (SpatialInventory.IsValid() && SpatialInventory->IsInAssignmentMode())
	{
		// Get item at this index
		if (UGridSlots* GridSlot = GridSlots[GridIndex])
		{
			if (UInventoryItem* Item = GridSlot->GetInventoryItem().Get())
			{
				EQuickSlotType TargetSlot = SpatialInventory->GetPendingAssignmentSlot();
				
				// Attempt to assign
				if (SpatialInventory->AssignItemToQuickSlot(Item, this, GridIndex, TargetSlot))
				{
					// Assignment succcessful
					return;
				}
			}
		}
		
		// Assignment failed or no item, cancel assignment mode
		SpatialInventory->CancelQuickSlotAssignment();
		return;
	}
	
	
	check(GridSlots.IsValidIndex(GridIndex));
	UInventoryItem* ClickedInventoryItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	
	if (!IsValid(ClickedInventoryItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("Clicked inventory item is null"));
		return;
	}
	
	if (!IsValid(HoverItem) && IsLeftClick(MouseEvent))
	{
		PickUp(ClickedInventoryItem, GridIndex);
		return;
	}
	
	if (IsRightClick(MouseEvent))
	{
		CreateItemPopUp(GridIndex);
		return;
	}
	
	
	// is hovered item and clicked inv item the same type/stackable
	if (IsSameStackable(ClickedInventoryItem))
	{
		const int32 ClickedStackCount = GridSlots[GridIndex]->GetStackCount();
		const FStackableFragment* StackableFragment = GetFragment<FStackableFragment>(ClickedInventoryItem, FragmentTags::StackableFragment);
		const int32 MaxStackSize = StackableFragment->GetMaxStackSize();
		const int32 RoomInClickedSlot = MaxStackSize - ClickedStackCount;
		const int32 HoveredStackCount = HoverItem->GetStackCount();
		
		// swap stack counts
		if (ShouldSwapStackCounts(RoomInClickedSlot, HoveredStackCount, MaxStackSize))
		{
			SwapStackCounts(ClickedStackCount, HoveredStackCount, GridIndex);
			return;
		}
		
		// consume hover items stack
		if (ShouldConsumeHoverItemStacks(HoveredStackCount, RoomInClickedSlot))
		{
			ConsumeHoverItemStacks(ClickedStackCount, HoveredStackCount, GridIndex);
			return;
		}
		
		// fill in stack of clicked item, not consume
		if (ShouldFillInStack(RoomInClickedSlot, HoveredStackCount))
		{
			FillInStack(RoomInClickedSlot, HoveredStackCount - RoomInClickedSlot, GridIndex);
			return;
		}
		
		// no room for clicked slot?
		if (RoomInClickedSlot == 0)
		{
			return;
		}
		
		
	}
		
	// swap with hover item if not same
	SwapWithHoverItem(ClickedInventoryItem, GridIndex);
	
}

void UInventoryGrid::CreateItemPopUp(const int32 GridIndex)
{
	UInventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;
	if (IsValid(GridSlots[GridIndex]->GetItemPopUp())) return;
	
	ItemPopUp = CreateWidget<UItemPopUp>(this, ItemPopUpClass);
	GridSlots[GridIndex]->SetItemPopUp(ItemPopUp);
	
	OwningCanvasPanel->AddChild(ItemPopUp);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(ItemPopUp);
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	CanvasSlot->SetPosition(MousePosition - ItemPopUpOffset);
	CanvasSlot->SetSize(ItemPopUp->GetBoxSize());
	
	const int32 SliderMax = GridSlots[GridIndex]->GetStackCount() - 1;
	if (RightClickedItem->IsStackable() && SliderMax > 0)
	{
		ItemPopUp->OnSplit.BindDynamic(this, &ThisClass::OnPopUpMenuSplit);
		ItemPopUp->SetSliderParams(SliderMax, FMath::Max(1, GridSlots[GridIndex]->GetStackCount() / 2));
	}
	else
	{
		ItemPopUp->CollapseSplitButton();
	}
	
	
	ItemPopUp->OnDrop.BindDynamic(this, &ThisClass::OnPopUpMenuDrop);
	
	if (RightClickedItem->IsUsable())
	{
		ItemPopUp->OnConsume.BindDynamic(this, &ThisClass::OnPopUpMenuConsume);
	}
	else
	{
		ItemPopUp->CollapseConsumeButton();
	}
	
	// Show/hide assign button based on item type
	if (const UInventoryItem* Item = GridSlots[GridIndex]->GetInventoryItem().Get())
	{
		const FItemManifest& Manifest = Item->GetItemManifest();
		const FItemPlacementRules& Rules = Manifest.GetPlacementRules();
		
		// Show assign button only for items that can be quick slotted
		if (Rules.bCanGoInQuickSlot || IsEquippableItem(Rules.EquipmentSlot))
		{
			ItemPopUp->ShowAssignButton();
		}
		else
		{
			ItemPopUp->CollapseAssignButton();
		}
	}
	else
	{
		ItemPopUp->CollapseAssignButton();
	}
	
	ItemPopUp->OnAssign.BindUFunction(this, FName("OnPopUpMenuAssign"));

}

void UInventoryGrid::OnGridSlotClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (!IsValid(HoverItem)) return;
	if (!GridSlots.IsValidIndex(GridIndex)) return;
	
	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		OnSlottedItemClicked(CurrentQueryResult.UpperLeftIndex, MouseEvent);
		return;
	}
	
	auto GridSlot = GridSlots[ItemDropIndex];
	if (!GridSlot->GetInventoryItem().IsValid())
	{
		PutDownOnIndex(ItemDropIndex);
	}
}

void UInventoryGrid::PutDownOnIndex(const int32 Index)
{
	AddItemAtIndex(HoverItem->GetInventoryItem(), Index, HoverItem->IsStackable(), HoverItem->GetStackCount());
	UpdateGridSlot(HoverItem->GetInventoryItem(), Index, HoverItem->IsStackable(), HoverItem->GetStackCount());
	ClearHoverItem();
}

void UInventoryGrid::ClearHoverItem()
{
	if (!IsValid(HoverItem)) return;
	
	HoverItem->SetInventoryItem(nullptr);
	HoverItem->SetIsStackable(false);
	HoverItem->SetPreviousGridIndex(INDEX_NONE);
	HoverItem->UpdateStackCount(0);
	HoverItem->SetImageBrush(FSlateNoResource());
	
	HoverItem->RemoveFromParent();
	HoverItem = nullptr;
	
	// Show cursor
	ShowCursor();
}

bool UInventoryGrid::IsSameStackable(const UInventoryItem* ClickedInventoryItem) const
{
	const bool bIsSameItem = ClickedInventoryItem == HoverItem->GetInventoryItem();
	const bool bIsStackable = ClickedInventoryItem->IsStackable();
	return bIsSameItem && bIsStackable && HoverItem->GetItemType().MatchesTagExact(ClickedInventoryItem->GetItemManifest().GetItemType());
}

void UInventoryGrid::SwapWithHoverItem(UInventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	if (!IsValid(HoverItem)) return;
	
	// Get dimensions of the hover item trying to place
	const FGridFragment* HoverGridFragment = GetFragment<FGridFragment>(HoverItem->GetInventoryItem(), FragmentTags::GridFragment);
	if (!HoverGridFragment) return;
	
	const FIntPoint HoverDimensions = HoverGridFragment->GetGridSize();
	
	// Validate that the hover item will fit at ItemDropIndex
	// need to check if placing the hover item will collide with OTHER items
	// (not including the one thats swapping out)
	
	// save original count before removing
	const int32 OriginalStackCount = ClickedInventoryItem->IsStackable() ? GridSlots[GridIndex]->GetStackCount() : 0;
	
	// first remove the clicked item
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
	
	// check if theres space for the hover item
	const FIntPoint DropCoordinate = UWidgetUtilities::GetPositionFromIndex(ItemDropIndex, Columns);
	FSpaceQueryResult QueryResult = CheckHoverPosition(DropCoordinate, HoverDimensions);
	
	if (!QueryResult.bHasSpace)
	{
		// not enough space
		AddItemAtIndex(ClickedInventoryItem, GridIndex, ClickedInventoryItem->IsStackable(), OriginalStackCount);
		UpdateGridSlot(ClickedInventoryItem, GridIndex, ClickedInventoryItem->IsStackable(), OriginalStackCount);
		return;
	}
	
	UInventoryItem* TempInventoryItem = HoverItem->GetInventoryItem();
	const int32 TempStackCount = HoverItem->GetStackCount();
	const bool bTempIsStackable = HoverItem->IsStackable();
	
	// keep previous grid index
	AssignHoverItem(ClickedInventoryItem, GridIndex, HoverItem->GetPreviousGridIndex());
	AddItemAtIndex(TempInventoryItem, ItemDropIndex, bTempIsStackable, TempStackCount);
	UpdateGridSlot(TempInventoryItem, ItemDropIndex, bTempIsStackable, TempStackCount);
}

bool UInventoryGrid::ShouldSwapStackCounts(const int32 RoomInClickedSlot, const int32 HoveredStackCount, const int32 MaxStackSize) const
{
	return RoomInClickedSlot == 0 && HoveredStackCount < MaxStackSize;
}

void UInventoryGrid::SwapStackCounts(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index)
{
	UGridSlots* GridSlot = GridSlots[Index];
	GridSlot->SetStackCount(HoveredStackCount);
	
	USlottedItem* ClickedSlottedItem = SlottedItems.FindChecked(Index);
	ClickedSlottedItem->UpdateStackCount(HoveredStackCount);
	
	HoverItem->UpdateStackCount(ClickedStackCount);
}

bool UInventoryGrid::ShouldConsumeHoverItemStacks(const int32 HoveredItemStackCount, const int32 RoomInClickedSlot) const
{
	return RoomInClickedSlot >= HoveredItemStackCount;
}

void UInventoryGrid::ConsumeHoverItemStacks(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index)
{
	const int32 AmountToTransfer = HoveredStackCount;
	const int32 NewClickedStackCount = ClickedStackCount + AmountToTransfer;
	
	GridSlots[Index]->SetStackCount(NewClickedStackCount);
	SlottedItems.FindChecked(Index)->UpdateStackCount(NewClickedStackCount);
	ClearHoverItem();
	ShowCursor();
	
	const FGridFragment* GridFragment = GridSlots[Index]->GetInventoryItem()->GetItemManifest().GetFragmentOfType<FGridFragment>();
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);
	HighlightSlots(Index, Dimensions);
}

bool UInventoryGrid::ShouldFillInStack(const int32 RoomInClickedSlot, const int32 HoveredStackCount) const
{
	return RoomInClickedSlot < HoveredStackCount;
}

void UInventoryGrid::FillInStack(const int32 FillAmount, const int32 Remainder, const int32 Index)
{
	UGridSlots* GridSlot = GridSlots[Index];
	const int32 NewStackCount = GridSlot->GetStackCount() + FillAmount;
	
	GridSlot->SetStackCount(NewStackCount);
	
	USlottedItem* ClickedSlottedItem = SlottedItems.FindChecked(Index);
	ClickedSlottedItem->UpdateStackCount(NewStackCount);
	
	HoverItem->UpdateStackCount(Remainder);
}

/*** CURSOR ***/
/* Comment out for now, don't need custom cursors yet
 * Will create cursors, but leave cursor visible in inventory
 */ 
UUserWidget* UInventoryGrid::GetVisibleCursorWidget()
{
	if (!IsValid(GetOwningPlayer())) return nullptr;
	if (!IsValid(VisibleCursorWidget))
	{
		VisibleCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), VisibleCursorWidgetClass);
	}
	
	return VisibleCursorWidget;
}

UUserWidget* UInventoryGrid::GetHiddenCursorWidget()
{
	if (!IsValid(GetOwningPlayer())) return nullptr;
	if (!IsValid(HiddenCursorWidget))
	{
		HiddenCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), HiddenCursorWidgetClass);
	}
	
	return HiddenCursorWidget;
}

void UInventoryGrid::ShowCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetVisibleCursorWidget());
}

void UInventoryGrid::HideCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::None, GetHiddenCursorWidget());
}

/*** END CURSOR***/

void UInventoryGrid::SetOwningCanvas(UCanvasPanel* OwningCanvas)
{
	OwningCanvasPanel = OwningCanvas;
}


void UInventoryGrid::OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;
	
	UGridSlots* GridSlot = GridSlots[GridIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetOccupiedTexture();
	}
}

void UInventoryGrid::OnGridSlotUnhovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;
	
	UGridSlots* GridSlot = GridSlots[GridIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetUnoccupiedTexture();
	}
}

void UInventoryGrid::OnPopUpMenuSplit(int32 SplitAmount, int32 Index)
{
	UInventoryItem* RightClickedItem = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;
	if (!RightClickedItem->IsStackable()) return;
	
	const int32 UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	UGridSlots* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
	const int32 StackCount = UpperLeftGridSlot->GetStackCount();
	const int32 NewStackCount = StackCount - SplitAmount;
	
	UpperLeftGridSlot->SetStackCount(NewStackCount);
	SlottedItems.FindChecked(UpperLeftIndex)->UpdateStackCount(NewStackCount);
	
	AssignHoverItem(RightClickedItem, UpperLeftIndex, UpperLeftIndex);
	HoverItem->UpdateStackCount(SplitAmount);
	
}

void UInventoryGrid::OnPopUpMenuDrop(int32 Index)
{
	UInventoryItem* RightClickedItem = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;
	
	PickUp(RightClickedItem, Index);
	DropItem();
	
}

void UInventoryGrid::OnPopUpMenuConsume(int32 Index)
{
	
}

void UInventoryGrid::OnPopUpMenuAssign(int32 Index)
{
	if (!SpatialInventory.IsValid())
	{
		return;
	}

	// Get the item at this index
	if (UGridSlots* GridSlot = GridSlots[Index])
	{
		if (UInventoryItem* Item = GridSlot->GetInventoryItem().Get())
		{
			// Show quick slot selection UI
			// For now, just assign to Slot_1 as default
			// TODO: Create a slot selection widget
			SpatialInventory->BeginQuickSlotAssignment(EQuickSlotType::Slot_1);
		
			// Close popup
			if (ItemPopUp)
			{
				ItemPopUp->RemoveFromParent();
				ItemPopUp = nullptr;
			}
		}
	}

}


FVector2D UInventoryGrid::GetDrawSize(const FGridFragment* GridFragment) const 
{
	const float IconTileWidth = SlotSize - (GridFragment->GetGridPadding() * 2);
	return GridFragment->GetGridSize() * IconTileWidth;
}

void UInventoryGrid::SetSlottedItemImage(const USlottedItem* SlottedItem, const FGridFragment* GridFragment, const FImageFragment* ImageFragment) const
{
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image; 
	Brush.ImageSize = GetDrawSize(GridFragment);
	SlottedItem->SetImageBrush(Brush);
}

void UInventoryGrid::SetSpatialInventory(class USpatialInventory* SpatialInv)
{
	SpatialInventory = SpatialInv;
}

bool UInventoryGrid::CanAcceptItem(const UInventoryItem* Item) const
{
	return MatchesPlacementRules(Item);
}


bool UInventoryGrid::MatchesEquipmentRestriction(const UInventoryItem* Item) const
{
	if (!Item) return false;
	
	const FItemManifest& Manifest = Item->GetItemManifest();
	const FItemPlacementRules& Rules = Manifest.GetPlacementRules();
	const EItem_Category ItemCategory = Rules.EquipmentSlot;
	
	// Equipment grids only accept items that match their required equipment type, and that item must be equippable
	if (!IsEquippableItem(ItemCategory)) return false;
	
	// Special cases: Weapon_Secondary slot accepts both Primary and Secondary weapons
	if (RequiredEquipmentType == EItem_Category::Weapon_Secondary)
	{
		return ItemCategory == EItem_Category::Weapon_Primary || ItemCategory == EItem_Category::Weapon_Secondary;
	}
	
	// Standard case: exact match required
	return ItemCategory == RequiredEquipmentType;
	
}

bool UInventoryGrid::MatchesStorageRestriction(const UInventoryItem* Item) const
{
	if (!Item) return false;
	
	const FItemManifest& Manifest = Item->GetItemManifest();
	const FItemPlacementRules& Rules = Manifest.GetPlacementRules();
	
	// First check: can this go in storage?
	if (!Rules.bCanGoInStorage) return false;
	
	// Second check: if item has specific storage restrictions, verify grid qualifies
	if (Rules.AllowedStorageGrids.Num() > 0)
	{
		// Item is restricted to specific storage types
		// Check if this grid's storage type is in the allowed list
		return Rules.AllowedStorageGrids.Contains(StorageGridType);
	}
	
	// Third check: if grid is specialized, chec against allowed itemt types
	if (AllowedItemTypes.Num() > 0)
	{
		// This grid only accepts specific item categories
		return AllowedItemTypes.Contains(Rules.EquipmentSlot);
	}
	
	// No restrictions , item can go here
	return true;
}

bool UInventoryGrid::IsEquippableItem(EItem_Category Category) const
{
	switch (Category)
	{
	case EItem_Category::Head:
	case EItem_Category::Eyewear:
	case EItem_Category::Earwear:
	case EItem_Category::Face:
	case EItem_Category::Armor:
	case EItem_Category::Armband:
	case EItem_Category::Shirt:
	case EItem_Category::Pants:
	case EItem_Category::Backpack:
	case EItem_Category::Rig:
	case EItem_Category::Belt:
	case EItem_Category::Weapon_Primary:
	case EItem_Category::Weapon_Secondary:
	case EItem_Category::Weapon_Holster:
	case EItem_Category::Tool:
		return true;
			
	default:
		return false;
	}
}

bool UInventoryGrid::IsUsableItem(EItem_Category Category) const
{
	switch (Category)
	{
	case EItem_Category::Usable_Medical:
	case EItem_Category::Usable_Food:
	case EItem_Category::Usable_Key:
		return true;
			
	default:
		return false;
	}
}

bool UInventoryGrid::IsMiscItem(EItem_Category Category) const
{
	switch (Category)
	{
	case EItem_Category::Misc_Ammo:
	case EItem_Category::Misc_Other:
		return true;
			
	default:
		return false;
	}
}














