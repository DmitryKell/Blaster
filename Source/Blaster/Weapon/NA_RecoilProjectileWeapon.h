// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "NA_RecoilProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ANA_RecoilProjectileWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	virtual void Fire(const FVector& HitTarget) override;

private:
	UPROPERTY(EditAnywhere, Category = WeaponProperties)
	TSubclassOf<class AProjectile> Projectile;
	
	UPROPERTY(EditAnywhere, Category = WeaponProperties)
	float ScatterAmount = 1;

	UPROPERTY(EditAnywhere, Category = Recoil)
	float RecoilPitchStrength = -1;
	
	UPROPERTY(EditAnywhere, Category = Recoil)
	float RecoilYawStrength = 1;

	void SimulateRecoil(APawn* InstigatorPawn);
};
