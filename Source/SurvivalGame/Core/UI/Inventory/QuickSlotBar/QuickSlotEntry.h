// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurvivalGame/Core/UI/Inventory/Types/QuickSlotTypes.h"
#include "QuickSlotEntry.generated.h"

class UImage;
class UTextBlock;
class UInventoryItem;
class UDragDropOperation;

// Fired upward to QuickSlotBar when a valid drag is dropped here
DECLARE_DELEGATE_TwoParams(FOnQuickSlotEntryDropped, EQuickSlotType, UDragDropOperation*);

/**
 *  This is per-slot widget. Mirrors SlottemItem - has an icon image, recevies drops, calls back
 */
UCLASS()
class SURVIVALGAME_API UQuickSlotEntry : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Drag target
	virtual bool NativeOnDrop(const FGeometry& InGeometry,
							  const FDragDropEvent& InDragDropEvent,
							  UDragDropOperation* InOperation) override;
	
	
	void SetImageBrush(const FSlateBrush& Brush) const;
	void SetInventoryItem(UInventoryItem* Item);
	UInventoryItem* GetInventoryItem() const { return InventoryItem.Get(); }

	void Clear();

	void SetSlotType(EQuickSlotType InSlotType) { SlotType = InSlotType; }
	EQuickSlotType GetSlotType() const { return SlotType; }

	// Optional: let BP react whenever assigned/cleared (visual control stays in BP)
	UFUNCTION(BlueprintImplementableEvent, Category="QuickSlot")
	void OnSlotChanged(UInventoryItem* Item);

	// Bound by QuickSlotBar
	FOnQuickSlotEntryDropped OnDropped;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_SlotNumber;

	UPROPERTY()
	EQuickSlotType SlotType = EQuickSlotType::None;

	UPROPERTY()
	TWeakObjectPtr<UInventoryItem> InventoryItem;
	
};











