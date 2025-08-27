// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"

FName ULevelManagerSubsystem::GetCurrentLevelName()
{
	return CurrentLevelName;
}

void ULevelManagerSubsystem::SetCurrentLevelName(FName LevelName)
{
	this->CurrentLevelName = LevelName;
}

void ULevelManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void ULevelManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void ULevelManagerSubsystem::ChangeLevel(FName LevelName)
{
	if (LevelName.IsValid() && CurrentLevelName != LevelName)
	{
		if (UWorld* World = GetWorld())
		{
			UGameplayStatics::OpenLevel(World, LevelName);
			CurrentLevelName = LevelName;
			CurrentLevel = World->GetCurrentLevel();
		}
	}
}

void ULevelManagerSubsystem::RestartLevel()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::OpenLevel(World, CurrentLevelName);
	}
}

void ULevelManagerSubsystem::LoadMainMenu()
{
	ChangeLevel(FName("MainMenu"));
}

void ULevelManagerSubsystem::LoadCreditsScreen()
{
	ChangeLevel(FName("CreditsScreen"));
}

void ULevelManagerSubsystem::LoadDeathScreen()
{
	ChangeLevel(FName("DeathScreen"));
}

void ULevelManagerSubsystem::LoadLevelOne()
{
	ChangeLevel(FName("LevelOne"));
}
