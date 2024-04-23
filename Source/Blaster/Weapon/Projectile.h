// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USoundCue;
class UBoxComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class UParticleSystemComponent;

UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(Server, Reliable)
	void ServerOnHit(FHitResult Hit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnHit(FHitResult Hit);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere, Category = Particles)
	UParticleSystem* HitCharacterParticles;

	UPROPERTY(EditAnywhere, Category = Particles)
	UParticleSystem* DefaultParticles;

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundCue* ImpactSound;
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionBox;
	
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileComponent;
	
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem;

	void SpawnTrailSystem();

	void StartDestroyedTimer();
	virtual void DestroyTimerFinished();
	void ExplodeDamage();
	
	UPROPERTY(VisibleAnywhere)
	class URocketMovementComponent* RocketMovementComponent;
	
	UPROPERTY(EditAnywhere, Category = "Projectile Properties")
	float DamageInnerRadius = 100.f;
	
	UPROPERTY(EditAnywhere, Category = "Projectile Properties")
	float DamageOuterRadius = 500.f;
private:
	
	UPROPERTY(EditAnywhere, Category = Particles)
	UParticleSystem* Tracer;

	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* TracerComponent;

	FTimerHandle DestroyTimer;
	
	UPROPERTY(EditAnywhere)
	float DestroyTrailTime = 0.01f;
};
