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
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(Server, Reliable)
	void ServerOnHit(FHitResult Hit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnHit(FHitResult Hit);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category = Particles)
	UParticleSystem* Tracer;

	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* TracerComponent;

	UPROPERTY(EditAnywhere, Category = Particles)
	UParticleSystem* HitCharacterParticles;

	UPROPERTY(EditAnywhere, Category = Particles)
	UParticleSystem* WoodParticles;

	UPROPERTY(EditAnywhere, Category = Particles)
	UParticleSystem* StoneParticles;

	UPROPERTY(EditAnywhere, Category = Particles)
	UParticleSystem* GrassParticles;

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundCue* ImpactSound;
};
