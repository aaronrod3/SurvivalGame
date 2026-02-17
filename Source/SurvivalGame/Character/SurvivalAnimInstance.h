// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SurvivalAnimInstance.generated.h"


UCLASS()
class SURVIVALGAME_API USurvivalAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	
private:
	
	// Variables
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* PlayerCharacter;
	
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float Speed;
	
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;
	
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool BIsAccelerating;
};
