// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileWeapon.h"
#include "Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	
	InstigatorPawn = Cast<APawn>(GetOwner());
	
	if (!HasAuthority()) return;
	
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));

	if (MuzzleFlashSocket && InstigatorPawn)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		
		FRotator RandomRotation = FRotator(FMath::RandRange(-ScatterAmount, ScatterAmount), FMath::RandRange(-ScatterAmount, ScatterAmount), 0.0f);
		
		//From MuzzleFlash socket to HitLocation From TraceUnderCrosshair

		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		
		// Apply the amount of rotation to the target rotation
		FRotator TargetRotation = (ToTarget.Rotation() + RandomRotation);
		
		if (Projectile)
		{
			FActorSpawnParameters SpawnParameters;

			SpawnParameters.Owner = GetOwner();
			SpawnParameters.Instigator = InstigatorPawn;

			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<AProjectile>(Projectile, SocketTransform.GetLocation(), TargetRotation, SpawnParameters);
			}
		}
	}
}
