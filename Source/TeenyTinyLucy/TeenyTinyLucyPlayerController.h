// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TeenyTinyLucyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TEENYTINYLUCY_API ATeenyTinyLucyPlayerController : public APlayerController
{
	GENERATED_BODY()

	bool bIsInitSpawned = false;

public:

	// Getters
	bool IsInitSpawned() const;

	// Setters
	void SetIsInitSpawned(bool bIsInitSpawned);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUserWidget* WidgetHUD;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUserWidget* WidgetPause;
};
