// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Weapon/Weapon.h"
#include "TimerManager.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Camera/CameraComponent.h"
#include "Engine/Texture2D.h"
#include "Sound/SoundCue.h"
#include "Blaster/Weapon/Projectile.h"
#include "TimerManager.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		if (Character->GetCameraComponent())
		{
			DefaultFOV = Character->GetCameraComponent()->FieldOfView;
			CurrentFOV = DefaultFOV;
			
		}
		if (Character->HasAuthority())
		{
			InitializeCarriedAmmo();
		}
	}
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon)
	DOREPLIFETIME(UCombatComponent, bAiming)
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, CombatState)
	DOREPLIFETIME(UCombatComponent, Grenades);
}

void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingAR_Ammo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, StartingRocketLauncher_Ammo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Submachine, StartingSubMachine_Ammo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartingPistol_Ammo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, StartingShotgun_Ammo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SniperRifle, StartingSniperRifle_Ammo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_GrenadeLauncher, StartingGrenadeLauncher_Ammo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AR_SO, Starting_AR_SO);
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;
	
	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetWeaponFOV(), DeltaTime ,EquippedWeapon->GetInterpZoomSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime ,ZoomedInterpSpeed);
	}
	if (Character && Character->GetCameraComponent())
	{
		Character->GetCameraComponent()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr) return;
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : BlasterPlayerController;

	if (BlasterPlayerController)
	{
		BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(BlasterPlayerController->GetHUD()) : BlasterHUD;
		if (BlasterHUD )
		{
			
			if (EquippedWeapon)
			{
				HUDPackage.CrosshairCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairBottom = EquippedWeapon->CrosshairsBottom;
			}
			else
			{
				HUDPackage.CrosshairCenter = nullptr;
				HUDPackage.CrosshairLeft = nullptr;
				HUDPackage.CrosshairRight = nullptr;
				HUDPackage.CrosshairTop = nullptr;
				HUDPackage.CrosshairBottom = nullptr;
			}
			// Calculate crosshair spread
			// [0 - 600] = [0 - 1]
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;
			
			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			
			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairFallingFactor = FMath::FInterpTo(CrosshairFallingFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairFallingFactor = FMath::FInterpTo(CrosshairFallingFactor, 0.f, DeltaTime, 20.f);
			}
			
			if (bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0, DeltaTime, 30.f);
			}
			
			if (bShrinkWhenAimingAtCharacter && !bAiming)
			{
				CrosshairAimAtAtCharacterFactor = FMath::FInterpTo(CrosshairAimAtAtCharacterFactor, 0.5f, DeltaTime, 30.f );
			}
			
			else if (!bShrinkWhenAimingAtCharacter || bAiming)
			{
				CrosshairAimAtAtCharacterFactor = FMath::FInterpTo(CrosshairAimAtAtCharacterFactor, 0.f, DeltaTime, 30.f );
			}
			
			CrosshairShootFactor = FMath::FInterpTo(CrosshairShootFactor, 0.f, DeltaTime, 20.f);
			
			HUDPackage.CrosshairSpread = 0.5f -CrosshairAimAtAtCharacterFactor + CrosshairVelocityFactor + CrosshairFallingFactor - CrosshairAimFactor + CrosshairShootFactor;
			BlasterHUD->SetHUDPackage(HUDPackage);
		}
	}
	
}



void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
	UGameplayStatics::GetPlayerController(this, 0),
	CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);
	
	//Calculate center of the screen ↑
	
	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		
		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
		}
		
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGHT;
		
		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECC_Visibility);
		if (!TraceHitResult.bBlockingHit)
		{
			HitTarget = End;
		}
		else
		{
			HitTarget = TraceHitResult.ImpactPoint;
		}
		
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			HUDPackage.CrosshairsColor = FColor::Red;
			bShrinkWhenAimingAtCharacter = true;
		}
		else
		{
			HUDPackage.CrosshairsColor = FColor::White;
			bShrinkWhenAimingAtCharacter = false;
		}
	}
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (!Character || !WeaponToEquip) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	
	DropEquippedWeapon();
	
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	
	AttachActorToRightHand(EquippedWeapon);
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->UpdateWeaponAmmoHUD();

	UpdateCarriedAmmo();
	// on server
	PlayEquipWeaponSound();
	ReloadEmptyWeapon();
	
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::DropEquippedWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}
}

