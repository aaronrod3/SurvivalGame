// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUDWidget.h"

#include "InfoMessage.h"
#include "SurvivalGame/Character/Inventory/InventoryComponent.h"
#include "SurvivalGame/Core/UI/Utilities/InventoryStatics.h"


void UMainHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	UInventoryComponent* InventoryComponent = UInventoryStatics::GetInventoryComponent(GetOwningPlayer());
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->NoRoomInInventory.AddDynamic(this, &UMainHUDWidget::OnNoRoom);
	}
}


void UMainHUDWidget::OnNoRoom()
{
	if (!IsValid(InfoMessage)) return;
	InfoMessage->SetMessage(FText::FromString("No room in inventory!"));
	
}