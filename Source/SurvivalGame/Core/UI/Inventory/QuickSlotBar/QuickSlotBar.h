// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurvivalGame/Core/UI/Inventory/Types/QuickSlotTypes.h"
#include "QuickSlotBar.generated.h"

class UInventoryItem;
class USpatialInventory;
class UQuickSlotEntry;

// Fired when a slot entry receives a drop
DECLARE_DYNAMIC_DELEGATE_TwoParams(FQuickSlotDropped, EQuickSlotType, SlotType, class UDragDropOperation*, Operation);



UCLASS()
class SURVIVALGAME_API UQuickSlotBar : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	virtual void NativeOnInitialized() override;
	
	// called by SpatialInventory->OnQuickSlotUpdated delegate
	UFUNCTION()
	void OnQuickSlotUpdated(EQuickSlotType SlotType, UInventoryItem* Item);
	
	// Sets the owning spatial inventory so slots can call back into it on drop
	void SetSpatialInventory(USpatialInventory* Inventory);
	
private:
	
	// Bind Widget
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UQuickSlotEntry> QuickSlot_Weapon_Primary;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UQuickSlotEntry> QuickSlot_Weapon_Holster;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UQuickSlotEntry> QuickSlot_Weapon_Secondary;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UQuickSlotEntry> QuickSlot_Tool;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UQuickSlotEntry> QuickSlot_5;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UQuickSlotEntry> QuickSlot_6;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UQuickSlotEntry> QuickSlot_7;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UQuickSlotEntry> QuickSlot_8;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UQuickSlotEntry> QuickSlot_9;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UQuickSlotEntry> QuickSlot_0;
	
	UPROPERTY(EditAnywhere, Category = "QuickSlotBar")
	float SlotSize = 64.f;
	
	UPROPERTY()
	TWeakObjectPtr<USpatialInventory> SpatialInventory;
	
	void AssignItemToSlot(UQuickSlotEntry* Entry, UInventoryItem* Item) const;
	
	// returns the entry widget for a given slot type
	UQuickSlotEntry* GetQuickSlotEntry(EQuickSlotType SlotType) const;
};













