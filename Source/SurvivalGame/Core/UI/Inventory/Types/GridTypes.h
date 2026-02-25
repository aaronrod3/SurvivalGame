#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GridTypes.generated.h"

class UInventoryItem;


/**
 * Grid Restriction Type
 * 
 * Defines how a grid filters items:
 * - Equipment: Only accepts items with specific EquipmentSlot (e.g., only helmets in head slot)
 * - Storage: Accepts any item that has bCanGoInStorage = true
 * - Specialized: Custom filtering using AllowedItemTypes array
 */
UENUM(BlueprintType)
enum class EGridRestrictionType : uint8
{
	None,
	Equipment     UMETA(DisplayName = "Equipment Slot"),    // Only specific type
	Storage       UMETA(DisplayName = "Storage Slot"),      // Any item
	Specialized   UMETA(DisplayName = "Specialized Slot")   // Custom logic (meds only, ammo only)
	// add hotkey bar
	// add stash
};


/**
 * Item Category / Equipment Slot Type
 * 
 * DUAL PURPOSE ENUM:
 * 1. In FItemPlacementRules.EquipmentSlot: Indicates WHICH equipment slot this item occupies
 * 2. In UInventoryGrid.RequiredEquipmentType: Indicates WHICH equipment type the grid accepts
 * 
 * IMPORTANT: 
 * - "None" means the item is NOT equipment (e.g., medkits, ammo)
 * - "_Slots" categories are STORAGE grids, not equipment slots
 * 
 * Example:
 * - A helmet has EquipmentSlot = Head (occupies head equipment slot)
 * - A medkit has EquipmentSlot = None (not equipment, storage only)
 * - Grid_Head has RequiredEquipmentType = Head (only accepts helmets)
 * - Grid_Backpack_Slots has RestrictionType = Storage (accepts anything)
 */
UENUM(BlueprintType)
enum class EItem_Category : uint8
{
	None			UMETA(DisplayName = "None (Not Equipment)"),
	// Equipment Categories (worn on body)
	Head			UMETA(DisplayName = "Head"),
	Ears			UMETA(DisplayName = "Ears"),
	Face			UMETA(DisplayName = "Face"),
	Eye				UMETA(DisplayName = "Eye"),
	Armband			UMETA(DisplayName = "Armband"),
	Shirt			UMETA(DisplayName = "Shirt"),
	Pants			UMETA(DisplayName = "Pants"),
	Rig				UMETA(DisplayName = "Rig"),
	Backpack		UMETA(DisplayName = "Backpack"),
	Belt			UMETA(DisplayName = "Belt"),
	Pocket			UMETA(DisplayName = "Pocket"),
	
	// Storage Categories (containers attached to equipment)
	// NOTE: These are NOT equipment slots - they're storage grids
	Rig_Slots		UMETA(DisplayName = "Rig Slots"),
	Backpack_Slots	UMETA(DisplayName = "Backpack Slots"),
	Belt_Slots		UMETA(DisplayName = "Belt Slots"),
	Pocket_Slots	UMETA(DisplayName = "Pocket Slots")
};

/**
 * Item Placement Rules
 * 
 * Defines WHERE an item can be placed in the inventory system.
 * 
 * DESIGN PHILOSOPHY:
 * - Equipment items (helmets, backpacks) can go in their equipment slot OR storage
 * - Storage items (medkits, ammo) skip equipment slots and go directly to storage
 * - Some items (like backpacks) can't be stored inside themselves
 * 
 * EXAMPLES:
 * 
 * HELMET:
 *   EquipmentSlot = Head              // "I occupy the head equipment slot"
 *   bCanGoInStorage = true             // "I can also be stored in backpack/rig"
 *   AllowedStorageGrids = []           // "I can go in ANY storage grid"
 * 
 * MEDKIT:
 *   EquipmentSlot = None               // "I'm NOT equipment"
 *   bCanGoInStorage = true             // "I'm a storage item"
 *   AllowedStorageGrids = []           // "I can go in ANY storage grid"
 * 
 * BACKPACK:
 *   EquipmentSlot = Backpack           // "I occupy the backpack equipment slot"
 *   bCanGoInStorage = false            // "I CANNOT be stored (can't store backpack in backpack)"
 *   AllowedStorageGrids = []           // (not used since bCanGoInStorage = false)
 * 
 * MEDICAL ITEM (Specialized):
 *   EquipmentSlot = None               // "I'm NOT equipment"
 *   bCanGoInStorage = true             // "I'm a storage item"
 *   AllowedStorageGrids = [MedPouch]   // "I can ONLY go in medical pouches"
 */

