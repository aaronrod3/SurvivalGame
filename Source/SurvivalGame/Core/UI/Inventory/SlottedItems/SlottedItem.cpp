// Fill out your copyright notice in the Description page of Project Settings.


#include "SlottedItem.h"
#include "SurvivalGame/Core/Data/Items/InventoryItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


FReply USlottedItem::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	OnSlottedItemClicked.Broadcast(GridIndex, MouseEvent);
	return FReply::Handled();
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
