// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Blaster/TurningInPlace.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include  "Components/TimelineComponent.h"
#include "BlasterCharacter.generated.h"

enum class ECombatState : uint8;
class ABlasterPlayerState;
class USoundCue;
class UTimelineComponent;
class ABlasterPlayerController;
class UCombatComponent;
class AWeapon;
class UWidgetComponent;
class UCameraComponent;
class UAnimMontage;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void OnRep_ReplicatedMovement() override;
	virtual void Destroyed() override;
	void PlayFireMontage(bool bAiming);
	void PlayHitReactMontage();
	void PlayElimMontage();
	void PlayReloadMontage();
	
	void Calculate_AO_Pitch();
	
	UFUNCTION(Reliable, NetMulticast)
	void MulticastEliminated();
	
	void Eliminated();
	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	
protected:
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	void UpdateHUDHealth();
	void RotateInPlace(float DeltaTime);
	// Poll for any relevant classes 
	void PollInit();
protected:
	virtual void BeginPlay() override;
	
#pragma region input
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	virtual void Jump() override;
	
	void AimOffset(float DeltaTime);
	void SimProxiesTurn();
	void AimButtonPressed();
	void AimButtonReleased();
	
	void CrouchButtonPressed();
	void CrouchButtonReleased();
	
	void EquipButtonPressed();

	void FireButtonPressed();
	void FireButtonReleased();
	void ReloadButtonPressed();
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	void TurnInPlace(float DeltaTime);

	void HideCharacterIfCharacterClose();
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	float CameraThreshold = 200.f;
#pragma endregion
	
private:
#pragma region components
	float CalculateSpeed();
	
	UPROPERTY(EditAnywhere, Category = "PlayerStats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "PlayerStats")
	float Health = 100.f;
	
	UFUNCTION()
	void OnRep_Health();
	
	float InterAO_Yaw;
	float AO_Yaw;
	float AO_Pitch;
	bool bRotateRootBone;
	bool bElimmed = false;
	float ProxyYaw;
	float TurnThreshold = .5f;
	float TimeSinceLastMovementReplication;
	
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AWeapon* OverlappingWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComponent;
	
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	/*
	 * Montages
	 */
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* HitReact;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* ReloadMontage;
	
	/*
	 * Montages
	 */
	
	UPROPERTY()
	ABlasterPlayerController* BlasterPlayerController;
	
	FTimerHandle ElimTimer;
	void OnElimTimerFinished();
	
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;

	/*
	 * Dissolve Effect
	 */

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimelineComponent;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurveFloat;
	
	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterial;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance;
	
	FOnTimelineFloat DissolveTrack;

	UFUNCTION()
	void UpdateDissolveMaterial(float Value);
	
	void StartDissolve();

	/*
	 * ElimBot 
	 */
	
	UPROPERTY(EditAnywhere, Category = Elim)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere, Category = Elim)
	USoundCue* ElimBotSound;

	UPROPERTY()
	ABlasterPlayerState* BlasterPlayerState;
#pragma endregion
	
	
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);
public:
	 void SetOverlappingItem(AWeapon* Weapon);
	 bool IsWeaponEquipped();
	 bool IsAiming();
	
	 FORCEINLINE float Get_AO_Yaw() const { return  AO_Yaw;}
	 FORCEINLINE float Get_AO_Pitch() const { return  AO_Pitch;}
	 FORCEINLINE ETurningInPlace GetTurningInPlaceEnum() const { return TurningInPlace;}
	 AWeapon* GetEquippedWeapon();
	
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetCameraComponent() { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const {return MaxHealth; }
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	ECombatState GetCombatState() const;


};
