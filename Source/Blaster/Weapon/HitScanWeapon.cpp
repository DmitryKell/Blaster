// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();
	
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket && InstigatorController)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		FVector End = Start + (HitTarget - Start) * 1.25;

		FHitResult FireHit;
		UWorld* World = GetWorld();
		if (World)
		{
			World->LineTraceSingleByChannel(FireHit, Start, End, ECC_Visibility);
			if (FireHit.bBlockingHit)
			{
				ABlasterCharacter* Character = Cast<ABlasterCharacter>(FireHit.GetActor()); 
				//we cant use  this character for instigator controller because its damaged player 
				if (Character)
				{
					if (HasAuthority())
					{
						UGameplayStatics::ApplyDamage(Character, DamageAmount, InstigatorController, this, UDamageType::StaticClass());
					}
					if (DefaultParticles)
					{
						UGameplayStatics::SpawnEmitterAtLocation(World, DefaultParticles, End, FireHit.ImpactNormal.Rotation());
					}
				}
				else if (CharacterParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(World, CharacterParticles, End, FireHit.ImpactNormal.Rotation());
				}
			}
		}
	}
}
