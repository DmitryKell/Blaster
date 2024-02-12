// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

class UProjectileMovementComponent;

UCLASS()
class BLASTER_API ACasing : public AActor
{
	GENERATED_BODY()

public:
	ACasing();
	virtual void Tick(float DeltaTime) override;


protected:
	virtual void BeginPlay() override;
	//virtual void Destroyed() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BulletCasing;

	UPROPERTY(EditAnywhere)
	class USoundCue* ShellSound;

	UPROPERTY(EditAnywhere)
	float CasingImpulseMin = 200.f;
	UPROPERTY(EditAnywhere)
	float CasingImpulseMax = 200.f;
};
