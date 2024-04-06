// Fill out your copyright notice in the Description page of Project Settings.

#include "NA_RecoilProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"
#include "Kismet/KismetMathLibrary.h"

void ANA_RecoilProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (!HasAuthority()) return;

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());

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
		SimulateRecoil(InstigatorPawn);
	}
}

void ANA_RecoilProjectileWeapon::SimulateRecoil(APawn* InstigatorPawn)
{
	if (InstigatorPawn == nullptr) return;
	
	float Pitch = UKismetMathLibrary::RandomFloatInRange(0.9, 1);
	float Yaw = UKismetMathLibrary::RandomFloatInRange(-1, 1);

	if (InstigatorPawn)
	{
		InstigatorPawn->AddControllerPitchInput(Pitch * RecoilPitchStrength);
		InstigatorPawn->AddControllerPitchInput(Yaw * RecoilYawStrength);
	}
}
