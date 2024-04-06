// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileWeapon.h"
#include "Components/TimelineComponent.h"
#include "RecoilProjectileWeapon.generated.h"

/**
 * 
 */

UCLASS()
class BLASTER_API ARecoilProjectileWeapon : public AProjectileWeapon
{
	GENERATED_BODY()
public:
	ARecoilProjectileWeapon();
	virtual void BeginPlay() override;
	
	virtual void OnFireButtonPressed(bool bPressed) override;
	virtual void OnReload() override;
private:
#pragma region Recoil

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* TimelineComponent;
	
	UPROPERTY(EditAnywhere, Category = "Recoil")
	UCurveVector* VectorCurve;
	
	void InitTimeline();
	
	UFUNCTION()
	void UpdateVectorTimeline(FVector Vector);

	UPROPERTY(EditAnywhere, Category = "Recoil")
	float RecoilPitchStrength = 1.f;

	UPROPERTY(EditAnywhere, Category = "Recoil")
	float RecoilYawStrength = 1.f;
	
#pragma endregion 
};
