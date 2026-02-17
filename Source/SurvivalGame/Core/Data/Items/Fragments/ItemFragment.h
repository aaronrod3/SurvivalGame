#pragma once


#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemFragment.generated.h"


USTRUCT(BlueprintType)
struct FItemFragment
{
	GENERATED_BODY()
	
	/* TO DERIVE CHILDREN FROM THIS STRUCT, NEED THE FOLLOWING */
	
	//empty
	FItemFragment() {}
	// copy constructor
	FItemFragment(const FItemFragment&) = default;
	// assignment operator
	FItemFragment& operator=(const FItemFragment&) = default;
	// move constructor
	FItemFragment(FItemFragment&&) = default;
	// move assignment
	FItemFragment& operator=(FItemFragment&&) = default;
	// virtual destructor
	virtual ~FItemFragment() {}
	
public:
	
	FGameplayTag GetFragmentTag() const {return FragmentTag;}
	void SetFragmentTag(FGameplayTag Tag) {FragmentTag = Tag;}
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag FragmentTag = FGameplayTag::EmptyTag;
	
	
	
	
};


USTRUCT(BlueprintType)
struct FGridFragment : public FItemFragment
{
	GENERATED_BODY()
	
public:
	
	FIntPoint GetGridSize() const {return GridSize;}
	void SetGridSize(FIntPoint Size) {GridSize = Size;}
	
	float GetGridPadding() const {return GridPadding;}
	void SetGridPadding(float Padding) {GridPadding = Padding;}
	
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FIntPoint GridSize{1,1};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float GridPadding{0.f};
};


USTRUCT(BlueprintType)
struct FImageFragment : public FItemFragment
{
	GENERATED_BODY()
	
public:
	
	UTexture2D* GetIcon() const {return Icon;}
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UTexture2D> Icon = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D IconDimensions{44.f, 44.f};
	
};


USTRUCT(BlueprintType)
struct FStackableFragment : public FItemFragment
{
	GENERATED_BODY()
	
public:
	
	int32 GetMaxStackSize() const {return MaxStackSize;}
	int32 GetStackCount() const {return StackCount;}
	void SetStackCount(int32 Count) {StackCount = Count;}
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxStackSize{1};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 StackCount{1};
	
};


















