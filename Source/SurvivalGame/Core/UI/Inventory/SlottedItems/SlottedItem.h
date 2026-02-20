// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurvivalGame/Core/Data/Items/InventoryItem.h"
#include "SlottedItem.generated.h"

class UImage;
class UInventoryItem;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSlottedItemClicked, int32, GridIndex, const FPointerEvent&, MouseEvent);


UCLASS()
class SURVIVALGAME_API USlottedItem : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	bool GetIsStackable() const {return bIsStackable;}
	void SetIsStackable(bool bStackable) {bIsStackable = bStackable;}
	UImage* GetImage_Icon() const {return Image_Icon;}
	void SetGridIndex(int32 Index) {GridIndex = Index;}
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetGridIndex() const {return GridIndex;}
	
	void SetGridDimensions(FIntPoint Dimensions) {GridDimensions = Dimensions;}
	FIntPoint GetGridDimensions() const {return GridDimensions;}
	void SetInventoryItem(UInventoryItem* Item);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UInventoryItem* GetInventoryItem() const {return InventoryItem.Get();}
	
	
	void SetImageBrush(const FSlateBrush& Brush) const;
	void UpdateStackCount(int32 StackCount);
	
	FSlottedItemClicked OnSlottedItemClicked;
	
private:
	
	//UPROPERTY()
	//FSlateBrush ImageBrush;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_StackCount;
	
	int32 GridIndex;
	FIntPoint GridDimensions;
	TWeakObjectPtr<UInventoryItem> InventoryItem;
	bool bIsStackable{false};
	
};
