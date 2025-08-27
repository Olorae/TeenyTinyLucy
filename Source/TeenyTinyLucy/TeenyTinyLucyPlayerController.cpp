// Fill out your copyright notice in the Description page of Project Settings.


#include "TeenyTinyLucyPlayerController.h"

bool ATeenyTinyLucyPlayerController::IsInitSpawned() const
{
	return bIsInitSpawned;
}

void ATeenyTinyLucyPlayerController::SetIsInitSpawned(bool IsInitSpawned)
{
	this->bIsInitSpawned = IsInitSpawned;
}