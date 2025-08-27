// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TeenyTinyLucy/TeenyTinyLucyCharacter.h"
#include "AimComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEENYTINYLUCY_API UAimComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAimComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	float DefaultArmLength = 400;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	FVector DefaultSocketOffset = {0,0,0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	float AimArmLength = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	FVector AimSocketOffset = {0,60,60};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	float DefaultFOV = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	float AimFOV = 70.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	float AimInterpSpeed = 5.0f;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	// float DefaultMouseSensitivity = 1.0f;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	// float AimMouseSensitivity = 0.5f;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	// bool bUseRightShoulder = true;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	// FVector RightShoulderOffset = FVector(0, 60, 60);
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	// FVector LeftShoulderOffset = FVector(0, -60, 60);

	void Aim();
	void StopAiming();

	bool bIsAiming = false;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	private:
	ATeenyTinyLucyCharacter* OwnerCharacter;
	USpringArmComponent* CameraBoom;
	UCameraComponent* CameraComponent;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};