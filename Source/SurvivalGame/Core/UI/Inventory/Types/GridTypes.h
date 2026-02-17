#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GridTypes.generated.h"

class UInventoryItem;


/**
 * Grid Restriction Type
 * 
 * Defines how a grid filters items:
 * - Equipment: Only accepts items with specific EquipmentSlot
 * - Storage: Accepts items based on placement rules
 * - QuickSlot: References items in inventory for quick access
 */
UENUM(BlueprintType)
enum class EGridRestrictionType : uint8
{
	None,
	Equipment     UMETA(DisplayName = "Equipment Slot"),
	Storage       UMETA(DisplayName = "Storage Slot"),
	QuickSlot     UMETA(DisplayName = "Quick Slot")
};



/**
 * Item Category Hierarchy
 * 
 * Defines the type and placement rules for all items.
 * 
 * STRUCTURE:
 * - Root categories define item behavior (Equippable, Usable, Misc)
 * - Sub-categories define specific types
 * - Equipment items occupy dedicated equipment slots
 * - Usable items go in storage or quick slots
 * - Misc items go in storage only
 */
UENUM(BlueprintType)
enum class EItem_Category : uint8
{
	None			UMETA(DisplayName = "None"),
	
	// =============================
	// EQUIPPABLE ITEMS
	// Items that occupy dedicated equipment slots on the character
	// =============================
	
	// Head Equipment
	Head				UMETA(DisplayName = "Head"),
	Eyewear				UMETA(DisplayName = "Eyewear"),
	Earwear				UMETA(DisplayName = "Earwear"),
	Face				UMETA(DisplayName = "Face"),
	
	// Body Equipment
	Armor				UMETA(DisplayName = "Armor"),
	Armband				UMETA(DisplayName = "Armband"),
	Shirt				UMETA(DisplayName = "Shirt"),
	Pants				UMETA(DisplayName = "Pants"),
	
	// Storage Equipment (worn containers that provide storage grids)
	Backpack			UMETA(DisplayName = "Backpack"),
	Rig					UMETA(DisplayName = "Rig"),
	Belt				UMETA(DisplayName = "Belt"),
	
	// Weapon Equipment
	Weapon_Primary		UMETA(DisplayName = "Weapon Primary"),
	Weapon_Secondary	UMETA(DisplayName = "Weapon Secondary"),
	Weapon_Holster		UMETA(DisplayName = "Weapon Holster"),
	
	// Tool Equipment
	Tool				UMETA(DisplayName = "Tool"),
	
	// =============================
	// USABLE ITEMS
	// Items that can be used/consumed and go in storage or quick slots
	// =============================
	
	Usable_Medical		UMETA(DisplayName = "Medical"),
	Usable_Food			UMETA(DisplayName = "Food/Water"),
	Usable_Key			UMETA(DisplayName = "Keys/Access Cards"),
	
	// =============================
	// MISC ITEMS
	// Items that only go in storage slots
	// =============================
	
	Misc_Ammo			UMETA(DisplayName = "Ammo"),
	Misc_Other			UMETA(DisplayName = "Misc")
};


/**
 * Storage Grid Type
 * 
 * Defines the type of storage grid, separate from item categories.
 * Used to identify which storage container a grid belongs to.
 */
UENUM(BlueprintType)
enum class EStorageGridType : uint8
{
	None				UMETA(DisplayName = "None"),
	Backpack			UMETA(DisplayName = "Backpack Slots"),
	Rig					UMETA(DisplayName = "Rig Slots"),
	Belt				UMETA(DisplayName = "Belt Slots"),
	Pocket				UMETA(DisplayName = "Pocket Slots"),
	Container			UMETA(DisplayName = "Generic Container")
};


/**
 * Item Placement Rules
 * 
 * Defines WHERE an item can be placed in the inventory system.
 * 
 * DESIGN PHILOSOPHY:
 * - Equippable items have an EquipmentSlot type and can also go in storage
 * - Usable items have EquipmentSlot = None, can go in storage and quick slots
 * - Misc items have EquipmentSlot = None, can only go in storage
 * 
 * EXAMPLES:
 * 
 * HELMET (Equippable):
 *   EquipmentSlot = Head
 *   bCanGoInStorage = true
 *   AllowedStorageGrids = [] (any storage)
 * 
 * MEDKIT (Usable):
 *   EquipmentSlot = Usable_Medical
 *   bCanGoInStorage = true
 *   bCanGoInQuickSlot = true
 *   AllowedStorageGrids = [] (any storage, or specify medical pouches only)
 * 
 * AMMO (Misc):
 *   EquipmentSlot = Misc_Ammo
 *   bCanGoInStorage = true
 *   bCanGoInQuickSlot = false
 *   AllowedStorageGrids = [] (or specify ammo pouches only)
 * 
 * PRIMARY WEAPON (Equippable):
 *   EquipmentSlot = Weapon_Primary
 *   bCanGoInStorage = true
 *   bCanGoInQuickSlot = false (weapon slots are separate from quick bar)
 *   AllowedStorageGrids = []
 */

// Item Placement Rules
USTRUCT(BlueprintType)
struct FItemPlacementRules
{
	GENERATED_BODY()
	
	/**
	 * Item Category
	 * 
	 * Determines the type of item and its primary placement:
	 * - Equippable categories (Head, Weapon_Primary, etc): Can go in equipment slot OR storage
	 * - Usable categories (Usable_Medical, Usable_Food): Can go in storage AND quick slots
	 * - Misc categories (Misc_Ammo, Misc_Other): Can only go in storage
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	EItem_Category EquipmentSlot = EItem_Category::None;
	
	/**
	 * Can This Item Go In Storage Grids?
	 * 
	 * - true: Item can be placed in storage containers (backpacks, rigs, etc.)
	 * - false: Item CANNOT be stored
	 * 
	 * Most items should be true.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	bool bCanGoInStorage = true;
	
	/**
	 * Can This Item Go In Quick Slots?
	 * 
	 * - true: Item can be assigned to quick slot bar (medical, food, consumables)
	 * - false: Item cannot be quick slotted (equipment, ammo, misc)
	 * 
	 * Only Usable items should have this set to true.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	bool bCanGoInQuickSlot = false;
	
	/**
	 * Allowed Storage Grids (Optional)
	 * 
	 * If EMPTY: Item can go in ANY storage grid
	 * If POPULATED: Item can ONLY go in these specific storage types
	 * 
	 * Use Cases:
	 * - Medical items only in medical pouches (use EStorageGridType)
	 * - Ammo only in ammo pouches
	 * - Keys only in secure pockets
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placement",
			  meta = (EditCondition = "bCanGoInStorage", EditConditionHides))
	TArray<EStorageGridType> AllowedStorageGrids;
	
	// Default constructor
	FItemPlacementRules() 
		: EquipmentSlot(EItem_Category::None)
		, bCanGoInStorage(true)
		, bCanGoInQuickSlot(false)
	{
	}
	
	/**
	 * Constructor with parameters
	 */
	FItemPlacementRules(EItem_Category InEquipmentSlot, bool bInCanGoInStorage = true, bool bInCanGoInQuickSlot = false) 
		: EquipmentSlot(InEquipmentSlot)
		, bCanGoInStorage(bInCanGoInStorage)
		, bCanGoInQuickSlot(bInCanGoInQuickSlot)
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
















