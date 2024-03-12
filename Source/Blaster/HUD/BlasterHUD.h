// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"


class UCharacterOverlay;

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	
	class UTexture2D* CrosshairCenter;
	UTexture2D* CrosshairLeft;
	UTexture2D* CrosshairRight;
	UTexture2D* CrosshairTop;
	UTexture2D* CrosshairBottom;
	float CrosshairSpread;
	FLinearColor CrosshairsColor;
	
};

UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;
	
	// Character Overlay
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;
	
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	
	void AddCharacterOverlay();

	// Announcement Overlay
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> AnnouncementOverlayClass;
	
	UPROPERTY()
	class UAnnouncement* AnnouncementOverlay;
	
	void AddAnnouncementOverlay();
	
protected:
	virtual void BeginPlay() override;
	
private:

	FHUDPackage HUDPackage;
	void DrawCrosshair(UTexture2D* ToTexture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairsColor);
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& HUDPackageToSet) {HUDPackage = HUDPackageToSet;}
};
