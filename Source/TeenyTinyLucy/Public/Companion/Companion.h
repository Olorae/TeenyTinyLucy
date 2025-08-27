// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Companion.generated.h"

UCLASS()
class TEENYTINYLUCY_API ACompanion : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACompanion();

	// Projectile components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileComponent")
	USphereComponent* SphereComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileComponent")
	USkeletalMeshComponent* SkeletalMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileComponent")
	UProjectileMovementComponent* ProjectileMovementComponent;

	// Properties
	bool bIsRecalled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsHitting = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileComponent")
	float CompanionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileComponent")
	float InitialSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileComponent")
	float MaxSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileComponent")
	float LaunchSpeed = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	FName BobSocketName;
	
	// Functions
	void FireInDirection(const FVector& ShootDirection) const;
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
