// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BaseAiController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/BaseEnemy.h"

ABaseAiController::ABaseAiController(const FObjectInitializer& ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create BehaviorTree Component
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTree Component"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
}

void ABaseAiController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Cast your Character/Pawn to get access to the attributes
	if (auto AICharacter = Cast<ABaseEnemy>(InPawn))
	{
		// Check if the assets has been selected in the editor
		if (AICharacter->TreeAsset && AICharacter->TreeAsset->BlackboardAsset) {

			BlackboardComponent->InitializeBlackboard(*AICharacter->TreeAsset->BlackboardAsset);

			// Run the behavior tree
			BehaviorTreeComponent->StartTree(*AICharacter->TreeAsset);
		}
	}
}

// Called when the game starts or when spawned
void ABaseAiController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseAiController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

