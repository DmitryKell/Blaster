// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"


UCLASS()
class BLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
protected:
	virtual void Fire(const FVector& HitTarget) override;
private:
	
	UPROPERTY(EditAnywhere)
	float DamageAmount = 20.f;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* DefaultParticles;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* CharacterParticles;
};
