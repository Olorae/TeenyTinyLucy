// Copyright Epic Games, Inc. All Rights Reserved.

#include "TeenyTinyLucyCharacter.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "LevelManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TeenyTinyLucyPlayerState.h"
#include "Component/AimComponent.h"
#include "Companion/Plateforme.h"
#include "Components/SplineMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ATeenyTinyLucyCharacter

ATeenyTinyLucyCharacter::ATeenyTinyLucyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	ProjectileSpline = CreateDefaultSubobject<USplineComponent>(TEXT("ProjectileSpline"));
	//ProjectileSpline->SetMobility(EComponentMobility::Movable);
	ProjectileSpline->SetupAttachment(GetMesh());

	setWallSliding(false);

	AimComponent = CreateDefaultSubobject<UAimComponent>(TEXT("AimComponent"));
	DecalsAimComponent = CreateDefaultSubobject<UDecalsAimComponent>(TEXT("DecalsAimComponent"));

	//this->OnTagAdded.AddDynamic(this, &ATeenyTinyLucyCharacter::TagAdded);
}

void ATeenyTinyLucyCharacter::AdjustCompanionSpeed(float Adjustment)
{
	if (!bPlateformePrediction) { return; }
	PredictParamsMaxSimTime = FMath::Clamp(PredictParamsMaxSimTime + Adjustment, .2f, 2.f);
}

void ATeenyTinyLucyCharacter::IncreaseCompanionSpeed()
{
	AdjustCompanionSpeed(.01f);
}

void ATeenyTinyLucyCharacter::DecreaseCompanionSpeed()
{
	AdjustCompanionSpeed(-.01f);
}

void ATeenyTinyLucyCharacter::AddSplineMeshes_Implementation()
{
}

void ATeenyTinyLucyCharacter::UpdateProjectionSpline(USplineMeshComponent* MeshNormal,
                                                     USplineMeshComponent* MeshPlateforme)
{
	ClearSplineLine();

	for (FPredictProjectilePathPointData PathPoint : PredictProjectilePath().PathData)
	{
		ProjectileSpline->AddSplinePoint(PathPoint.Location, ESplineCoordinateSpace::World, true);
	}
}

void ATeenyTinyLucyCharacter::ConfigureSplineMeshSegments(int32 Index, USplineMeshComponent* MeshComp)
{
	FVector CurrentLocation, CurrentTangent, NextLocation, NextTangent;

	ProjectileSpline->GetLocationAndTangentAtSplinePoint(Index, CurrentLocation, CurrentTangent,
	                                                     ESplineCoordinateSpace::World);
	ProjectileSpline->GetLocationAndTangentAtSplinePoint(Index + 1, NextLocation, NextTangent,
	                                                     ESplineCoordinateSpace::World);
	SplineMeshes.Add(MeshComp);

	// If CurrentPoint est last de ProjectileSpline, add "plateforme", else add "arrow"
	(Index == ProjectileSpline->GetNumberOfSplinePoints() - 1)
		? AddSplineMeshPlateforme(CurrentLocation, MeshComp)
		: AddSplineMeshArrow(CurrentLocation, CurrentTangent, NextLocation, NextTangent, MeshComp);

	LastSplinePointLocation = ProjectileSpline->GetLocationAtSplinePoint(
		ProjectileSpline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
}

void ATeenyTinyLucyCharacter::AddSplineMeshPlateforme(FVector& CurrentLocation, USplineMeshComponent* MeshComp) const
{
	const FVector Forward = GetActorForwardVector();
	FVector Min, Max;

	MeshComp->GetLocalBounds(Min, Max);
	CurrentLocation -= (250 * Forward);
	FVector EndPos = CurrentLocation + (Forward * (Max - Min));

	MeshComp->SetStartAndEnd(CurrentLocation, Forward, EndPos, Forward, true);
}

void ATeenyTinyLucyCharacter::AddSplineMeshArrow(const FVector& CurrentLocation, const FVector& CurrentTangent, const FVector& NextLocation,
                                                 const FVector& NextTangent, USplineMeshComponent* MeshComp)
{
	const FVector Forward = GetActorForwardVector();
	FVector Min, Max;
	//FVector EndPos = FMath::Lerp(CurrentLocation, NextLocation, 0.5);
	FVector EndTangent = FMath::Lerp(CurrentTangent, NextTangent, 0.5);

	MeshComp->GetLocalBounds(Min, Max);
	FVector EndPos = CurrentLocation + (Forward * Max - Min);

	MeshComp->SetStartAndEnd(CurrentLocation, CurrentTangent, EndPos, EndTangent, true);
}

void ATeenyTinyLucyCharacter::SpawnPlateforme()
{
	if (!bIsCompanionDetached || bCompanionIsTransformed || NbPlateformCurrent >= NbPlateformMax)
	{
		return;
	}

	FVector CompanionLocation = CompanionReference->GetActorLocation();
	float CompanionRadius = CompanionReference->CompanionRadius;
	FRotator Rotation = FRotator(0.f, RotationWhenShoot.Yaw, 0.f);

	FVector Scale = FVector(1.f, 1.f, 1.f);
	FTransform SpawnTransform(Rotation, CompanionLocation, Scale);
	CompanionReference->Destroy();

	CompanionLocation.Z = CompanionLocation.Z - 500.0f; // CompanionRadius * 5.0f;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	PlateformeReference = GetWorld()->SpawnActor<APlateforme>(
		PlateformeClass,
		SpawnTransform,
		SpawnParams
	);

	bCompanionIsTransformed = true;
	NbPlateformCurrent++;
	//currentPlatformNumber++;
}

