// Fill out your copyright notice in the Description page of Project Settings.


#include "Fragment.h"

#include "EngineUtils.h"
#include "LevelManagerSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "TeenyTinyLucy/TeenyTinyLucyCharacter.h"

DEFINE_LOG_CATEGORY(LogFrag);

// Sets default values
AFragment::AFragment()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Collision component
	//SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CapsuleComponent"));
	//RootComponent = SphereComponent;
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;

	// Mesh
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(BoxComponent);

	// Bindings
	//SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AFragment::OnBeginZoneOverlap);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AFragment::OnBeginOverlap);
}

void AFragment::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogFrag, Warning, TEXT("OnBeginOverlap"));
	
	if (OtherActor->IsA(ATeenyTinyLucyCharacter::StaticClass()))
	{
		if (UIWidgetClass)
		{
			UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), UIWidgetClass);
			if (Widget)
			{
				Widget->AddToViewport();
			}
		}

		// CachedCharacter = Cast<ATeenyTinyLucyCharacter>(OtherActor);
		//
		// // Désactive l'input du joueur si tu veux le figer
		// if (APlayerController* PC = Cast<APlayerController>(CachedCharacter->GetController()))
		// {
		// 	PC->DisableInput(PC);
		// }

		// Timer de redémarrage du niveau
		GetWorld()->GetTimerManager().SetTimer(TeleportTimerHandle, this, &AFragment::RestartLevel, 3.0f, false);
		StaticMeshComponent->SetVisibility(false);
	}
}

void AFragment::RestartLevel()
{
	if (UWorld* World = GetWorld())
	{
		if (ULevelManagerSubsystem* LevelManager = World->GetGameInstance()->GetSubsystem<ULevelManagerSubsystem>())
		{
			LevelManager->RestartLevel();
		}
	}
}

