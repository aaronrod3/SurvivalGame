// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryGrid.h"
#include "Components/WidgetSwitcher.h"
#include "SurvivalGame/Core/UI/Inventory/InventoryBase.h"
#include "SpatialInventory.generated.h"

class UCanvasPanel;

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
	
private:
	
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
	
	
	
	
	// add quick slot bar
	
	// add stash later
	
};
