// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterGameMode.h"
#include "DuelBlasterGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ADuelBlasterGameMode : public ABlasterGameMode
{
	GENERATED_BODY()
public:
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController) override;
protected:
	virtual void BeginPlay() override;

};
