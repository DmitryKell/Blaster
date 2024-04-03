// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileShotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileShotgun::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	
	while (CurrentProjectilesAmount != ProjectilesAmount)
	{
		SpawnProjectile(HitTarget);
		CurrentProjectilesAmount++;
		
	}
	if (CurrentProjectilesAmount == ProjectilesAmount)
	{
		CurrentProjectilesAmount = 0;
	}
}
 
void AProjectileShotgun::SpawnProjectile(const FVector& HitTarget)
{
	if (!HasAuthority()) return;
	
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));

	if (MuzzleFlashSocket && InstigatorPawn)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());

		//From MuzzleFlash socket to HitLocation From TraceUnderCrosshair

		// Calculate spread
		FRotator RandomRotation = FRotator(FMath::RandRange(-ScatterAmount, ScatterAmount), FMath::RandRange(-ScatterAmount, ScatterAmount), 0.0f);
		
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		
		// Apply the random rotation to the target rotation
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
