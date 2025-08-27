// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Plateforme.generated.h" 

UCLASS()
class TEENYTINYLUCY_API APlateforme : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlateforme();

	// Plateforme components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* SkeletalMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* HitBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounce")
	float BounceStrength = 800.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bounce")
	float CurrentBounceStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounce")
	float MinBounceStrength = 400.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounce")
	float MaxBounceStrength = 1300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounce")
	float BounceDampingFactor = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounce")
	float BounceAugmentationFactor = 1.4f;

	/* *** COLLISIONS *** */
	FCollisionQueryParams CollisionParams;
	FCollisionQueryParams CollisionParamsPlayer;
	TArray<FHitResult> OutHits;
	TArray<FHitResult> OutHitsPlayer;
	float displacementNumberWalls = 20.0f;
	float displacementNumber = 10.0f;
	bool bHasMovedDueToWall = false;

	void ResetBounce();
	
	bool CollisionWithObjects(FVector& NewActorLocation);
	void CollisionWithPlayer(FVector& NewActorLocation);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	FTimerHandle BounceResetHandle;
	UAnimInstance* PlateformAnimInstance;
};
