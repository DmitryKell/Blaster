// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Weapon.generated.h"

class ACasing;
class USphereComponent;
class UWidgetComponent;
class UAnimationAsset;
class UTexture2D;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_InitialState UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX"),
};

UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ShowPickupWidget(bool bShowWidget);
	virtual void OnRep_Owner() override;
	virtual void Fire(const FVector& HitTarget);

	UFUNCTION()
	void OnRep_WeaponState();
	void Dropped();

	void UpdateWeaponAmmoHUD();
	// Textures for weapon crosshairs

	UPROPERTY(EditAnywhere, Category = WeaponProperties)
	float CrosshairShootFactor;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsBottom;

	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
#pragma region components
	UPROPERTY(VisibleAnywhere, Category = WeaponProperties, ReplicatedUsing = OnRep_WeaponState)
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category = WeaponProperties)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = WeaponProperties)
	USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnywhere, Category = WeaponProperties)
	UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = WeaponProperties)
	UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category = WeaponProperties)
	TSubclassOf<ACasing> BulletCasing;
	
	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter;

	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController;

	UPROPERTY(EditAnywhere, Replicated, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo;
	
	UPROPERTY()
	ABlasterCharacter* OwnerCharacter;
	
	UPROPERTY()
	ABlasterPlayerController* OwnerPlayerController;
	
	UFUNCTION()
	void OnRep_Ammo();

	void SpendRound();
	

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;
	
	// Zoomed FOV
	UPROPERTY(EditAnywhere, Category = WeaponProperties)
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere, Category = WeaponProperties)
	float ZoomInterpSpeed = 20.f;

	UPROPERTY(EditAnywhere, Category = WeaponProperties)
	float FireFrequency;

	UPROPERTY(EditAnywhere, Category = WeaponProperties)
	bool bAutomaticFire = true;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
	
#pragma endregion

public:
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetWeaponSphereComponent() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	FORCEINLINE float GetWeaponFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetInterpZoomSpeed() const { return ZoomInterpSpeed; }

	FORCEINLINE float GetCrosshairShootFactor() const { return CrosshairShootFactor; }

	FORCEINLINE float GetFireFrequency() const { return FireFrequency; }
	FORCEINLINE bool GetbAutomatic() const { return bAutomaticFire; }
	
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	bool IsEmpty();
};
