// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h" // Replication
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"


class UAnimBlueprint;
class UInputMappingContext;
class UInputAction;

UCLASS()
class SURVIVALGAME_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();
	
	// Default Third Person Animation Blueprint
	UPROPERTY(EditAnywhere, Category = Animations)
	UAnimBlueprint* ThirdPersonAnimBP;
	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
	// Property replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	
	/*** HEALTH ***/ 
	
	// Getter for Max Health
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	
	// Getter for Current Health
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	
	/* Setter for Current Health */
	/* Clamps the value between 0 and MaxHeatlh */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetCurrentHealth(float HealthValue);
	
	// Event for taking damage, overridden in .cpp
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;
	
	/*** CAMERA ***/
	FORCEINLINE USpringArmComponent* GetSpringArmComp() const { return SpringArmComp; }
	FORCEINLINE UCameraComponent* GetCameraComp() const { return CameraComp; }
	
private:
	
	/*** Camera ***/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComp;
	
	
	/*** MOVEMENT ***/

	// Move
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	// Look
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
 
	// Jump 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;
	
	// Use 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> UseAction;
	
	
	// Handles 2D Movement Input
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	// Look input
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	
	
	
	
	/*** Health ***/
	
	// Maximum health value for this character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta=(AllowPrivateAccess = "true"))
	float MaxHealth;
	
	// Current health, replicated to clients
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;
	
	// RepNotify for changes to current health
	UFUNCTION()
	void OnRep_CurrentHealth();
	
	// Response to health being updated
	void OnHealthUpdate();

protected:
	virtual void BeginPlay() override;


	
	

	
	
	
};














