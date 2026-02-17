// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItem.h"
#include "SurvivalGame/Core/Data/Items/Fragments/ItemFragment.h"
#include "Net/UnrealNetwork.h"


void UInventoryItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, ItemManifest);
	DOREPLIFETIME(ThisClass, TotalStackCount);
}

void UInventoryItem::SetItemManifest(const FItemManifest& Manifest)
{
	ItemManifest = FInstancedStruct::Make<FItemManifest>(Manifest);
}

bool UInventoryItem::IsStackable() const
{
	const FStackableFragment* Stackable = GetItemManifest().GetFragmentOfType<FStackableFragment>();
	return Stackable != nullptr;
}


bool UInventoryItem::IsUsable() const
{
	// TODO: switch to this function after implementing fragment usable type
	/*
	// check if item has a usable fragment
	const FUsableFragment* UsableFragment = GetItemManifest().GetFragmentOfType<FUsableFragment>();
	return UsableFragment != nullptr;
	*/
	
	// Check if item type is usable (medical, food, consumable)
	const FGameplayTag& Type = GetItemManifest().GetItemType();
	return Type.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Medical")) ||
		   Type.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Food")) ||
		   Type.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Consumable"));
	
	
}
