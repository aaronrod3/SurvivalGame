// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalPlayerController.h"
#include "SurvivalGame/Character/Inventory/InventoryComponent.h"
#include "SurvivalGame/Core/UI/HUD/MainHUDWidget.h"
#include "SurvivalGame/Framework/SurvivalGameGameInstance.h"
#include "SurvivalGame/Framework/Subsystems/SurvivalGameOnlineSubsystem.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "SurvivalGame/Core/Data/Items/ItemComponent.h"


// remove

ASurvivalPlayerController::ASurvivalPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// initialize tracelength
	TraceLength = 500.0f;
	
	
}

void ASurvivalPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TraceForItem();
}

void ASurvivalPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// Temp inventory log
	UE_LOG(LogInventory, Log, TEXT("Begin Play for Player Controller"));
	
	
	// MOVED HERE
	// Input mapping now lives here
	if (IsLocalController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (const TObjectPtr<UInputMappingContext>& Context : InputMappingContexts)
			{
				Subsystem->AddMappingContext(Context, 0);
			}
			UE_LOG(LogInventory, Log, TEXT("Added Input Mapping Contexts to Player Controller"));
		}
	}
	
	
	
	// Create UI
	CreateHUDWidget();
	
	// Create Inventory
	InventoryComponent = FindComponentByClass<UInventoryComponent>();
	
	
	/*** Online Services ***/
	
	// Register player with Online Services
	// Get GameInstance
	USurvivalGameGameInstance* GameInstance = Cast<USurvivalGameGameInstance>(GetWorld()->GetGameInstance());
	
	if (!GameInstance)
	{
		UE_LOG(LogSurvivalGameGameInstance, Error, TEXT("Game Instance is not USurvivalGameGameInstance. "));
		return;
	}
	
	
	// Get Game Instance Subsystem
	USurvivalGameOnlineSubsystem* OnlineSubsystem = GameInstance->GetSubsystem<USurvivalGameOnlineSubsystem>();
	// Get the local player for this controller
	ULocalPlayer* LocalPlayer = Super::GetLocalPlayer();
	
	
	if (LocalPlayer && OnlineSubsystem)
	{
		FPlatformUserId LocalPlayerPlatformUserId = LocalPlayer->GetPlatformUserId();
		UE_LOG(LogSurvivalGameGameInstance, Log, TEXT("Registering PlatformUserId: %d"), LocalPlayerPlatformUserId.GetInternalId());
		
		// 1) Register player with Online Services
		OnlineSubsystem->RegisterLocalOnlineUser(LocalPlayerPlatformUserId);
		
		// 2) Read the title file and display it on screen
		const FString TitleFileContents = OnlineSubsystem->ReadTitleFile(TEXT("StatusFile"), LocalPlayerPlatformUserId);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, TitleFileContents);
		}
	}
	else
	{
		UE_LOG(LogSurvivalGameGameInstance, Error, TEXT("Local Player or Online Subsystem invalid in BeginPlay."));
	}
}

void ASurvivalPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// MOVED HERE
	// Check the UInputComponent passed to this function and cast it to an UEnhancedInputComponent
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Interact Actions
		EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Triggered, this, &ASurvivalPlayerController::PrimaryInteract);
		
		// Inventory Action
		EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Triggered, this, &ASurvivalPlayerController::ToggleInventory);
	} 
}


void ASurvivalPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


/***  INVENTORY FUNCTIONS ***/

void ASurvivalPlayerController::ToggleInventory()
{
	if (!InventoryComponent.IsValid()) return;
	InventoryComponent->ToggleInventoryMenu();
}


/*** HUD and UI ***/
void ASurvivalPlayerController::CreateHUDWidget()
{
	if (!IsLocalController()) return;
	HUDWidget = CreateWidget<UMainHUDWidget>(this, HUDWidgetClass);
	if (IsValid(HUDWidget))
	{
		HUDWidget->AddToViewport();
	}
}


// Function to trace for an item
void ASurvivalPlayerController::TraceForItem()
{
	// check if GEngine and GameViewport is valid
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport)) return ;
	
	// create variable for viewportsize, get viewportsize
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	
	// find center of viewport
	const FVector2D ViewportCenter = ViewportSize / 2.f;
	
	// get tracestart and forward direction for GameplayStatics
	FVector TraceStart;
	FVector Forward;
	if (!UGameplayStatics::DeprojectScreenToWorld(this, ViewportCenter, TraceStart, Forward)) return;
	
	// calculate trace end and get hit result
	const FVector TraceEnd = TraceStart + Forward * TraceLength;
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, TraceChannel);
	
	// variables for making sure it traces item and then resets when not tracing item
	LastActor = ThisActor;
	ThisActor = HitResult.GetActor();
	
	if (!ThisActor.IsValid())
	{
		if (IsValid(HUDWidget))
		{
			HUDWidget->HidePickupMessage();
		}
	}
	
	if (ThisActor == LastActor) return;
	
	if (ThisActor.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("Hit New Actor"));
		UItemComponent* ItemComponent = ThisActor->FindComponentByClass<UItemComponent>();
		if (!IsValid(ItemComponent)) return;
		
		if (IsValid(HUDWidget))
		{
			HUDWidget->ShowPickupMessage(ItemComponent->GetPickupMessage());
		}
		
		
	}
	
	if (LastActor.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("Stop tracing last actor"));
	}
}


void ASurvivalPlayerController::PrimaryInteract()
{
	if (!ThisActor.IsValid()) return;
	
	UItemComponent* ItemComponent = ThisActor->FindComponentByClass<UItemComponent>();
	if (!IsValid(ItemComponent) || !InventoryComponent.IsValid()) return;
	
	InventoryComponent->TryAddItem(ItemComponent);
}







