// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSlots.h"
#include "SurvivalGame/Core/Data/Items/InventoryItem.h"
#include "SurvivalGame/Core/UI/Inventory/ItemPopUp/ItemPopUp.h"
#include "Components/Image.h"


void UGridSlots::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, MouseEvent);
	
	GridSlotHovered.Broadcast(TileIndex, MouseEvent);
	
}

void UGridSlots::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseLeave(MouseEvent);
	
	GridSlotUnhovered.Broadcast(TileIndex, MouseEvent);

}

FReply UGridSlots::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	GridSlotClicked.Broadcast(TileIndex, MouseEvent);
	return FReply::Handled();

}

void UGridSlots::SetInventoryItem(UInventoryItem* Item)
{
	InventoryItem = Item;
}



void UGridSlots::SetUnoccupiedTexture()
{
	GridSlotState = EGridSlotState::Unoccupied;
	Image_GridSlot->SetBrush(Brush_Unoccupied);
}




void UGridSlots::SetItemPopUp(UItemPopUp* PopUp)
{
	ItemPopUp = PopUp;
	ItemPopUp->SetGridIndex(GetIndex());
	ItemPopUp->OnNativeDestruct.AddUObject(this, &ThisClass::OnItemPopUpDestruct);
}

UItemPopUp* UGridSlots::GetItemPopUp() const
{
	return ItemPopUp.Get();
}

void UGridSlots::OnItemPopUpDestruct(UUserWidget* Menu)
{
	ItemPopUp.Reset();
}


void UGridSlots::SetOccupiedTexture()
{
	GridSlotState = EGridSlotState::Occupied;
	Image_GridSlot->SetBrush(Brush_Occupied);
}

void UGridSlots::SetSelectedTexture()
{
	GridSlotState = EGridSlotState::Selected;
	Image_GridSlot->SetBrush(Brush_Selected);
}

void UGridSlots::SetGrayedOutTexture()
{
	GridSlotState = EGridSlotState::GrayedOut;
	Image_GridSlot->SetBrush(Brush_GrayedOut);
}






