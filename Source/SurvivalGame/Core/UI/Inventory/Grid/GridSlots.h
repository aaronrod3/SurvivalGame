// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurvivalGame/Core/Data/Items/InventoryItem.h"
#include "GridSlots.generated.h"

class UItemPopUp;
class UImage;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGridSlotEvent, int32, GridIndex, const FPointerEvent&, MouseEvent);

UENUM(BlueprintType)
enum class EGridSlotState : uint8
{
	Unoccupied,
	Occupied,
	Selected,
	GrayedOut	
};




UCLASS()
class SURVIVALGAME_API UGridSlots : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	
	void SetTileIndex(int32 Index) {TileIndex = Index;}
	int32 GetTileIndex() const {return TileIndex;}
	EGridSlotState GetGridSlotState() const {return GridSlotState;}
	TWeakObjectPtr<UInventoryItem> GetInventoryItem() const {return InventoryItem;}
	void SetInventoryItem(UInventoryItem* Item);
	int32 GetStackCount() const {return StackCount;}
	void SetStackCount(int32 Count) {StackCount = Count;}
	int32 GetIndex() const {return TileIndex;}
	void SetIndex(int32 Index) {TileIndex = Index;}
	int32 GetUpperLeftIndex() const {return UpperLeftIndex;}
	void SetUpperLeftIndex(int32 Index) {UpperLeftIndex = Index;}
	bool IsAvailable() const {return bAvailable;}
	void SetAvailable(bool bIsAvailable) {bAvailable = bIsAvailable;}
	void SetItemPopUp(UItemPopUp* PopUp);
	UItemPopUp* GetItemPopUp() const;
	
	
	void SetOccupiedTexture();
	void SetGrayedOutTexture();
	void SetSelectedTexture();
	void SetUnoccupiedTexture();
	
	FGridSlotEvent GridSlotClicked;
	FGridSlotEvent GridSlotHovered;
	FGridSlotEvent GridSlotUnhovered;
	
private:
	
	TWeakObjectPtr<UInventoryItem> InventoryItem;
	TWeakObjectPtr<UItemPopUp> ItemPopUp;
	int32 StackCount{0};
	int32 TileIndex{INDEX_NONE};
	int32 UpperLeftIndex{INDEX_NONE};
	bool bAvailable{true};
	
	
	UPROPERTY(meta= (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;
	
	
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Unoccupied;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Occupied;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Selected;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_GrayedOut;
	
	
	EGridSlotState GridSlotState;
	
	UFUNCTION()
	void OnItemPopUpDestruct(UUserWidget* Menu);
	
};














