// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//#include "GameplayEffect.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "AIController.h"
#include "BaseAiController.generated.h"

UCLASS()
class TEENYTINYLUCY_API ABaseAiController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseAiController(const FObjectInitializer& ObjectInitializer);

	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(Transient)
	class UBehaviorTreeComponent* BehaviorTreeComponent;
	
	UPROPERTY(Transient)
	class UBlackboardComponent* BlackboardComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};