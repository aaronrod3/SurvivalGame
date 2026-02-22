# pragma once

#include "CoreMinimal.h"
#include "QuickSlotTypes.generated.h"

class UInventoryItem;
class UInventoryGrid;

/**
 * Quick Slot Type
 * 
 * Defines the type of quick slot for different item categories
 */

UENUM(BlueprintType)
enum class EQuickSlotType : uint8
{
	None				UMETA(DisplayName = "None"),
	Weapon_Primary		UMETA(DisplayName = "Primary Weapon"),
	Weapon_Holster		UMETA(DisplayName = "Holster Weapon"),
	Weapon_Secondary	UMETA(DisplayName = "Secondary Weapon (On Back)"),
	Tool				UMETA(DisplayName = "Belt Tool"),
	Slot_5				UMETA(DisplayName = "Slot 5"),
	Slot_6				UMETA(DisplayName = "Slot 6"),
	Slot_7				UMETA(DisplayName = "Slot 7"),
	Slot_8				UMETA(DisplayName = "Slot 8"),
	Slot_9				UMETA(DisplayName = "Slot 9"),
	Slot_0				UMETA(DisplayName = "Slot 0")
};


/**
 * Quick Slot Reference
 * 
 * References an item in the inventory for quick access.
 * The item remains in its storage location.
 */
USTRUCT(BlueprintType)
struct FQuickSlotReference
{
	GENERATED_BODY()
	
	UPROPERTY()
	EQuickSlotType SlotType = EQuickSlotType::None;

	UPROPERTY()
	TWeakObjectPtr<UInventoryItem> ItemReference = nullptr;

	UPROPERTY()
	TWeakObjectPtr<UInventoryGrid> SourceGrid = nullptr;

	UPROPERTY()
	int32 StorageGridIndex = INDEX_NONE;
	
	UPROPERTY()
	bool bIsOccupied = false;

	bool IsValid() const
	{
		return SlotType != EQuickSlotType::None
			&& ItemReference.IsValid()
			&& SourceGrid.IsValid()
			&& StorageGridIndex != INDEX_NONE;
	}

	void Clear()
	{
		ItemReference = nullptr;
		SourceGrid = nullptr;
		StorageGridIndex = INDEX_NONE;
	}
};



















