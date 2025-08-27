// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Fragment.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogFrag, Log, All);

class ATeenyTinyLucyCharacter;

UCLASS()
class TEENYTINYLUCY_API AFragment : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AFragment();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UBoxComponent* BoxComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> UIWidgetClass;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
private:
	void RestartLevel();
	FTimerHandle TeleportTimerHandle;
	ATeenyTinyLucyCharacter* CachedCharacter;
};