void UCombatComponent::AttachActorToRightHand(AActor* ActorToAttach)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || ActorToAttach == nullptr) return;
	
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
}

void UCombatComponent::AttachActorToLeftHand(AActor* ActorToAttach)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || ActorToAttach == nullptr || EquippedWeapon == nullptr) return;
	
	bool bUsePistolSocket = EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol ||
		EquippedWeapon->GetWeaponType() ==  EWeaponType::EWT_Submachine;
	
	FName SocketName = bUsePistolSocket ? FName("LeftHandPistolSocket") : FName("LeftHandPistolSocket");

	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(SocketName);
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
}
void UCombatComponent::ShowAttachedGrenade(bool bShow)
{
	if (Character && Character->GetGrenadeComponent())
	{
		Character->GetGrenadeComponent()->SetVisibility(bShow);
	}
}
void UCombatComponent::UpdateCarriedAmmo()
{
	if (EquippedWeapon == nullptr ) return;
	
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : BlasterPlayerController;
	if (BlasterPlayerController && EquippedWeapon)
	{
		BlasterPlayerController->SetHUDCarriedAmmo(CarriedAmmo);
		
		// Sets on Server
		FString WeaponTypeText = GetNameOfWeaponType(EquippedWeapon->GetWeaponType());
		BlasterPlayerController->SetWeaponTypeText(WeaponTypeText);
	}
}

void UCombatComponent::PlayEquipWeaponSound()
{
	if (Character && EquippedWeapon && EquippedWeapon->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), EquippedWeapon->EquipSound, Character->GetActorLocation());
	}
}

void UCombatComponent::ReloadEmptyWeapon()
{
	if (EquippedWeapon && EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}

// replicate to clients
void UCombatComponent::OnRep_Weapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	
		AttachActorToRightHand(EquippedWeapon);
		PlayEquipWeaponSound();
		
		// sets on clients
		if (BlasterPlayerController)
		{
			FString WeaponTypeText = GetNameOfWeaponType(EquippedWeapon->GetWeaponType());
			BlasterPlayerController->SetWeaponTypeText(WeaponTypeText);
		}
		
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::LaunchGrenade()
{
	ShowAttachedGrenade(false);
	if (Character && Character->IsLocallyControlled())
	{
		ServerLaunchGrenade(HitTarget);
	}
}


void UCombatComponent::SetAiming(bool bIsAiming)
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	
	bAiming = bIsAiming;
	//bShrinkWhenAimingAtCharacter = false;
	ServerSetAiming(bIsAiming);
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
	
	if (Character->IsLocallyControlled() && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle)
	{
		Character->ShowSniperScopeWidget(bIsAiming);
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}


void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if (bFireButtonPressed)
	{
		Fire();
	}
	if (EquippedWeapon == nullptr || Character == nullptr) return;
	EquippedWeapon->OnFireButtonPressed(bFireButtonPressed);
}

void UCombatComponent::Fire()
{
	if (CanFire())
	{
		bCanFire = false;
		ServerFire(HitTarget);
		CrosshairShootFactor = EquippedWeapon->GetCrosshairShootFactor();
		StartFireTimer();
	}
}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr || Character == nullptr) return;
	
	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UCombatComponent::FireTimerFinished,
		EquippedWeapon->GetFireFrequency()
	);
}

void UCombatComponent::FireTimerFinished()
{
	if (EquippedWeapon == nullptr) return;
	bCanFire = true;
	if (bFireButtonPressed && EquippedWeapon->GetbAutomatic())
	{
		Fire();
	}
	ReloadEmptyWeapon();
}


void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;
	if (Character)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr) return false;
	return !EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::Reload()
{
	if (CarriedAmmo > 0 && CombatState == ECombatState::ECS_Unoccupied
		&& EquippedWeapon && EquippedWeapon->GetAmmo() != EquippedWeapon->GetMagCapacity())
	{
		ServerReload();
	}
}
void UCombatComponent::HandleReload()
{
	Character->PlayReloadMontage();
	EquippedWeapon->OnReload();
}

