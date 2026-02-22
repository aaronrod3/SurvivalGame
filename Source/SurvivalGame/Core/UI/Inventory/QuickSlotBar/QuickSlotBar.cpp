// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickSlotBar.h"
#include "QuickSlotEntry.h"
#include "SurvivalGame/Core/UI/Inventory/Spatial/SpatialInventory.h"
#include "SurvivalGame/Core/UI/Inventory/DragItem/DragItem.h"
#include "SurvivalGame/Core/Data/Items/InventoryItem.h"
#include "SurvivalGame/Core/Data/Items/Fragments/ItemFragment.h"
#include "SurvivalGame/Core/Data/Items/Fragments/FragmentTags.h"

void UQuickSlotBar::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	// Stamp each entry with its slot type so it can pass it back drop
	if (QuickSlot_Weapon_Primary)  QuickSlot_Weapon_Primary->SetSlotType(EQuickSlotType::Weapon_Primary);
	if (QuickSlot_Weapon_Holster)  QuickSlot_Weapon_Holster->SetSlotType(EQuickSlotType::Weapon_Holster);
	if (QuickSlot_Weapon_Secondary)QuickSlot_Weapon_Secondary->SetSlotType(EQuickSlotType::Weapon_Secondary);
	if (QuickSlot_Tool)            QuickSlot_Tool->SetSlotType(EQuickSlotType::Tool);
	if (QuickSlot_5)               QuickSlot_5->SetSlotType(EQuickSlotType::Slot_5);
	if (QuickSlot_6)               QuickSlot_6->SetSlotType(EQuickSlotType::Slot_6);
	if (QuickSlot_7)               QuickSlot_7->SetSlotType(EQuickSlotType::Slot_7);
	if (QuickSlot_8)               QuickSlot_8->SetSlotType(EQuickSlotType::Slot_8);
	if (QuickSlot_9)               QuickSlot_9->SetSlotType(EQuickSlotType::Slot_9);
	if (QuickSlot_0)               QuickSlot_0->SetSlotType(EQuickSlotType::Slot_0);
}

void UQuickSlotBar::SetSpatialInventory(USpatialInventory* Inventory)
{
	SpatialInventory = Inventory;
	
	// Wire each entry's drop delegate to SpatialInventory
	auto BindEntry = [this](UQuickSlotEntry* Entry)
	{
		if (!Entry) return;
		Entry->OnDropped.BindLambda([this](EQuickSlotType SlotType, UDragDropOperation* Operation)
		{
			if (!SpatialInventory.IsValid()) return;
			
			UDragItem* DragOp = Cast<UDragItem>(Operation);
			if (!DragOp || !DragOp->Item || !DragOp->SourceGrid) return;
			
			SpatialInventory->TryAssignConsumableQuickSlot(
				DragOp->Item,
				DragOp->SourceGrid,
				DragOp->SourceIndex,
				SlotType);
		});
	};
	
	BindEntry(QuickSlot_Weapon_Primary);
	BindEntry(QuickSlot_Weapon_Holster);
	BindEntry(QuickSlot_Weapon_Secondary);
	BindEntry(QuickSlot_Tool);
	BindEntry(QuickSlot_5);
	BindEntry(QuickSlot_6);
	BindEntry(QuickSlot_7);
	BindEntry(QuickSlot_8);
	BindEntry(QuickSlot_9);
	BindEntry(QuickSlot_0);
}

void UQuickSlotBar::OnQuickSlotUpdated(EQuickSlotType SlotType, UInventoryItem* Item)
{
	UQuickSlotEntry* Entry = GetQuickSlotEntry(SlotType);
	if (!Entry) return;
	
	if (!IsValid(Item))
	{
		Entry->Clear();
		return;
	}
	
	AssignItemToSlot(Entry, Item);
}

void UQuickSlotBar::AssignItemToSlot(UQuickSlotEntry* Entry, UInventoryItem* Item) const
{
	const FGridFragment* GridFragment = GetFragment<FGridFragment>(Item, FragmentTags::GridFragment);
	const FImageFragment* ImageFragment = GetFragment<FImageFragment>(Item, FragmentTags::IconFragment);
	if (!GridFragment || !ImageFragment) return;
	
	const float IconTileWidth = SlotSize - (GridFragment->GetGridPadding() * 2.f);
	const FVector2D DrawSize  = FVector2D(GridFragment->GetGridSize()) * IconTileWidth;

	FSlateBrush IconBrush;
	IconBrush.SetResourceObject(ImageFragment->GetIcon());
	IconBrush.DrawAs    = ESlateBrushDrawType::Image;
	IconBrush.ImageSize = DrawSize;

	Entry->SetImageBrush(IconBrush);
	Entry->SetInventoryItem(Item);
}

UQuickSlotEntry* UQuickSlotBar::GetQuickSlotEntry(EQuickSlotType SlotType) const
{
	switch (SlotType)
	{
	case EQuickSlotType::Weapon_Primary:   return QuickSlot_Weapon_Primary;
	case EQuickSlotType::Weapon_Holster:   return QuickSlot_Weapon_Holster;
	case EQuickSlotType::Weapon_Secondary: return QuickSlot_Weapon_Secondary;
	case EQuickSlotType::Tool:             return QuickSlot_Tool;
	case EQuickSlotType::Slot_5:           return QuickSlot_5;
	case EQuickSlotType::Slot_6:           return QuickSlot_6;
	case EQuickSlotType::Slot_7:           return QuickSlot_7;
	case EQuickSlotType::Slot_8:           return QuickSlot_8;
	case EQuickSlotType::Slot_9:           return QuickSlot_9;
	case EQuickSlotType::Slot_0:           return QuickSlot_0;
	default:                               return nullptr;
	}
}










