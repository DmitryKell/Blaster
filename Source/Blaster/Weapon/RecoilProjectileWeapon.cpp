// Fill out your copyright notice in the Description page of Project Settings.


#include "RecoilProjectileWeapon.h"
#include "Components/TimelineComponent.h"

ARecoilProjectileWeapon::ARecoilProjectileWeapon()
{
	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>("Timeline");

}

void ARecoilProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
	InitTimeline();
	if (TimelineComponent && !TimelineComponent->IsPlaying())
	{
		
	}
}

void ARecoilProjectileWeapon::OnFireButtonPressed(bool bPressed)
{
	GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Emerald, bPressed ? TEXT("true") : TEXT("false"), false);
	
	if (bPressed && GetAmmo() != 0)
	{
		if (VectorCurve == nullptr) return;
		TimelineComponent->Play();
		
	}
	
	else if (!bPressed)
	{
		if (VectorCurve == nullptr) return;
		TimelineComponent->Stop();
	}
}

void ARecoilProjectileWeapon::OnReload()
{
	TimelineComponent->SetPlaybackPosition(0, false, false);
	GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Emerald, TEXT("OnReload"), false);
}

void ARecoilProjectileWeapon::InitTimeline()
{
	FOnTimelineVector ProgressUpdate;
	ProgressUpdate.BindUFunction(this, FName("UpdateVectorTimeline"));
	TimelineComponent->AddInterpVector(VectorCurve, ProgressUpdate);
	TimelineComponent->SetPlaybackPosition(0, false, false);
}

void ARecoilProjectileWeapon::UpdateVectorTimeline(FVector Vector)
{
	if (GetInstPawn() == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Emerald, TEXT("Ints in nullptr"), false);
		return;
	}
	
	GetInstPawn()->AddControllerPitchInput(Vector.Y * RecoilPitchStrength);
	GetInstPawn()->AddControllerYawInput(Vector.X * RecoilYawStrength);
}
//TimelineComponent->SetPlaybackPosition(0, false, false);