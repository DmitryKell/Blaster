// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "OverheadWidget.generated.h"

class UTextBlock;

UCLASS()
class BLASTER_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DisplayText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DisplayPlayerNameText;
	
	UFUNCTION(BlueprintCallable)
	void SetDisplayPlayerNameText(APawn* InPawn);
	
	void SetDisplayText(FString TextToDisplay);
	
	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);
protected:
	virtual void NativeDestruct() override;
};
