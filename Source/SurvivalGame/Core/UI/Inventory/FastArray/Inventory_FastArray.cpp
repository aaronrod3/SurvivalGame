
#include "Inventory_FastArray.h"

#include "Developer/LogVisualizer/Internal/VisualLoggerTypes.h"
#include "SurvivalGame/Character/Inventory/InventoryComponent.h"
#include "SurvivalGame/Core/Data/Items/ItemComponent.h"
#include "SurvivalGame/Core/Data/Items/InventoryItem.h"


// Functions

TArray<UInventoryItem*> FInventoryFastArray::GetAllItems() const
{
	TArray<UInventoryItem*> Results;
	Results.Reserve(InventoryEntries.Num());
	for (const auto& Entries : InventoryEntries)
	{
		if (!IsValid(Entries.Item)) continue;
		Results.Add(Entries.Item);
	}
	return Results;
	
}


void FInventoryFastArray::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	UInventoryComponent* IC = Cast<UInventoryComponent>(OwnerComponent);
	if (!IsValid(IC)) return;
	
	for (int32 Index : RemovedIndices)
	{
		IC->OnItemRemoved.Broadcast(InventoryEntries[Index].Item);
	}
	
}

void FInventoryFastArray::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	UInventoryComponent* IC = Cast<UInventoryComponent>(OwnerComponent);
	if (!IsValid(IC)) return;
	
	for (int32 Index : AddedIndices)
	{
		IC->OnItemAdded.Broadcast(InventoryEntries[Index].Item);
	}
	
}


UInventoryItem* FInventoryFastArray::AddEntry(UItemComponent* ItemComponent)
{
	check(OwnerComponent);
	
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());
	UInventoryComponent* IC = Cast<UInventoryComponent>(OwnerComponent);
	if (!IsValid(IC)) return nullptr;
	
	FInventoryEntry& NewEntry = InventoryEntries.AddDefaulted_GetRef();
	NewEntry.Item = ItemComponent->GetItemManifest().Manifest(OwningActor);
	
	IC->AddRepSubObj(NewEntry.Item);
	MarkItemDirty(NewEntry);
	
	return NewEntry.Item;
}

UInventoryItem* FInventoryFastArray::AddEntry(UInventoryItem* Item)
{
	check(OwnerComponent)
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority())
	
	FInventoryEntry& NewEntry = InventoryEntries.AddDefaulted_GetRef();
	NewEntry.Item = Item;
	
	MarkItemDirty(NewEntry);
	return Item;
	
}

void FInventoryFastArray::RemoveEntry(UInventoryItem* Item)
{
	for (auto EntryIterator = InventoryEntries.CreateIterator(); EntryIterator; ++EntryIterator)
	{
		FInventoryEntry& Entry = *EntryIterator;
		if (Entry.Item == Item)
		{
			EntryIterator.RemoveCurrent();
			MarkArrayDirty();
		}
		
	}
	
	
}

UInventoryItem* FInventoryFastArray::FindFirstItemByType(const FGameplayTag& ItemType)
{
	auto* FoundItem = InventoryEntries.FindByPredicate([ItemType = ItemType](const FInventoryEntry& Entry)
	{
		return IsValid(Entry.Item) && Entry.Item->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
	});
	
	return FoundItem ? FoundItem->Item : nullptr;
}














