// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileShotgun.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AProjectileShotgun : public AWeapon
{
	GENERATED_BODY()
protected:
	
	virtual void Fire(const FVector& HitTarget) override;
	void SpawnProjectile(const FVector& HitTarget);
private:
	
	UPROPERTY(EditAnywhere, Category = WeaponProperties)
	TSubclassOf<class AProjectile> Projectile;
	
	int CurrentProjectilesAmount = 0;
	
	UPROPERTY(EditAnywhere, Category = WeaponProperties)
	int ProjectilesAmount = 5;
	
	UPROPERTY(EditAnywhere, Category = WeaponProperties)
	float ScatterAmount = 1.f;
};
