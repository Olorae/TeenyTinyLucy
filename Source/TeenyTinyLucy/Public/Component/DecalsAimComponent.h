// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DecalActor.h"
#include "DecalsAimComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEENYTINYLUCY_API UDecalsAimComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDecalsAimComponent();

	// Decals Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decals")
	float TraceDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decals")
	float MinimumHitDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decals")
	UMaterialInterface* DecalMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decals")
	FVector DecalSize;
	
	TMap<FVector, ADecalActor*> ActiveDecals;
	TArray<FVector> Directions;
	bool bAreDecalsShown;

	virtual void BeginPlay() override;
	void UpdateDecals(FVector Start, AActor* Instigator);
	void InitializeDecals(AActor* Instigator);
	void HideDecals();
};


