// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "BaseEnemy.generated.h"

UCLASS()
class TEENYTINYLUCY_API ABaseEnemy : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseEnemy();

	// Add a Behavior tree to select in the editor for your character
	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* TreeAsset;

	// Pawn components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PawnComponent")
	UStaticMeshComponent* SphereMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PawnComponent")
	USphereComponent* SphereComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PawnComponent")
	UPawnMovementComponent* PawnMovementComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PawnComponent")
	float PawnRadius;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
