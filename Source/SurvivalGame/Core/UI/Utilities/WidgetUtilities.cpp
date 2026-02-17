// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetUtilities.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/Widget.h"


FVector2D UWidgetUtilities::GetWidgetPosition(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();
	FVector2D PixelPosition;
	FVector2D ViewportPosition;
	USlateBlueprintLibrary::LocalToViewport(Widget, Geometry, USlateBlueprintLibrary::GetLocalTopLeft(Geometry), PixelPosition, ViewportPosition);
	
	return ViewportPosition;
}

FVector2D UWidgetUtilities::GetWidgetSize(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();
	return Geometry.GetLocalSize();
}

bool UWidgetUtilities::IsWithinBounds(const FVector2D& BoundaryPos, const FVector2D& WidgetSize,
                                      const FVector2D& MousePos)
{
	return MousePos.X >= BoundaryPos.X && MousePos.X <= (BoundaryPos.X + WidgetSize.X) && MousePos.Y >= BoundaryPos.Y && MousePos.Y <= (BoundaryPos.Y + WidgetSize.Y);
}


int32 UWidgetUtilities::GetIndexFromPosition(const FIntPoint& Position, const int32 Columns)
{
	return Position.X + (Position.Y * Columns);
}


FIntPoint UWidgetUtilities::GetPositionFromIndex(const int32 Index, const int32 Columns)
{
	return FIntPoint(Index % Columns, Index / Columns);
}