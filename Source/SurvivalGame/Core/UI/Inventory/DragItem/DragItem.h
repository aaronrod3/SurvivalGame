#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "DragItem.generated.h"

class UInventoryItem;
class UInventoryGrid;

UCLASS()
class SURVIVALGAME_API UDragItem : public UDragDropOperation
{
    GENERATED_BODY()

public:

    UPROPERTY()
    TObjectPtr<UInventoryItem> Item = nullptr;

    UPROPERTY()
    TObjectPtr<UInventoryGrid> SourceGrid = nullptr;

    UPROPERTY()
    int32 SourceIndex = INDEX_NONE;
};