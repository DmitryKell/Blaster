// Fill out your copyright notice in the Description page of Project Settings.
#include "DuelBlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void ADuelBlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

}

void ADuelBlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
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
		
		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(ElimmedCharacter);
		
		if (BlasterCharacter && BlasterCharacter->GetPlayerStart())
		{
			RestartPlayerAtPlayerStart(ElimmedController, BlasterCharacter->GetPlayerStart());
		}

		
	}
}