FPredictProjectilePathResult ATeenyTinyLucyCharacter::PredictProjectilePath()
{
	FPredictProjectilePathParams PredictParams;
	FPredictProjectilePathResult PredictResult;

	PredictParams.StartLocation = CompanionReference->GetActorLocation();
	PredictParams.LaunchVelocity = GetLaunchVelocity();
	PredictParams.bTraceWithCollision = true;
	PredictParams.ProjectileRadius = 0.0f;
	PredictParams.MaxSimTime = PredictParamsMaxSimTime;
	PredictParams.bTraceWithChannel = true;
	PredictParams.TraceChannel = ECC_WorldStatic;
	PredictParams.ActorsToIgnore.Add(this);
	PredictParams.SimFrequency = 13.0f; //PredictParamsSimFrequency;
	PredictParams.OverrideGravityZ = 0.0f;
	PredictParams.DrawDebugType = EDrawDebugTrace::None;
	PredictParams.DrawDebugTime = 5.0f;
	PredictParams.bTraceComplex = false;

	//UGameplayStatics::Blueprint_PredictProjectilePath_Advanced(GetWorld(), PredictParams, PredictResult);
	UGameplayStatics::PredictProjectilePath(
		GetWorld(),
		PredictParams,
		PredictResult
	);
	return PredictResult;
}

FVector ATeenyTinyLucyCharacter::GetLaunchVelocity() const
{
	FVector LaunchVelocity = FVector(0, 0, 0);
	TObjectPtr<APlayerCameraManager> PlayerCameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	// UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

	if (!PlayerCameraManager) return LaunchVelocity;

	FRotator CameraRotation = PlayerCameraManager->GetCameraRotation();
	FVector ForwardVector = UKismetMathLibrary::GetForwardVector(CameraRotation);

	LaunchVelocity.X = (ForwardVector * LaunchSpeed).X;
	LaunchVelocity.Y = (ForwardVector * LaunchSpeed).Y;
	double ValeurConvertie = FMath::GetMappedRangeValueUnclamped(FVector2D(0.0, 1.0),
	                                                             FVector2D(MappedValueMin, MappedValueMax),
	                                                             ForwardVector.Z);
	LaunchVelocity.Z = ValeurConvertie * CompanionSpeed;

	return LaunchVelocity;
}

