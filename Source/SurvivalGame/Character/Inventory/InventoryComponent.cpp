// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "SurvivalGame/Core/Data/Items/ItemComponent.h"
#include "SurvivalGame/Core/Data/Items/InventoryItem.h"
#include "SurvivalGame/Core/UI/Inventory/InventoryBase.h"


DEFINE_LOG_CATEGORY(LogInventory);


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent() : InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
	bInventoryMenuOpen = false;
}



void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ConstructInventory();
	
}



// FUNCTIONS

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UInventoryComponent, InventoryList);
}

void UInventoryComponent::ConstructInventory()
{
	OwningController = Cast<APlayerController>(GetOwner());
	checkf(OwningController.IsValid(), TEXT("Inventory Component should have a player controller as owner."))
	
	if (!OwningController->IsLocalController()) return;
	
	InventoryMenu = CreateWidget<UInventoryBase>(OwningController.Get(), InventoryMenuClass);
	
	if (!IsValid(InventoryMenu)) return;
	
	InventoryMenu->AddToViewport();
	CloseInventoryMenu();
	
}

void UInventoryComponent::Server_DropItem_Implementation(UInventoryItem* Item, int32 StackCount)
{
	const int32 NewStackCount = Item->GetTotalStackCount() - StackCount;
	if (NewStackCount <= 0)
	{
		InventoryList.RemoveEntry(Item);
	}
	else
	{
		Item->SetTotalStackCount(NewStackCount);
	}
	
	SpawnDroppedItem(Item, StackCount);
}

void UInventoryComponent::SpawnDroppedItem(UInventoryItem* Item, int32 StackCount)
{
	const APawn* OwningPawn = OwningController->GetPawn();
	FVector RotatedForward = OwningPawn->GetActorForwardVector();
	RotatedForward = RotatedForward.RotateAngleAxis(FMath::FRandRange(DropSpawnAngleMin, DropSpawnAngleMax), FVector::UpVector);
	FVector SpawnLocation = OwningPawn->GetActorLocation() + RotatedForward * FMath::FRandRange(DropSpawnDistanceMin, DropSpawnDistanceMax);
	SpawnLocation.Z -= RelativeSpawnElevation;
	const FRotator SpawnRotation = FRotator::ZeroRotator;
	
	FItemManifest& ItemManifest = Item->GetItemManifestMutable();
	if (FStackableFragment* StackableFragment = ItemManifest.GetFragmentofTypeMutable<FStackableFragment>())
	{
		StackableFragment->SetStackCount(StackCount);
	}
	ItemManifest.SpawnPickupActor(this, SpawnLocation, SpawnRotation);
}


void UInventoryComponent::ToggleInventoryMenu()
{
	if (bInventoryMenuOpen)
	{
		CloseInventoryMenu();
	}
	else
	{
		OpenInventoryMenu();
	}
}

void UInventoryComponent::OpenInventoryMenu()
{
	if (!IsValid(InventoryMenu) || !OwningController.IsValid()) return;
	
	InventoryMenu->SetVisibility(ESlateVisibility::Visible);
	bInventoryMenuOpen = true;
	
	FInputModeGameAndUI InputMode;
	OwningController->SetInputMode(InputMode);
	OwningController->SetShowMouseCursor(true);
	
}

void UInventoryComponent::CloseInventoryMenu()
{
	if (!IsValid(InventoryMenu) || !OwningController.IsValid()) return;
	
	InventoryMenu->SetVisibility((ESlateVisibility::Collapsed));
	bInventoryMenuOpen = false;
	
	FInputModeGameOnly InputMode;
	OwningController->SetInputMode(InputMode);
	OwningController->SetShowMouseCursor(false);
	
	
}


void UInventoryComponent::TryAddItem(UItemComponent* ItemComponent)
{
	FSlotAvailabilityResult Result = InventoryMenu->HasRoomForItem(ItemComponent);
	
	UInventoryItem* FoundItem = InventoryList.FindFirstItemByType(ItemComponent->GetItemManifest().GetItemType());
	Result.Item = FoundItem;
	
	if (Result.TotalRoomToFill == 0)
	{
		NoRoomInInventory.Broadcast();
		return;
	}
	
	if (Result.Item.IsValid() && Result.bStackable)
	{
		// add stacks to an item that already exists. only want to update stack count, not create new item
		OnStackChange.Broadcast(Result);
		Server_AddStacksToItem(ItemComponent, Result.TotalRoomToFill, Result.Remainder);
	}
	else if (Result.TotalRoomToFill > 0)
	{
		// This item type doesnt exist in the inventory, create a new one, and create all pertinent slots
		Server_AddNewItem(ItemComponent, Result.bStackable ? Result.TotalRoomToFill : 0);
	}
	
}


void UInventoryComponent::Server_AddNewItem_Implementation(UItemComponent* ItemComponent, int32 StackCount)
{
	UInventoryItem* NewItem = InventoryList.AddEntry(ItemComponent);
	NewItem->SetTotalStackCount(StackCount);
	
	if (GetOwner()->GetNetMode() == NM_ListenServer || GetOwner()->GetNetMode() == NM_Standalone)
	{
		OnItemAdded.Broadcast(NewItem);
	}
	
	ItemComponent->PickedUp();
}

void UInventoryComponent::Server_AddStacksToItem_Implementation(UItemComponent* ItemComponent, int32 StackCount, int32 Remainder)
{
	const FGameplayTag& ItemType = IsValid(ItemComponent) ? ItemComponent->GetItemManifest().GetItemType() : FGameplayTag::EmptyTag;
	UInventoryItem* Item = InventoryList.FindFirstItemByType(ItemType);
	if (!IsValid(Item)) return;
	
	Item->SetTotalStackCount(Item->GetTotalStackCount() + StackCount);
	
	if (Remainder == 0)
	{
		 ItemComponent->PickedUp();
	}
	else if (FStackableFragment* StackableFragment = ItemComponent->GetItemManifest().GetFragmentofTypeMutable<FStackableFragment>())
	{
		StackableFragment->SetStackCount(Remainder);
	}

}

void UInventoryComponent::AddRepSubObj(UObject* SubObj)
{
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && IsValid(SubObj))
	{
		AddReplicatedSubObject(SubObj);
	}
	
}















