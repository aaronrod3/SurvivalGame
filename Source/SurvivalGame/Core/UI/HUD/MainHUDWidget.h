// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.generated.h"

class UInfoMessage;
/**
 * 
 */
UCLASS()
class SURVIVALGAME_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
	// Showing text when able to pickup item
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void ShowPickupMessage(const FString& Message);	
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void HidePickupMessage();
	
private:
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInfoMessage> InfoMessage;
	
	UFUNCTION()
	void OnNoRoom();
	
};
