#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Inventory_FastArray.generated.h"

struct FGameplayTag;
class UItemComponent;
class UInventoryItem;
class UInventoryComponent;

/* A single entry in an inventory */
USTRUCT(BlueprintType)
struct FInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
	FInventoryEntry() {}
	
	
	
private:
	friend struct FInventoryFastArray;
	friend UInventoryComponent;
	
	UPROPERTY()
	TObjectPtr<UInventoryItem> Item = nullptr;
	
};


/* List of inventory items */
USTRUCT(BlueprintType)
struct FInventoryFastArray : public FFastArraySerializer
{
	GENERATED_BODY()
	
	
	FInventoryFastArray() : OwnerComponent(nullptr) {}
	FInventoryFastArray(UActorComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent) {}
	
	TArray<UInventoryItem*> GetAllItems() const;
	
	// FastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	
	// End of FastArraySerializer contract
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FInventoryEntry, FInventoryFastArray>(InventoryEntries, DeltaParams, *this);
	}
	
	UInventoryItem* AddEntry(UItemComponent* ItemComponent);
	UInventoryItem* AddEntry(UInventoryItem* Item);
	void RemoveEntry(UInventoryItem* Item);
	UInventoryItem* FindFirstItemByType(const FGameplayTag& ItemType);
	
	
private:
	friend UInventoryComponent;
	
	// Replicated list of items
	UPROPERTY()
	TArray<FInventoryEntry> InventoryEntries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent = nullptr;
	
};


template<>
struct TStructOpsTypeTraits<FInventoryFastArray> : TStructOpsTypeTraitsBase2<FInventoryFastArray>
{
	enum { WithNetDeltaSerializer = true };
	
};


















