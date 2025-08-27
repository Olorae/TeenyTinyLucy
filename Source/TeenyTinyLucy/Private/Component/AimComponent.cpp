// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/AimComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values for this component's properties
UAimComponent::UAimComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAimComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerCharacter = Cast<ATeenyTinyLucyCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		CameraBoom = OwnerCharacter->FindComponentByClass<USpringArmComponent>();
		CameraComponent = OwnerCharacter->FindComponentByClass<UCameraComponent>();
		if (CameraBoom && CameraComponent)
		{
			CameraBoom->TargetArmLength = DefaultArmLength;
			CameraBoom->SocketOffset = DefaultSocketOffset;
			CameraComponent->SetFieldOfView(DefaultFOV);
		}
	}
}


// Called every frame
void UAimComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CameraBoom)
	{
		// Interpolation fluide entre les valeurs
		float TargetLength = bIsAiming ? AimArmLength : DefaultArmLength;
		FVector TargetOffset = bIsAiming ? AimSocketOffset : DefaultSocketOffset;
		float TargetFOV = bIsAiming ? AimFOV : DefaultFOV;

		// Interpolation fluide avec un damping
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, TargetLength, DeltaTime, AimInterpSpeed);
		CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset, TargetOffset, DeltaTime, AimInterpSpeed);
		CameraComponent->SetFieldOfView(TargetFOV);
	}
}

void UAimComponent::Aim()
{
	if (CameraBoom && CameraComponent)
	{
		bIsAiming = true;
	}

}

void UAimComponent::StopAiming()
{
	if (CameraBoom && CameraComponent)
	{
		bIsAiming = false;
	}
}

