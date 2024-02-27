// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGHT 80000.f

class ABlasterHUD;
class ABlasterPlayerController;
class AWeapon;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UCombatComponent();
	friend class ABlasterCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void Reload();
	
	void EquipWeapon(AWeapon* WeaponToEquip);
	UFUNCTION()
	void OnRep_Weapon();
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(Server, Reliable)
	void ServerReload();
	
	void SetAiming(bool bIsAiming);
	void FireButtonPressed(bool bPressed);
	
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

private:
	UPROPERTY(Replicated)
	bool bAiming;
	bool bFireButtonPressed;
	bool bShrinkWhenAimingAtCharacter;

	UPROPERTY()
	ABlasterPlayerController* BlasterPlayerController;

	UPROPERTY()
	ABlasterHUD* BlasterHUD;

	UPROPERTY()
	ABlasterCharacter* Character;
	
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_Weapon)
	AWeapon* EquippedWeapon;
	
	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	FVector HitTarget;
	
	/*
	 * HUD and Crosshairs
	 */
	FHUDPackage HUDPackage;
	
	float CrosshairVelocityFactor;
	float CrosshairFallingFactor;
	float CrosshairAimFactor;
	float CrosshairShootFactor;
	float CrosshairAimAtAtCharacterFactor;
	// Aiming and FOV

	UPROPERTY(EditAnywhere, Category = Combat)
	float CurrentFOV;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float DefaultFOV;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedInterpSpeed = 20.f;
	
	void InterpFOV(float DeltaTime);

	/*
	 * Automatic Fire 
	 */
	FTimerHandle FireTimer;
	
	bool bCanFire = true;
	bool CanFire();
	void Fire();
	void FireTimerFinished();
	void StartFireTimer();

	// Carried Ammo for currently equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere, Category = StartingAmmo)
	int32 StartingAR_Ammo = 90;
	
	void InitializeCarriedAmmo();
	
	UFUNCTION()
	void OnRep_CarriedAmmo();
};
