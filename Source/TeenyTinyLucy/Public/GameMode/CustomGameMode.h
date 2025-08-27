// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CustomGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TEENYTINYLUCY_API ACustomGameMode : public AGameMode
{
	GENERATED_BODY()

	bool bCanPlayerRespawn = true;

public:
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;
	virtual void RestartPlayer(AController* NewPlayer) override;

protected:
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
	virtual void FailedToRestartPlayer(AController* NewPlayer) override;

public:
	virtual void RestartGame() override;
	virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;
};
