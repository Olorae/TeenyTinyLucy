// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/DecalsAimComponent.h"

#include "Components/DecalComponent.h"
#include "Engine/DecalActor.h"

// Sets default values for this component's properties
UDecalsAimComponent::UDecalsAimComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	TraceDistance = 10000;
	MinimumHitDistance = 150;
	DecalSize = FVector(100.0f, 100.0f, 100.0f);
	Directions = {
		FVector(1, 0, 0),  // Devant
		FVector(-1, 0, 0), // Derrière
		FVector(0, 1, 0),    // Droite
		FVector(0, -1, 0),   // Gauche
		FVector(0, 0, -1)       // Bas
	};
	bAreDecalsShown = false;
}

void UDecalsAimComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeDecals(GetOwner());
}

void UDecalsAimComponent::UpdateDecals(FVector Start, AActor* Instigator)
{
	bAreDecalsShown = true;
	
	UWorld* World = GetWorld();
	if (!World || !DecalMaterial) return;

	for (FVector Dir : Directions)
	{
		FHitResult Hit;
		FVector End = Start + Dir * TraceDistance;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Instigator);

		bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams);
		float HitDistance = bHit ? (Hit.Location - Start).Size() : 0.f;

		ADecalActor* DecalActor = ActiveDecals.Contains(Dir) ? ActiveDecals[Dir] : nullptr;
		if (!IsValid(DecalActor)) continue;

		if (bHit && HitDistance >= MinimumHitDistance)
		{
			FVector DecalLocation = Hit.Location;
			FVector ToStart = (Start - Hit.Location).GetSafeNormal();
			FRotator DecalRotation = FRotationMatrix::MakeFromXY(Hit.Normal, ToStart).Rotator();

			DecalActor->SetActorLocation(DecalLocation);
			DecalActor->SetActorRotation(DecalRotation);
			DecalActor->SetActorHiddenInGame(false);
			DecalActor->GetDecal()->SetVisibility(true);
		}
		else
		{
			DecalActor->SetActorHiddenInGame(true);
			DecalActor->GetDecal()->SetVisibility(false);
		}
	}
}

void UDecalsAimComponent::InitializeDecals(AActor* Instigator)
{
	UWorld* World = GetWorld();
	if (!World || !DecalMaterial) return;

	for (FVector Dir : Directions)
	{
		if (!ActiveDecals.Contains(Dir) || !IsValid(ActiveDecals[Dir]))
		{
			ADecalActor* Decal = World->SpawnActor<ADecalActor>(FVector::ZeroVector, FRotator::ZeroRotator);
			if (IsValid(Decal))
			{
				Decal->SetDecalMaterial(DecalMaterial);
				Decal->GetDecal()->DecalSize = DecalSize;
				Decal->SetActorHiddenInGame(true);
				Decal->GetDecal()->SetVisibility(false);
				Decal->SetLifeSpan(0); // persistant
				ActiveDecals.Add(Dir, Decal);
			}
		}
	}
}

void UDecalsAimComponent::HideDecals()
{
	for (FVector Dir : Directions)
	{
		if (IsValid(ActiveDecals[Dir]))
		{
			ActiveDecals[Dir]->SetActorHiddenInGame(true);
			ActiveDecals[Dir]->GetDecal()->SetVisibility(false);
		}
	}

	bAreDecalsShown = false;
}
