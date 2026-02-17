#pragma once


#include "CoreMinimal.h"
#include "SurvivalGame/Core/UI/Inventory/Types/GridTypes.h"
#include "GamePlayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "SurvivalGame/Core/Data/Items/Fragments/ItemFragment.h"
#include "ItemManifest.generated.h"


/** The item manifest contains all necessary data for creating new inventory item **/

class UInventoryItem;

USTRUCT(BlueprintType)
struct SURVIVALGAME_API FItemManifest
{
	GENERATED_BODY()
	
public:
	
	UInventoryItem* Manifest(UObject* NewOuter) const;
	
	const FItemPlacementRules& GetPlacementRules() const { return PlacementRules; }
	FText GetItemName() const;
	FGameplayTag GetItemType() const {return ItemType;}
	EItem_Category GetItemCategory() const { return PlacementRules.EquipmentSlot; }
	
	
	template <typename T> requires std::derived_from<T, FItemFragment>
	const T* GetFragmentOfTypeWithTag(const FGameplayTag& Tag) const;
	
	template <typename T> requires std::derived_from<T, FItemFragment>
	const T* GetFragmentOfType() const;
	
	template <typename T> requires std::derived_from<T, FItemFragment>
	T* GetFragmentofTypeMutable();
	
	
	void SpawnPickupActor(const UObject* WorldContextObject, const FVector& SpawnLocation, const FRotator& SpawnRotation);
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct));
	TArray<TInstancedStruct<FItemFragment>> Fragments;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FItemPlacementRules PlacementRules;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag ItemType;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<AActor> PickupActorClass;
	
	
};


template <typename T>
requires std::derived_from<T, FItemFragment>
const T* FItemManifest::GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const
{
	for (const TInstancedStruct<FItemFragment>& Fragment : Fragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			if (!FragmentPtr->GetFragmentTag().MatchesTagExact(FragmentTag)) continue;
			return FragmentPtr;
		}
		
		
	}
	
	return nullptr;
}

template <typename T> requires std::derived_from<T, FItemFragment>
const T* FItemManifest::GetFragmentOfType() const
{
	for (const TInstancedStruct<FItemFragment>& Fragment : Fragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			return FragmentPtr;
		}
		
		
	}
	
	return nullptr;
}

template <typename T> requires std::derived_from<T, FItemFragment>
T* FItemManifest::GetFragmentofTypeMutable()
{
	for (TInstancedStruct<FItemFragment>& Fragment : Fragments)
	{
		if (T* FragmentPtr = Fragment.GetMutablePtr<T>())
		{
			return FragmentPtr;
		}
		
		
	}
	
	return nullptr;
}





