void ATeenyTinyLucyCharacter::ClearSplineLine()
{
	GetComponents<USplineMeshComponent>(SplineMeshes);

	for (USplineMeshComponent* SplineMesh : SplineMeshes)
	{
		if (SplineMesh)
		{
			SplineMesh->DestroyComponent();
		}
	}

	SplineMeshes.Empty();
	ProjectileSpline->ClearSplinePoints();
}

void ATeenyTinyLucyCharacter::PlayerDeath_Implementation() const
// const FDamageInfo& DamageInfo
{
	if (ATeenyTinyLucyPlayerState* PS = Cast<ATeenyTinyLucyPlayerState>(GetPlayerState()))
	{
		PS->SetIsAlive(false);

		Controller->ChangeState(NAME_Spectating);
		//Controller->ClientGotoState(NAME_Spectating);

		/*
		 * When switching to spectator mode, UE5 spawns a SpectatorPawn (configured in GameMode) and sets up its HUD.
		 * Design a Custom Spectator HUD:
		 * To enhance user experience, create a custom widget (SpectatorHUD) that shows:
		 *	- Respawn countdown (if applicable).
		 *	- Information on the cause of death (damage details, killer info, etc.).
		 */

		//GEngine->AddOnScreenDebugMessage(-1, 7.5f, FColor::White, TEXT("Player is dead"));
	}
}

void ATeenyTinyLucyCharacter::AttachCompanionToComponent(AActor* TargetActor, USceneComponent* Parent)
{
	if (!TargetActor || !Parent)
	{
		return;
	}

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget,
	                                          EAttachmentRule::SnapToTarget,
	                                          EAttachmentRule::SnapToTarget,
	                                          true);

	TargetActor->AttachToComponent(Parent, AttachmentRules, TEXT("Bob_Socket"));
	bIsCompanionDetached = false;
}

void ATeenyTinyLucyCharacter::RecallCompanion()
{
	if (!bIsCompanionDetached) return;
	
	FVector SpawnLocation;
	FRotator SpawnRotation;
	FActorSpawnParameters SpawnParams;
	
	if (PlateformeReference)
	{
		SpawnLocation = PlateformeReference->GetActorLocation();
		SpawnRotation = PlateformeReference->GetActorRotation();

		PlateformeReference->Destroy();
		
	}
	if (CompanionReference)
	{
		SpawnLocation = CompanionReference->GetActorLocation();
		SpawnRotation = CompanionReference->GetActorRotation();

		CompanionReference->Destroy();
		
		if (USphereComponent* SphereComp = CompanionReference->SphereComponent)
		{
			SphereComp->SetSimulatePhysics(false);
			SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SphereComp->SetEnableGravity(false);
		}
	}
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CompanionReference = GetWorld()->SpawnActor<ACompanion>(CompanionClass, SpawnLocation, SpawnRotation,
																SpawnParams);
	bCompanionIsTransformed = false;
	bIsCompanionDetached = false;
}

void ATeenyTinyLucyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	/* CODE QUI TOUCHE AU WALL SLIDE, MAIS A ÉTÉ ANNULER POUR LE MOMENT
	bool bIsFalling = GetMovementComponent()->Velocity.Z < 0;
	if (bIsFalling)
	{
		FVector Start = GetActorLocation();
		FVector EndFront = Start + GetActorForwardVector() * WallSlideRange;
		FVector EndBack = Start - GetActorForwardVector() * WallSlideRange;
		FVector EndRight = Start + GetActorRightVector() * WallSlideRange;
		FVector EndLeft = Start - GetActorRightVector() * WallSlideRange;

		FHitResult Hit;
		FCollisionQueryParams TraceParams;

		TryWallHitCheck(Hit, TraceParams, Start, EndFront, "front");
		if (!bIsWallSliding)
		{
			TryWallHitCheck(Hit, TraceParams, Start, EndBack, "back");
			if (!bIsWallSliding)
			{
				TryWallHitCheck(Hit, TraceParams, Start, EndRight, "right");
				if (!bIsWallSliding)
				{
					TryWallHitCheck(Hit, TraceParams, Start, EndLeft, "left");
				}
			}
		}
	}

	if (GetCharacterMovement()->IsMovingOnGround() && bIsWallSliding)
	{
		setWallSliding(false);
	}
	*/

	/* *** DECALS *** */
	if (bIsAiming && !bCompanionIsTransformed && bPlateformePrediction)
	{
		DecalsAimComponent->UpdateDecals(LastSplinePointLocation, this);
	}
	else if (bIsCompanionThrown && CompanionReference && !bCompanionIsTransformed)
	{
		DecalsAimComponent->UpdateDecals(CompanionReference->GetActorLocation(), CompanionReference);
	}
	if (!bIsAiming && DecalsAimComponent->bAreDecalsShown)
	{
		DecalsAimComponent->HideDecals();
	}

	/* *** RECALL COMPANION *** */
	if (CompanionReference && CompanionReference->bIsRecalled)
	{
		FVector CompanionLocation = CompanionReference->GetActorLocation();
		FVector TargetLocation = this->GetMesh()->GetSocketLocation(CompanionReference->BobSocketName);

		FVector VectorToTarget = TargetLocation - CompanionLocation;
		VectorToTarget.Normalize();

		FVector NewLocation = CompanionLocation + VectorToTarget * CompanionReference->MaxSpeed * DeltaSeconds;

		float SinusoidalOffset = FMath::Sin(GetWorld()->GetTimeSeconds() * FrequencyOfWaves) * AmplitudeWaves;
		NewLocation.Z += SinusoidalOffset;

		CompanionReference->SetActorLocation(NewLocation);

		if (FVector::Dist(CompanionLocation, TargetLocation) < 10.0f)
		{
			CompanionReference->bIsRecalled = false;
			CompanionReference->SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			CompanionReference->ProjectileMovementComponent->Activate();
			CompanionReference->SphereComponent->SetSimulatePhysics(true);
			AttachCompanionToComponent(CompanionReference, GetMesh());
		}
	}
}

void ATeenyTinyLucyCharacter::TryWallHitCheck(FHitResult Hit, FCollisionQueryParams TraceParams, FVector Start,
                                              FVector End, FString Direction)
{
	/* CODE QUI TOUCHE AU WALL SLIDE, MAIS A ÉTÉ ANNULER POUR LE MOMENT
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && (HitActor->ActorHasTag("Gameplay.Object.Wall") || (Hit.Normal.Z <= .1f && Hit.Normal.Z >= -.1f)))
		{
			WallNormal = Hit.Normal;
			ApplyWallSlide();
			return;
		}
		setWallSliding(false);
	}
	setWallSliding(false);
	*/
}
void ATeenyTinyLucyCharacter::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
                                        bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse,
                                        const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (!Other || Other == this)
	{
		return;
	}
}

void ATeenyTinyLucyCharacter::setWallSliding(bool bNewWallSliding)
{
	// CODE QUI TOUCHE AU WALL SLIDE, MAIS A ÉTÉ ANNULER POUR LE MOMENT
	if (bIsWallSliding != bNewWallSliding)
	{
		bIsWallSliding = bNewWallSliding;
		OnWallSlideChanged.Broadcast(bIsWallSliding);
	}

	if (!bIsAiming)
	{
		GetCharacterMovement()->bOrientRotationToMovement = !bIsWallSliding;
	}
}


void ATeenyTinyLucyCharacter::ApplyWallSlide()
{
	// CODE QUI TOUCHE AU WALL SLIDE, MAIS A ÉTÉ ANNULER POUR LE MOMENT
	FVector CurrentVelocity = GetCharacterMovement()->Velocity;
	CurrentVelocity.Z = FMath::Clamp(CurrentVelocity.Z, -WallSlideSpeed, 0.0f);
	CurrentVelocity.X = 0.0f;
	CurrentVelocity.Y = 0.0f;

	GetCharacterMovement()->Velocity = CurrentVelocity;
	setWallSliding(true);
}

void ATeenyTinyLucyCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATeenyTinyLucyCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		if (bIsAiming)
		{
			double ValeurConvertie = UKismetMathLibrary::MapRangeUnclamped(
				PredictParamsMaxSimTime, 0.2, 1.9, AimingSensitivityMin, AimingSensitivityMax);
			LookAxisVector = FVector2D(LookAxisVector.X / ValeurConvertie, LookAxisVector.Y / ValeurConvertie);
		}
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATeenyTinyLucyCharacter::Jump()
{
	bIsJumping = true;


	// Timer for bIsJumping
	GetWorld()->GetTimerManager().SetTimer(JumpTimerHandle, this, &ATeenyTinyLucyCharacter::StopJumpingTimer,
	                                       TimerDeactivateJump, false);

	// Si le joueur saute juste un peu après le hit, il saute plus haut quand meme
	if (!bJumpedAbovePlateforme && bIsOverlappingPlateforme && MovementComponent->Velocity.Z > 0)
	{
		const float MomentumX = GetVelocity().X;
		const float MomentumY = GetVelocity().Y;

		LastBouncedPlatform->CurrentBounceStrength *= LastBouncedPlatform->BounceAugmentationFactor;
		LastBouncedPlatform->CurrentBounceStrength = FMath::Clamp(LastBouncedPlatform->CurrentBounceStrength,
		                                                          LastBouncedPlatform->MinBounceStrength,
		                                                          LastBouncedPlatform->MaxBounceStrength);

		FVector JumpVelocity(MomentumX, MomentumY, LastBouncedPlatform->CurrentBounceStrength);
		LaunchCharacter(JumpVelocity, false, false);

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Magenta, JumpVelocity.ToString());
	}
	bJumpedAbovePlateforme = bIsOverlappingPlateforme;

	if (bIsWallSliding)
	{
		setWallSliding(false);
		FVector WallJumpForce = WallNormal * WallJumpHorizontalForce + FVector(0, 0, WallJumpVerticalForce);
		LaunchCharacter(WallJumpForce, true, true);
	}
	Super::Jump();
}

void ATeenyTinyLucyCharacter::StopJumping()
{
	Super::StopJumping();
	bIsJumping = false;
	GetWorld()->GetTimerManager().ClearTimer(JumpTimerHandle);
}

void ATeenyTinyLucyCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	bIsOnPlateforme = false;
	setWallSliding(false);

	AActor* HitActor = Hit.GetActor();

	// Land on Floor
	if (HitActor && HitActor->ActorHasTag("Gameplay.Object.floor"))
	{
		if (ATeenyTinyLucyPlayerState* PS = Cast<ATeenyTinyLucyPlayerState>(GetPlayerState()))
		{
			PS->SetIsAlive(false);
		}
		if (ULevelManagerSubsystem* LevelManager = GetGameInstance()->GetSubsystem<ULevelManagerSubsystem>())
		{
			LevelManager->RestartLevel();
			return;
		}
	}

	// Land on Plateforme
	if (APlateforme* LandedPlatform = Cast<APlateforme>(HitActor))
	{
		bIsOnPlateforme = true;
		if (LastBouncedPlatform && LastBouncedPlatform != LandedPlatform)
		{
			LastBouncedPlatform->ResetBounce();
			LastBouncedPlatform = nullptr;
		}
	}
	else
	{
		if (LastBouncedPlatform)
		{
			LastBouncedPlatform->ResetBounce();
			NbPlateformCurrent = 1;
		}
		else
		{
			NbPlateformCurrent = 0;
		}
	}
}

void ATeenyTinyLucyCharacter::StopJumpingTimer()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Jump is false after timer"));
	bIsJumping = false;
}


