// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/CustomGameMode.h"

#include "TeenyTinyLucy/TeenyTinyLucyPlayerController.h"

APlayerController* ACustomGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	return Super::SpawnPlayerController(InRemoteRole, Options);
}

void ACustomGameMode::RestartPlayer(AController* NewPlayer)
{
	ATeenyTinyLucyPlayerController* BaseNewPlayer = Cast<ATeenyTinyLucyPlayerController>(NewPlayer);
	if (!BaseNewPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't cast to ATeenyTinyLucyPlayerController - RestartPlayer() Server Side!"));
		return;
	}

	// Allow the first spawn regardless of respawn rules.
	if (!BaseNewPlayer->IsInitSpawned())
	{
		BaseNewPlayer->SetIsInitSpawned(true);
		Super::RestartPlayer(NewPlayer);
		return;
	}

	// Check the game rule for subsequent respawns.
	if (bCanPlayerRespawn)
	{
		Super::RestartPlayer(NewPlayer);
	}
}

void ACustomGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

void ACustomGameMode::FailedToRestartPlayer(AController* NewPlayer)
{
	Super::FailedToRestartPlayer(NewPlayer);
}

void ACustomGameMode::RestartGame()
{
	Super::RestartGame();
}

bool ACustomGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	return Super::PlayerCanRestart_Implementation(Player);
}
