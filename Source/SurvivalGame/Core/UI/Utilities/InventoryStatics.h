// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetUtilities.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SurvivalGame/Core/Data/Items/ItemComponent.h"
#include "SurvivalGame/Core/UI/Inventory/Types/GridTypes.h"
#include "InventoryStatics.generated.h"

class UInventoryComponent;
/**
 * 
 */
UCLASS()
class SURVIVALGAME_API UInventoryStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static UInventoryComponent* GetInventoryComponent(const APlayerController* PlayerController);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static EItem_Category GetItemCategoryFromItemComp(UItemComponent* Item);
	
	template<typename T, typename FuncT>
	static void ForEach2D(TArray<T>& Array, int32 Index, const FIntPoint& Range2D, int32 GridColumns, const FuncT& Function);
};


template <typename T, typename FuncT>
void UInventoryStatics::ForEach2D(TArray<T>& Array, int32 Index, const FIntPoint& Range2D, int32 GridColumns, const FuncT& Function)
{
	for (int32 j = 0; j < Range2D.Y; j++)
	{
		for (int32 i = 0; i < Range2D.X; i++)
		{
			const FIntPoint Coordinates = UWidgetUtilities::GetPositionFromIndex(Index, GridColumns) + FIntPoint(i, j);
			const int32 TileIndex = UWidgetUtilities::GetIndexFromPosition(Coordinates, GridColumns);
			
			if (Array.IsValidIndex(TileIndex))
			{
				Function(Array[TileIndex]);
			}
		}
	}
}

























