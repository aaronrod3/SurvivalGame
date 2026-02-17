// Fill out your copyright notice in the Description page of Project Settings.


#include "InfoMessage.h"

#include "Components/TextBlock.h"


void UInfoMessage::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Text_Message->SetText(FText::GetEmpty());
	MessageHide();
}


void UInfoMessage::SetMessage(const FText& Message)
{
	Text_Message->SetText(Message);
	
	if (!bIsMessageActive)
	{
		
		MessageShow();
	}
	bIsMessageActive = true;
	
	GetWorld()->GetTimerManager().SetTimer(MessageTimerHandle, [this]()
	{
		MessageHide();
		bIsMessageActive = false;
	}, MessageLifetime, false);
}
