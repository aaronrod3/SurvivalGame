
#include "ItemManifest.h"

#include "SurvivalGame/Core/Data/Items/InventoryItem.h"
#include "SurvivalGame/Core/Data/Items/ItemComponent.h"

UInventoryItem* FItemManifest::Manifest(UObject* NewOuter) const
{
	UInventoryItem* Item = NewObject<UInventoryItem>(NewOuter, UInventoryItem::StaticClass());
	Item->SetItemManifest(*this);
	
	return Item;
}


void FItemManifest::SpawnPickupActor(const UObject* WorldContextObject, const FVector& SpawnLocation,
	const FRotator& SpawnRotation)
{
	if (!IsValid(PickupActorClass) || !IsValid(WorldContextObject)) return;
	
	AActor* SpawnedActor = WorldContextObject->GetWorld()->SpawnActor<AActor>(PickupActorClass, SpawnLocation, SpawnRotation);
	if (!IsValid(SpawnedActor)) return;
	
	// set the item manifest, item category, item type, etc
	UItemComponent* ItemComp = SpawnedActor->FindComponentByClass<UItemComponent>();
	if (!ItemComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawned actor does not have an item component"));
		SpawnedActor->Destroy();
		return;
	}
	
	ItemComp->InitItemManifest(*this);
}
