void ATeenyTinyLucyCharacter::Aim()
{
	
	if (bIsCompanionDetached || !CompanionReference)
	{
		return;
	}
	if (AimComponent)
	{
		AimComponent->Aim();
	}
	//CompanionReference->SphereComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void ATeenyTinyLucyCharacter::StopAiming()
{
	if (AimComponent)
	{
		AimComponent->StopAiming();
	}

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	ClearSplineLine();
}

// Timer pour recall bob quand on le lance et le transforme pas en plateforme
void ATeenyTinyLucyCharacter::StartTimerBob()
{
	GetWorld()->GetTimerManager().SetTimer(RecallTimerHandle, this, &ATeenyTinyLucyCharacter::RecallIfNotPlatform,
	                                       TimeBobAlive, false);
}

// Fonction to check si Bob est en pas en plateforme et le recall s'il ne l'est pas 
void ATeenyTinyLucyCharacter::RecallIfNotPlatform_Implementation()
{
	/*
	if (!bCompanionIsTransformed && bIsCompanionDetached)
	{
		RecallCompanion();
	}
	*/
}

// Fonction qui verifie la distance entre la plateforme et Lucy et recall si on depasse une certaine distance
void ATeenyTinyLucyCharacter::CheckPlateformeDistance()
{
	if (bCompanionIsTransformed && PlateformeReference->GetDistanceTo(this) > PlateformeAliveDistance)
	{
		RecallCompanion();
	}
}

void ATeenyTinyLucyCharacter::Throw()
{
	FVector LaunchVelocity;
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FRotator CameraRotation = CameraManager->GetCameraRotation();
	FVector ForwardVector = UKismetMathLibrary::GetForwardVector(CameraRotation);

	CompanionReference->SphereComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CompanionReference->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	bIsCompanionDetached = true;
	bIsCompanionThrown = true;
	GetWorld()->GetTimerManager().ClearTimer(TimerDrawPath);
	ClearSplineLine();
	CompanionReference->SphereComponent->SetEnableGravity(true);

	LaunchVelocity.X = ForwardVector.X * LaunchSpeed;
	LaunchVelocity.Y = ForwardVector.Y * LaunchSpeed;
	float MappedValue = FMath::GetMappedRangeValueUnclamped(
		FVector2D(0.0f, 1.0f),
		FVector2D(MappedValueMin, MappedValueMax),
		ForwardVector.Z
	);
	LaunchVelocity.Z = MappedValue * CompanionSpeed;

	CompanionReference->SphereComponent->SetPhysicsLinearVelocity(LaunchVelocity, false);
	SetActorRotation(GetControlRotation());
	RotationWhenShoot = GetViewRotation();
}

void ATeenyTinyLucyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATeenyTinyLucyCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATeenyTinyLucyCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATeenyTinyLucyCharacter::Look);

		// Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ATeenyTinyLucyCharacter::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this,
		                                   &ATeenyTinyLucyCharacter::StopAiming);

		// Adjust Companion Speed
		EnhancedInputComponent->BindAction(IncreaseCompanionSpeedAction, ETriggerEvent::Triggered, this,
		                                   &ATeenyTinyLucyCharacter::IncreaseCompanionSpeed);
		EnhancedInputComponent->BindAction(IncreaseCompanionSpeedAction, ETriggerEvent::Ongoing, this,
		                                   &ATeenyTinyLucyCharacter::IncreaseCompanionSpeed);
		EnhancedInputComponent->BindAction(DecreaseCompanionSpeedAction, ETriggerEvent::Triggered, this,
		                                   &ATeenyTinyLucyCharacter::DecreaseCompanionSpeed);
		EnhancedInputComponent->BindAction(DecreaseCompanionSpeedAction, ETriggerEvent::Ongoing, this,
		                                   &ATeenyTinyLucyCharacter::DecreaseCompanionSpeed);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}
