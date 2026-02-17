# pragma once

#include "CoreMinimal.h"
#include "QuickSlotTypes.generated.h"

class InventoryItem;

/**
 * Quick Slot Type
 * 
 * Defines the type of quick slot for different item categories
 */

UENUM(BlueprintType)
enum class EQuickSlotType : uint8
{
	Weapon_Primary		UMETA(DisplayName = "Primary Weapon"),
	Weapon_Holster		UMETA(DisplayName = "Holster Weapon"),
	Weapon_Secondary	UMETA(DisplayName = "Secondary Weapon (On Back)"),
	Tool				UMETA(DisplayName = "Belt Tool"),
	Slot_1				UMETA(DisplayName = "Slot 1"),
	Slot_2				UMETA(DisplayName = "Slot 2"),
	Slot_3				UMETA(DisplayName = "Slot 3"),
	Slot_4				UMETA(DisplayName = "Slot 4"),
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
	
	/**
	 * Reference to the item in inventory
	 * Weak pointer to avoid circular references
	 */
	UPROPERTY(BlueprintReadOnly, Category = "QuickSlot")
	TWeakObjectPtr<UInventoryItem> ItemReference;
	
	/**
	 * Grid where the item is stored
	 * Weak pointer to the grid widget
	 */
	UPROPERTY(BlueprintReadOnly, Category = "QuickSlot")
	TWeakObjectPtr<class UInventoryGrid> SourceGrid;
	
	/**
	 * Grid index where the item is stored
	 * Used to update or clear the reference if item moves
	 */
	UPROPERTY(BlueprintReadOnly, Category = "QuickSlot")
	int32 StorageGridIndex = INDEX_NONE;
	
	/**
	 * Type of slot this reference occupies
	 */
	UPROPERTY(BlueprintReadOnly, Category = "QuickSlot")
	EQuickSlotType SlotType = EQuickSlotType::Slot_1;
	
	/**
	 * Is this slot currently occupied?
	 */
	UPROPERTY(BlueprintReadOnly, Category = "QuickSlot")
	bool bIsOccupied = false;
	
	// Default constructor
	FQuickSlotReference()
		: ItemReference(nullptr)
		, SourceGrid(nullptr)
		, StorageGridIndex(INDEX_NONE)
		, SlotType(EQuickSlotType::Slot_1)
		, bIsOccupied(false)
	{
	}
	
	/**
	 * Check if reference is valid
	 */
	bool IsValid() const
	{
		return bIsOccupied && ItemReference.IsValid() && SourceGrid.IsValid() && StorageGridIndex != INDEX_NONE;
	}
	
	/**
	 * Clear the reference
	 */
	void Clear()
	{
		ItemReference = nullptr;
		SourceGrid = nullptr;
		StorageGridIndex = INDEX_NONE;
		bIsOccupied = false;
	}
};



















