// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TeenyTinyLucyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TEENYTINYLUCY_API ATeenyTinyLucyPlayerState : public APlayerState
{
	GENERATED_BODY()

	bool bIsAlive;

public:
	bool IsAlive();

	UFUNCTION(BlueprintCallable)
	void SetIsAlive(bool bIsAlive);
};
