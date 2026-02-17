// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemComponent.h"
#include "Net/UnrealNetwork.h"


UItemComponent::UItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	PickupMessage = FString ("F - Pick Up");
	
}

void UItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, Manifest);
}

void UItemComponent::InitItemManifest(FItemManifest CopyOfManifest)
{
	Manifest = CopyOfManifest;
}

void UItemComponent::PickedUp()
{
	OnPickedUp();
	GetOwner()->Destroy();
}