// Item Placement Rules
USTRUCT(BlueprintType)
struct FItemPlacementRules
{
	GENERATED_BODY()
	
	/**
	 * Equipment Slot This Item Occupies
	 * 
	 * - None: Item is NOT equipment (medkits, ammo, food)
	 * - Head/Backpack/etc: Item IS equipment and occupies that slot
	 * 
	 * When routing items:
	 * 1. If EquipmentSlot != None → Try equipment grid first
	 * 2. If EquipmentSlot == None → Skip equipment, go to storage
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	EItem_Category EquipmentSlot = EItem_Category::None;
	
	/**
	 * Can This Item Go In Storage Grids?
	 * 
	 * - true: Item can be placed in storage containers (backpacks, rigs, etc.)
	 * - false: Item CANNOT be stored (e.g., backpacks can't store themselves)
	 * 
	 * Most items should be true. Only set false for special cases.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	bool bCanGoInStorage = true;
	
	/**
	 * Allowed Storage Grids (Optional)
	 * 
	 * If EMPTY: Item can go in ANY storage grid
	 * If POPULATED: Item can ONLY go in these specific storage grids
	 * 
	 * Use Cases:
	 * - Medical items only in med pouches
	 * - Ammo only in ammo pouches
	 * - Keys only in key rings
	 * 
	 * NOTE: Only used if bCanGoInStorage = true
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placement",
			  meta = (EditCondition = "bCanGoInStorage", EditConditionHides))
	TArray<EItem_Category> AllowedStorageGrids;
	
	// Default constructor
	FItemPlacementRules() : EquipmentSlot(EItem_Category::None), bCanGoInStorage(true)
	{
	}
	
	/**
	 * Constructor with Equipment Slot
	 */
	FItemPlacementRules(EItem_Category InEquipmentSlot, bool bInCanGoInStorage = true) : EquipmentSlot(InEquipmentSlot) , bCanGoInStorage(bInCanGoInStorage) 
	{
		
	}
};

// Info for individual slot
USTRUCT()
struct FSlotAvailability
{
	GENERATED_BODY()
	
	FSlotAvailability() {}
	FSlotAvailability(int32 ItemIndex, int32 Room, bool bHasItem) : Index(ItemIndex), AmountToFill(Room), bItemAtIndex(bHasItem) {}
	
	int32 Index{INDEX_NONE};
	int32 AmountToFill{0};
	bool bItemAtIndex{false};
};

// Info for inventory as a whole
USTRUCT()
struct FSlotAvailabilityResult
{
	GENERATED_BODY()
	
	FSlotAvailabilityResult() {}
	
	TWeakObjectPtr<UInventoryItem> Item;
	int32 TotalRoomToFill{0};
	int32 Remainder{0};
	bool bStackable{false};
	TArray<FSlotAvailability> SlotAvailabilities;
	
};

UENUM(BlueprintType)
enum class ETileQuadrant : uint8
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
	None
};

USTRUCT(BlueprintType)
struct FTileParameters
{
	GENERATED_BODY()

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	FIntPoint TileCoordinates{};
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	int32 TileIndex{INDEX_NONE};
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	ETileQuadrant TileQuadrant{ETileQuadrant::None};
	
};

inline bool operator==(const FTileParameters& A, const FTileParameters& B)
{
	return A.TileCoordinates == B.TileCoordinates && A.TileIndex == B.TileIndex && A.TileQuadrant == B.TileQuadrant;
}


USTRUCT()
struct FSpaceQueryResult
{
	GENERATED_BODY()
	
	// true if space queried has no items in it
	bool bHasSpace;
	
	// valid if theres a single item we can swap with
	TWeakObjectPtr<UInventoryItem> ValidItem = nullptr;
	
	// upper left index of the valid item, if there is one
	int32 UpperLeftIndex{INDEX_NONE};
	
	
};
















