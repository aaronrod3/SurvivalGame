// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickSlotEntry.h"
#include "SurvivalGame/Core/UI/Inventory/DragItem/DragItem.h"
#include "SurvivalGame/Core/Data/Items/InventoryItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


bool UQuickSlotEntry::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// only accept UDragItem drops - ignore everything else
	UDragItem* DragOp = Cast<UDragItem>(InOperation);
	if (!DragOp || !DragOp->Item) return false;
	
	// Fire upward to QuickSlotBar which has SpatialInventory context
	if (OnDropped.IsBound())
	{
		OnDropped.Execute(SlotType, InOperation);
	}
	
	return true;
}

void UQuickSlotEntry::SetImageBrush(const FSlateBrush& Brush) const
{
	if (Image_Icon) Image_Icon->SetBrush(Brush);
}

void UQuickSlotEntry::SetInventoryItem(UInventoryItem* Item)
{
	InventoryItem = Item;
	OnSlotChanged(Item);
}

void UQuickSlotEntry::Clear()
{
	InventoryItem = nullptr;
	
	if (Image_Icon)
	{
		Image_Icon->SetBrush(FSlateNoResource());
	}
	
	OnSlotChanged(nullptr);
}











