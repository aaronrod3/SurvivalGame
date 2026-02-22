// Fill out your copyright notice in the Description page of Project Settings.


#include "SlottedItem.h"

#include "InputState.h"
#include "SurvivalGame/Core/Data/Items/InventoryItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "SurvivalGame/Core/UI/Inventory/DragItem/DragItem.h"


FReply USlottedItem::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		// queue drag detection if mouse doesn't move past threshold
		return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	
	// Right click fires the context menu delegate 
	OnSlottedItemClicked.Broadcast(GridIndex, MouseEvent);
	return FReply::Handled();
}

void USlottedItem::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (!InventoryItem.IsValid()) return;
	
	UDragItem* DragOp = NewObject<UDragItem>(this);
	DragOp->Item				= InventoryItem.Get();
	DragOp->SourceGrid			= OwningGrid.Get();
	DragOp->SourceIndex			= GridIndex;
	DragOp->DefaultDragVisual	= this;						// widget follows the cursor
	DragOp-> Pivot				= EDragPivot::MouseDown;
	
	OutOperation = DragOp;
}

void USlottedItem::SetInventoryItem(UInventoryItem* Item)
{
	InventoryItem = Item;
}

void USlottedItem::SetImageBrush(const FSlateBrush& Brush) const
{
	Image_Icon->SetBrush(Brush);
}

void USlottedItem::UpdateStackCount(int32 StackCount)
{
	if (StackCount > 0)
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Visible);
		Text_StackCount->SetText(FText::AsNumber(StackCount));
	}
	else
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}
