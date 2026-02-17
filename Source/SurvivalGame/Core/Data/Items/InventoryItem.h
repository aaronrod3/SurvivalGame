// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Manifest/ItemManifest.h"
#include "InventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API UInventoryItem : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; };
	
	
	void SetItemManifest(const FItemManifest& Manifest);
	
	const FItemManifest& GetItemManifest() const {return ItemManifest.Get<FItemManifest>(); }
	FItemManifest& GetItemManifestMutable() {return ItemManifest.GetMutable<FItemManifest>(); }
	bool IsStackable() const;
	bool IsUsable() const;
	int32 GetTotalStackCount() const {return TotalStackCount;}
	void SetTotalStackCount(int32 Count) {TotalStackCount = Count;}
	
	
private:
	
	UPROPERTY(VisibleAnywhere, meta = (BaseStruct = "/Script/SurvivalGame.ItemManifest"), Replicated)
	FInstancedStruct ItemManifest;
	
	UPROPERTY(Replicated)
	int32 TotalStackCount{0};
	
};

template <typename T>
const T* GetFragment(const UInventoryItem* Item, const FGameplayTag& Tag)
{
	if (!IsValid(Item)) return nullptr;
	
	const FItemManifest& Manifest = Item->GetItemManifest();
	
	return Manifest.GetFragmentOfTypeWithTag<T>(Tag);
}

















