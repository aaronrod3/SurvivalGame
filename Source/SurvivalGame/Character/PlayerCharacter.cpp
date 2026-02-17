// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "EnhancedInputComponent.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	/*** CAMERA ***/

	// Create the spring arm and attach to root
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;          // Distance behind character
	SpringArmComp->SocketOffset = FVector(0.0f, 60.0f, 70.0f);
	SpringArmComp->bUsePawnControlRotation = true;    // Rotate arm based on controller

	// Create the camera and attach to spring arm end
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false; // Camera follows boom, not controller directly

	// Let the controller affect the character rotation (typical third‑person)
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	/*** HEALTH ***/ 
	
	// Initialize player's health
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	

	// Set animations on mesh
	if (ThirdPersonAnimBP)
	{
		GetMesh()->SetAnimInstanceClass(ThirdPersonAnimBP->GeneratedClass);
		
	}
	
	
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Now using Player Character."));
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Check the UInputComponent passed to this function and cast it to an UEnhancedInputComponent
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Bind Movement Actions
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		//Bind Look Actions
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		
		// Bind Jump Actions
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}
}


/*** Functions ***/

// Tell unreal which properties to replicate
void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Replicate current health
	DOREPLIFETIME(APlayerCharacter, CurrentHealth);
	
}

// health changes: debug messages for now; *** change this function when adding animations and response to health changes***
void APlayerCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void APlayerCharacter::OnHealthUpdate()
{
	// Client-specific functionality
	if (IsLocallyControlled())
	{
		const FString HealthMessage = FString::Printf(TEXT("Health: %f"), CurrentHealth);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, HealthMessage);
		}
		
		if (CurrentHealth <= 0.0f)
		{
			const FString DeathMessage = TEXT("You have been killed.");
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, DeathMessage);
			}
		}
	}
	
	// Server-specific functionality
	if (GetLocalRole() == ROLE_Authority)
	{
		const FString HealthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, HealthMessage);
		}
	}
	
}


/*** LOOK FUNCTIONS ***/

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

/*** Movement and look functions ***/

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// 2D Vector of movement values returned from the input action
	const FVector2D MovementValue = Value.Get<FVector2D>();

	/*** Test Movement***/
	if (Controller && (MovementValue.X != 0 || MovementValue.Y != 0))
	{
		const FRotator ControlRot = Controller->GetControlRotation();
		const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

		const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		const FVector RightDir   = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDir, MovementValue.Y);
		AddMovementInput(RightDir,   MovementValue.X);
	}
	

	
	/*** Movement where character turns to the direction of movement, don't want this but keeping here in case
	* 
	// Check if the controller possessing this Actor is valid
	if (Controller )
	{
		// Add left and right movement
		const FVector Right = GetActorRightVector();
		AddMovementInput(Right, MovementValue.X);

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Right: %f"), Right.X));
 
		// Add forward and back movement
		const FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, MovementValue.Y);

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Forward: %f"), Forward.Y));
	}
	***/
}




/*** HEALTH FUNCTIONS ***/

// Set Current Health
void APlayerCharacter::SetCurrentHealth(float HealthValue)
{
	// Only the server should modify replicated health
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(HealthValue, 0.0f, MaxHealth);
		OnHealthUpdate();
	}
}

// Take Damage
float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float DamageApplied = CurrentHealth - DamageAmount;
	SetCurrentHealth(DamageApplied);
	return DamageApplied;
}













