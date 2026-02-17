// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Manifest/ItemManifest.h"
#include "ItemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class SURVIVALGAME_API UItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UItemComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void InitItemManifest(FItemManifest CopyOfManifest);
	FItemManifest GetItemManifest() const {return Manifest;}
	FString GetPickupMessage() const {return PickupMessage;}

	void PickedUp();
	
protected:
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnPickedUp();
	
private:
	
	UPROPERTY(Replicated, EditAnywhere, Category = "Inventory")
	FItemManifest Manifest;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FString PickupMessage;
};
