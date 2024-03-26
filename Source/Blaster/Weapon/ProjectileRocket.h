// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectileRocket();
protected:
	
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void BeginPlay() override;

	
	virtual void ServerOnHit_Implementation(FHitResult Hit) override;
	virtual void MulticastOnHit_Implementation(FHitResult Hit) override;

	void DestroyTimerFinished();

	UPROPERTY(EditAnywhere)
	USoundCue* ProjectileLoop;

	UPROPERTY()
	class UAudioComponent* ProjectileLoopComponent;

	UPROPERTY(EditAnywhere)
	class USoundAttenuation* LoopingSoundAttenuation;
	
private:
	UPROPERTY(VisibleAnywhere)
	class URocketMovementComponent* RocketMovementComponent;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketComponent;
	
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem;
	
	UPROPERTY(EditAnywhere, Category = "Projectile Properties")
	float DamageInnerRadius = 100.f;
	
	UPROPERTY(EditAnywhere, Category = "Projectile Properties")
	float DamageOuterRadius = 500.f;

	FTimerHandle DestroyTimer;
	
	UPROPERTY(EditAnywhere)
	float DestroyTrailTime = 3.f;
};
