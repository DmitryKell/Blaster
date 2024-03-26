// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"
#include "NiagaraFunctionLibrary.h"
#include "RocketMovementComponent.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AProjectileRocket::AProjectileRocket()
{
	RocketComponent = CreateDefaultSubobject<UStaticMeshComponent>("RocketMesh");
	RocketComponent->SetupAttachment(RootComponent);
	RocketComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>("ProjectileRocketMovementComponent");
	RocketMovementComponent->bRotationFollowsVelocity = true;
	RocketMovementComponent->SetIsReplicated(true);
}

void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();
	if (TrailSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(TrailSystem, GetRootComponent(), FName(""),
			GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
	}
	
	if (ProjectileLoop && LoopingSoundAttenuation)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(ProjectileLoop, GetRootComponent(),
			FName(), GetActorLocation(),
			EAttachLocation::KeepWorldPosition,
			false, 1.f, 1.f, 0.f,  LoopingSoundAttenuation, (USoundConcurrency*)nullptr, false);
	}

}


void AProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	//Get instigator pawn using SetOwner when fire in class WeaponProjectile:
	//"APawn* InstigatorPawn = Cast<APawn>(GetOwner());"
	if (OtherActor == GetOwner())
	{
		return;
	}
	APawn* FiringPawn = GetInstigator();
	
	if (FiringPawn && HasAuthority())
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
	
	ServerOnHit(Hit);
	GetWorld()->GetTimerManager().SetTimer(DestroyTimer, this, &AProjectileRocket::DestroyTimerFinished, DestroyTrailTime);

	if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
	{
		ProjectileLoopComponent->Stop();
	}
}

void AProjectileRocket::ServerOnHit_Implementation(FHitResult Hit)
{
	Super::ServerOnHit_Implementation(Hit);
}

void AProjectileRocket::MulticastOnHit_Implementation(FHitResult Hit)
{
	if (Hit.GetActor() && Hit.GetActor()->Implements<UInteractWithCrosshairsInterface>())
	{
		if (HitCharacterParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitCharacterParticles, GetActorLocation(), GetActorRotation(), FVector(0.5f));
		}
	}
	else
	{
		if (DefaultParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DefaultParticles, GetActorLocation());
		}
	}
	
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (RocketComponent)
	{
		RocketComponent->DestroyComponent();
	}
	
	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AProjectileRocket::DestroyTimerFinished()
{
	Destroy();
}


