#include "Companion/Plateforme.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "TeenyTinyLucy/TeenyTinyLucyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

APlateforme::APlateforme()
{
	PrimaryActorTick.bCanEverTick = true;

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	RootComponent = HitBox;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(HitBox);

	SkeletalMesh->OnComponentHit.AddDynamic(this, &APlateforme::OnHit);
	HitBox->OnComponentBeginOverlap.AddDynamic(this, &APlateforme::OnBeginOverlap);
	HitBox->OnComponentEndOverlap.AddDynamic(this, &APlateforme::OnEndOverlap);
}

void APlateforme::BeginPlay()
{
	Super::BeginPlay();
	CurrentBounceStrength = BounceStrength;

	SetActorEnableCollision(false);
	CollisionParams.AddIgnoredActor(this);
	CollisionParamsPlayer.AddIgnoredActor(this);

	if (const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (const ATeenyTinyLucyCharacter* Player = Cast<ATeenyTinyLucyCharacter>(PlayerController->GetPawn()))
		{
			CollisionParams.AddIgnoredActor(Player);
		}
	}

	if (USkeletalMeshComponent* PlateformSkeletalMesh = FindComponentByClass<USkeletalMeshComponent>())
	{
		PlateformAnimInstance = PlateformSkeletalMesh->GetAnimInstance();
		if (PlateformAnimInstance)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("AnimInstance saved successfully"));
		}
	}
}

void APlateforme::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewActorLocation = GetActorLocation();
	SetActorEnableCollision(false);

	if (!CollisionWithObjects(NewActorLocation))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("The platform can't spawn, she's destroyed."));
		Destroy();
		SetActorTickEnabled(false);
		return;
	}

	CollisionWithPlayer(NewActorLocation);

	SetActorLocation(NewActorLocation, true);
	SetActorEnableCollision(true);
	SetActorTickEnabled(false);
}

/* *** COLLISIONS *** */
bool APlateforme::CollisionWithObjects(FVector& NewActorLocation)
{
	bHasMovedDueToWall = false;
	int MaxIterations = 50;
	int Iterations = 0;

	while (Iterations < MaxIterations)
	{
		bool bWallHit = GetWorld()->SweepMultiByChannel(
			OutHits,
			NewActorLocation,
			NewActorLocation,
			FQuat::Identity,
			ECC_WorldStatic,
			FCollisionShape::MakeBox(HitBox->GetScaledBoxExtent()),
			CollisionParams
		);

		if (!bWallHit)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, TEXT("Wall not HIT"));
			return true;
		}
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("HIT"));

		for (const FHitResult& Hit : OutHits)
		{
			if (Cast<ATeenyTinyLucyCharacter>(Hit.GetActor()))
			{
				continue;
			}
			FVector Normal = Hit.Normal;
			NewActorLocation += Normal * displacementNumberWalls;
			bHasMovedDueToWall = true;
		}

		Iterations++;
	}

	return false;
}

void APlateforme::CollisionWithPlayer(FVector& NewActorLocation)
{
	int MaxIterations = 50;
	int Iterations = 0;

	while (Iterations < MaxIterations)
	{
		bool bPlayerHit = GetWorld()->SweepMultiByChannel(
			OutHitsPlayer,
			NewActorLocation,
			NewActorLocation,
			FQuat::Identity,
			ECC_Pawn,
			FCollisionShape::MakeBox(HitBox->GetScaledBoxExtent()),
			CollisionParamsPlayer
		);

		if (!bPlayerHit)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Player Not HIT"));
			return;
		}

		if (!bHasMovedDueToWall)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("NO WALLS JUST LUCY"));

			for (const FHitResult& Hit : OutHitsPlayer)
			{
				if (ATeenyTinyLucyCharacter* Lucy = Cast<ATeenyTinyLucyCharacter>(Hit.GetActor()))
				{
					if (Lucy->GetCharacterMovement()->IsFalling())
					{
						NewActorLocation = Lucy->GetActorLocation() - FVector(0.f, 0.f, Lucy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + HitBox->GetScaledBoxExtent().Z + 1.f);
						GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Lucy tombe, plateforme positionnée en dessous"));
					}
					else
					{
						// Déplacer la plateforme en avant de Lucy (direction regard)
						FVector Direction = Lucy->GetActorForwardVector().GetSafeNormal();
						NewActorLocation += Direction * displacementNumber;
					}
					break;
				}
			}
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("WALLS !!!!!!!!!!"));

			for (const FHitResult& Hit : OutHitsPlayer)
			{
				if (Cast<ATeenyTinyLucyCharacter>(Hit.GetActor()))
				{
					NewActorLocation.Z += displacementNumber;
				}
			}
		}

		Iterations++;
	}
}

void APlateforme::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
	if (ATeenyTinyLucyCharacter* Player = Cast<ATeenyTinyLucyCharacter>(OtherActor))
	{
		Player->bIsOverlappingPlateforme = true;
	}
}

void APlateforme::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ATeenyTinyLucyCharacter* Player = Cast<ATeenyTinyLucyCharacter>(OtherActor))
	{
		Player->bIsOverlappingPlateforme = false;
		Player->bJumpedAbovePlateforme = false;
	}
}

void APlateforme::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                        FVector NormalImpulse, const FHitResult& Hit)
{
	if (Hit.Normal.Z > -.9f)
	{
		return;
	}

	if (ATeenyTinyLucyCharacter* Player = Cast<ATeenyTinyLucyCharacter>(OtherActor))
	{
		const float MomentumX = Player->GetVelocity().X;
		const float MomentumY = Player->GetVelocity().Y;
		
		CurrentBounceStrength *= (Player->bJumpedAbovePlateforme || Player->bIsJumping) ? BounceAugmentationFactor : BounceDampingFactor;
		CurrentBounceStrength = FMath::Clamp(CurrentBounceStrength, MinBounceStrength, MaxBounceStrength);

		FVector JumpVelocity(MomentumX, MomentumY, CurrentBounceStrength);
		Player->LaunchCharacter(JumpVelocity, false, false);

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, JumpVelocity.ToString());

		// Marquer plateforme comme la dernière utilisée
		Player->LastBouncedPlatform = this;
		Player->bJumpedAbovePlateforme = false;

		/* *** BOUNCED OF THE ANIM *** */
		static const FName BouncedVarName(TEXT("Bounced"));

		if (FBoolProperty* BoolProp = FindFProperty<FBoolProperty>(PlateformAnimInstance->GetClass(), BouncedVarName))
		{
			BoolProp->SetPropertyValue_InContainer(PlateformAnimInstance, true);

			GetWorld()->GetTimerManager().SetTimer(BounceResetHandle, [this, BoolProp]()
				{
					BoolProp->SetPropertyValue_InContainer(PlateformAnimInstance, false);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Bounced = false"));
				}, 0.1f, false);
		}
	}
}

void APlateforme::ResetBounce()
{
	CurrentBounceStrength = BounceStrength;
}