void UCombatComponent::ThrowGrenade()
{
	if (Grenades == 0) return;
	if (CombatState != ECombatState::ECS_Unoccupied || EquippedWeapon == nullptr) return;
	
	CombatState = ECombatState::ECS_ThrowingGrenade;
	if (Character)
	{
		Character->PlayThrowGrenadeMontage();
		AttachActorToLeftHand(EquippedWeapon);
		ShowAttachedGrenade(true);
	}
	
	if (Character && !Character->HasAuthority())
	{
		ServerThrowGrenade();
	}
	if (Character && Character->HasAuthority())
	{
		Grenades = FMath::Clamp(Grenades - 1, 0, MaxGrenades);
		UpdateHUDGrenades();
	}
}

void UCombatComponent::OnRep_Grenades()
{
	UpdateHUDGrenades();
}

void UCombatComponent::UpdateHUDGrenades()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDGrenades(Grenades);
	}
}

void UCombatComponent::ServerLaunchGrenade_Implementation(const FVector_NetQuantize& Target)
{
	if (Character  && GrenadeClass && Character->GetGrenadeComponent())
	{
		const FVector StartingLocation = Character->GetGrenadeComponent()->GetComponentLocation();
		FVector ToTarget = Target - StartingLocation;
		
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = Character;
		SpawnParameters.Instigator = Character;
		
		UWorld* World = GetWorld();
		if (World)
		{
			World->SpawnActor<AProjectile>(GrenadeClass, StartingLocation, ToTarget.Rotation(), SpawnParameters);
		}
	}
}


void UCombatComponent::ServerThrowGrenade_Implementation()
{
	if (Grenades == 0) return;
	CombatState = ECombatState::ECS_ThrowingGrenade;
	if (Character)
	{
		Character->PlayThrowGrenadeMontage();
		AttachActorToLeftHand(EquippedWeapon);
		ShowAttachedGrenade(true);
	}
	Grenades = FMath::Clamp(Grenades - 1, 0, MaxGrenades);
	UpdateHUDGrenades();
}

void UCombatComponent::ServerReload_Implementation()
{
	if (Character == nullptr && EquippedWeapon == nullptr) return;
	// Called on Server
	CombatState = ECombatState::ECS_Reloading;

	HandleReload();
}


void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading:
		// Called on Clients
		HandleReload();
		break;
	case ECombatState::ECS_Unoccupied:
		if (bFireButtonPressed)
		{
			// Called on Clients
			Fire();
		}
		break;
	case ECombatState::ECS_ThrowingGrenade:
		if (Character && !Character->IsLocallyControlled())
		{
			Character->PlayThrowGrenadeMontage();
			AttachActorToLeftHand(EquippedWeapon);
			ShowAttachedGrenade(true);
		}
		break;
	}
}


void UCombatComponent::FinishReloading()
{
	if (Character == nullptr) return;
	
	if ( Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
	}
	if (bFireButtonPressed)
	{
		// called on server
		Fire();
	}
}

void UCombatComponent::FinishThrowingGrenade()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (EquippedWeapon)
	{
		AttachActorToRightHand(EquippedWeapon);
		//ShowAttachedGrenade(true);
	}
}


int32 UCombatComponent::AmountToReload()
{
	if (EquippedWeapon == nullptr) return 0;
	int32 RoomInMag;
	RoomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(RoomInMag, AmountCarried);
		return FMath::Clamp(RoomInMag, 0, Least);
	}
	return 0;
}

void UCombatComponent::UpdateAmmoValues()
{
	if (Character == nullptr && EquippedWeapon == nullptr) return;
	
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDCarriedAmmo(CarriedAmmo);
	}
	
	EquippedWeapon->AddAmmo(-ReloadAmount);
}

FString UCombatComponent::GetNameOfWeaponType(EWeaponType WeaponType)
{
	FString Text;
	switch (WeaponType)
	{
	case EWeaponType::EWT_AssaultRifle:
		Text = "Assault Rifle";
		return Text;
	case EWeaponType::EWT_RocketLauncher:
		Text = "Rocket Launcher";
		return Text;
	case EWeaponType::EWT_Pistol:
		Text = "Pistol";
		return Text;
	case EWeaponType::EWT_Submachine:
		Text = "Submachine";
		return Text;
	case EWeaponType::EWT_Shotgun:
		Text = "Shotgun";
		return Text;
	case EWeaponType::EWT_SniperRifle:
		Text = "Sniper Rifle";
		return Text;
	case EWeaponType::EWT_GrenadeLauncher:
		Text = "Grenade Launcher";
		return Text;
	case EWeaponType::EWT_AR_SO:
		Text = "AR_SO2";
		return Text;
	}
	return "";
}



