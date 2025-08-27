// Fill out your copyright notice in the Description page of Project Settings.


#include "Companion/Companion.h"

ACompanion::ACompanion()
{
	PrimaryActorTick.bCanEverTick = true;

	// Collision component
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(CompanionRadius);
	RootComponent = SphereComponent;

	// Mesh component
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(SphereComponent);

	// Movement component
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);

	// Binding
	SphereComponent->OnComponentHit.AddDynamic(this, &ACompanion::OnHit);
}
void ACompanion::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(Hit.Normal.Z));
}

// Called when the game starts or when spawned
void ACompanion::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACompanion::FireInDirection(const FVector& ShootDirection) const
{
	FVector Target = GetActorLocation() + ShootDirection;
	FVector Direction = Target - GetActorLocation();
	Direction.Z++;
	Direction.Normalize();

	FString DebugMessage = FString::Printf(TEXT("InitialSpeed: %.2f"), ProjectileMovementComponent->InitialSpeed);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, DebugMessage);
	
	ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
	ProjectileMovementComponent->Velocity.Z  = ProjectileMovementComponent->Velocity.Z / 2;
}
