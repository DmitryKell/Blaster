// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"

#include "Kismet/GameplayStatics.h"

AProjectileRocket::AProjectileRocket()
{
	RocketComponent = CreateDefaultSubobject<UStaticMeshComponent>("RocketMesh");
	RocketComponent->SetupAttachment(RootComponent);
	RocketComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	//Get instigator pawn using SetOwnerInOther when fire in class WeaponProjectile:
	//"APawn* InstigatorPawn = Cast<APawn>(GetOwner());"
	APawn* FiringPawn = GetInstigator();
	
	if (FiringPawn)
	{
		AController* FiringController = FiringPawn->GetController();
		if (FiringController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(this,

			Damage, 10.f, GetActorLocation(),
				DamageInnerRadius, DamageOuterRadius, 1.f,
				UDamageType::StaticClass(), TArray<AActor*>(), this, FiringController);
		}
	}
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}
