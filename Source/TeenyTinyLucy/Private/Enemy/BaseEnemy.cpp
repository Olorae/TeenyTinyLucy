// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BaseEnemy.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"


// Sets default values
ABaseEnemy::ABaseEnemy()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Collision component
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(PawnRadius);
	RootComponent = SphereComponent;

	// Mesh component
	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
	//SphereMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	SphereMesh->SetupAttachment(SphereComponent);

	// Movement component
	PawnMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("ProjectileMovementComponent"));
	//PawnMovementComponent->SetUpdatedComponent(SphereComponent);
	PawnMovementComponent->UpdatedComponent = RootComponent;
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

