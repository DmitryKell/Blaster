// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDElimText(FString Text);
	void SetHUDWeaponAmmo(int32 WeaponAmmo);
	void SetHUDCarriedAmmo(int32 CarriedAmmo);
	void SetWeaponTypeText(FString Text);
	virtual void OnPossess(APawn* InPawn) override;
	void SetHUDMatchCountDown(float CountDownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void SetHUDGrenades(int32 Grenades);
	void SetColorText();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void OnMatchStateSet(FName State);
	void HandleCoolDown();
protected:
	
	virtual void BeginPlay() override;
	virtual void ReceivedPlayer() override; // sync with server clock as soon as possible
	virtual void Tick(float DeltaSeconds) override;
	void SetHUDTime();
	void PollInit();
	/*
	 * Sync time between client and server
	 */

	// Requests the current server time, passing in the clients time when the request was sent
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	// Reports the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	// Difference between client and server time 
	float ClientServerDelta = 0.f;

	//Sync with server world clock
	virtual float GetServerTime();

	
	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.f;
	
	float TimeSyncRunningTime = 0.f;

	void CheckTimeSync(float DeltaTime);

	void HandleMatchHasStarted();
	
	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float StartingTime, float CDTime);
private:
	UPROPERTY()
	class ABlasterHUD* BlasterHUD;

	float LevelStartingTime = 0.f;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownInt = 0;
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	
	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode;
	
	bool bInitializeCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeats;
	int32 HUDGrenades;
};
