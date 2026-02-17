
#include "ItemManifest.h"

#include "SurvivalGame/Core/Data/Items/InventoryItem.h"
#include "SurvivalGame/Core/Data/Items/ItemComponent.h"

UInventoryItem* FItemManifest::Manifest(UObject* NewOuter) const
{
	UInventoryItem* Item = NewObject<UInventoryItem>(NewOuter, UInventoryItem::StaticClass());
	Item->SetItemManifest(*this);
	
	return Item;
}

FText FItemManifest::GetItemName() const
{
	/*
	const FNameFragment* NameFragment = GetFragmentOfType<FNameFragment>();
	if (NameFragment)
	{
		return NameFragment->GetItemName();
	}
	return FText::FromString(TEXT("Unknown Item"));
	*/
	
	// Stub: Use ItemType for debugging until NameFragment is ready
	if (ItemType.IsValid())
	{
		return FText::FromString(ItemType.ToString());
	}
	return FText::FromString(TEXT("Unknown Item"));
}

void FItemManifest::SpawnPickupActor(const UObject* WorldContextObject, const FVector& SpawnLocation,
	const FRotator& SpawnRotation)
{
	if (!IsValid(PickupActorClass) || !IsValid(WorldContextObject)) return;
	
	AActor* SpawnedActor = WorldContextObject->GetWorld()->SpawnActor<AActor>(PickupActorClass, SpawnLocation, SpawnRotation);
	if (!IsValid(SpawnedActor)) return;
	
	// set the item manifest, item category, item type, etc
	UItemComponent* ItemComp = SpawnedActor->FindComponentByClass<UItemComponent>();
	check(ItemComp);
	
	ItemComp->InitItemManifest(*this);
}
















