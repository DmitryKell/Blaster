#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	// if add new weapon type, then edit GetNameOfWeaponType in combat cpp and carried ammo; in blastercharacter add case for reloading smg
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_Submachine UMETA(DisplayName = "Submachine"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_SniperRifle UMETA(DisplayName = "SniperRifle"),
	EWT_GrenadeLauncher UMETA(DisplayName = "GrenadeLauncher"),
	EWT_AR_SO  UMETA(DisplayName = "AR_SO"),
	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};
