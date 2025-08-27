// Fill out your copyright notice in the Description page of Project Settings.


#include "TeenyTinyLucyPlayerState.h"

bool ATeenyTinyLucyPlayerState::IsAlive()
{
	return bIsAlive;
}

void ATeenyTinyLucyPlayerState::SetIsAlive(bool IsAlive)
{
	this->bIsAlive = IsAlive;
}
