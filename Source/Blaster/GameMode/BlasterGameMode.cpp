// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (MatchState == MatchState::WaitingToStart)
	{
		CountDownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountDownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown )
	{
		CountDownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStartingTime = GetWorld()->GetTimeSeconds();
}


void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayerController = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayerController)
		{
			BlasterPlayerController->OnMatchStateSet(GetMatchState());
		}
	}
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* EliminatedCharacter,
	ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	ABlasterPlayerState* AttackerBlasterPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimBlasterPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;
	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	if (AttackerBlasterPlayerState && AttackerBlasterPlayerState != VictimBlasterPlayerState && BlasterGameState) 
	{
		AttackerBlasterPlayerState->AddToScore(1.f);
		BlasterGameState->UpdateTopScore(AttackerBlasterPlayerState);
	}
	
	if (VictimBlasterPlayerState)
	{
		VictimBlasterPlayerState->AddToDefeats(1);
		VictimBlasterPlayerState->AddElimText("You Were Eliminated!");
		// Sets on server
		VictimBlasterPlayerState->AddWeaponTypeText("");
	}
	
	if (EliminatedCharacter)
	{
		EliminatedCharacter->Eliminated();
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		ABlasterPlayerState* VictimBlasterPlayerState = ElimmedController ? Cast<ABlasterPlayerState>(ElimmedController->PlayerState) : nullptr;
		if (VictimBlasterPlayerState)
		{
			VictimBlasterPlayerState->AddElimText("");
		}
		
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}


