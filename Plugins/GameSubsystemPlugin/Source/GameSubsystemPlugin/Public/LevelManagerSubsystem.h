// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelManagerSubsystem.generated.h"

/*
* Access the Subsystem in Game:
*	ULevelManagerSubsystem* LevelManager = GetGameInstance()->GetSubsystem<ULevelManagerSubsystem>();
 */

/**
 * 
 */
UCLASS()
class GAMESUBSYSTEMPLUGIN_API ULevelManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	FName CurrentLevelName;
	UPROPERTY()
	ULevel* CurrentLevel;

public:

	/** Getters */
	UFUNCTION(BlueprintCallable)
	FName GetCurrentLevelName();

	/** Setters */
	UFUNCTION(BlueprintCallable)
	void SetCurrentLevelName(FName LevelName);

	virtual void Deinitialize() override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Use the level's name without .umap
	void ChangeLevel(FName LevelName);
	void RestartLevel();

	void LoadMainMenu();
	void LoadCreditsScreen();

	void LoadDeathScreen();
	void LoadLevelOne();
};
