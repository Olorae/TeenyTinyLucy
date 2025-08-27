// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelManagerSubsystem.h"
#include "Engine/GameInstance.h"
#include "CustumGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TEENYTINYLUCY_API UCustumGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Level")
	ULevelManagerSubsystem* GetLevelManagerSubsystem();
};
