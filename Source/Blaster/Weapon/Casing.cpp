// Fill out your copyright notice in the Description page of Project Settings.

#include "Casing.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	BulletCasing = CreateDefaultSubobject<UStaticMeshComponent>("BulletCasing");
	SetRootComponent(BulletCasing);
	BulletCasing->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	BulletCasing->SetSimulatePhysics(true);
	BulletCasing->SetEnableGravity(true);
	BulletCasing->SetNotifyRigidBodyCollision(true);
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();
	BulletCasing->OnComponentHit.AddDynamic(this, &ACasing::OnHit);

	float CasingImpulse = FMath::FRandRange(CasingImpulseMin, CasingImpulseMax);
	BulletCasing->AddImpulse(BulletCasing->GetForwardVector() * CasingImpulse);

	float RandRotation = FMath::FRandRange(0.f, 40.f);
	SetActorRotation(FRotator(RandRotation, GetActorRotation().Yaw, GetActorRotation().Roll));

	SetLifeSpan(3.f);

}

void ACasing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACasing::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}
	BulletCasing->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	BulletCasing->SetNotifyRigidBodyCollision(false);

}
