// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterHUD.h"
#include "CharacterOverlay.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Announcement.h"



void ABlasterHUD::BeginPlay()
{
	Super::BeginPlay();

}

void ABlasterHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void ABlasterHUD::AddAnnouncementOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && AnnouncementOverlayClass)
	{
		AnnouncementOverlay = CreateWidget<UAnnouncement>(PlayerController, AnnouncementOverlayClass);
		AnnouncementOverlay->AddToViewport();
	}
}

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();
	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D CenterOfTheScreen =  FVector2D(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
		
		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;
		
		if (HUDPackage.CrosshairCenter)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CrosshairCenter, CenterOfTheScreen,Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairLeft)
		{
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairLeft, CenterOfTheScreen, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairRight)
		{
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairRight, CenterOfTheScreen, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairTop)
		{
			FVector2D Spread( 0.f, -SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairTop, CenterOfTheScreen, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairBottom)
		{
			FVector2D Spread( 0.f, SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairBottom, CenterOfTheScreen, Spread, HUDPackage.CrosshairsColor);
		}
	}
}


void ABlasterHUD::DrawCrosshair(UTexture2D* ToTexture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairsColor)
{
	const float TextureWidth = ToTexture->GetSizeX();
	const float TextureHeight = ToTexture->GetSizeY();
	const FVector2D TextureDrawPoint(ViewportCenter.X - (TextureWidth / 2 ) + Spread.X, ViewportCenter.Y - (TextureHeight / 2) + Spread.Y);
	DrawTexture(ToTexture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f, 1.f, CrosshairsColor);
}

