// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Blaster/TurningInPlace.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "BlasterCharacter.generated.h"

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
	void PlayFireMontage(bool bAiming);
	void PlayHitReactMontage();
	void Calculate_AO_Pitch();

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
	float InterAO_Yaw;
	float AO_Yaw;
	float AO_Pitch;
	bool bRotateRootBone;
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

	UPROPERTY(VisibleAnywhere, Category = Camera )
	UCombatComponent* CombatComponent;
	
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* HitReact;
	
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
	
	FVector GetHitTarget() const ;
	FORCEINLINE UCameraComponent* GetCameraComponent() { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() { return bRotateRootBone; }
};
