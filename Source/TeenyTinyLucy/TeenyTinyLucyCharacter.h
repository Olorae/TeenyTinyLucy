// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Companion/Companion.h"
#include "Component/DecalsAimComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Logging/LogMacros.h"
#include "Public/BaseCharacter.h"
#include "TeenyTinyLucyCharacter.generated.h"

class UAimComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class APlateforme;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWallSlideChanged, bool, bIsSliding);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTagChanged, AActor*, Actor, FGameplayTag, Tag);

USTRUCT(BlueprintType)
struct FProjectileSplineData
{
	GENERATED_BODY()

	FVector CurrentLocation;
	FVector CurrentTangent;
	FVector NextLocation;
	FVector NextTangent;
};

UCLASS(config=Game)
class ATeenyTinyLucyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	/** AdjustCompanionSpeed Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IncreaseCompanionSpeedAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DecreaseCompanionSpeedAction;

public:
	ATeenyTinyLucyCharacter();

	/** References */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reference")
	ACompanion* CompanionReference;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reference")
	APlateforme* PlateformeReference;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reference")
	TSubclassOf<ACompanion> CompanionClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reference")
	APlateforme* LastBouncedPlatform;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	/** Properties */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aiming")
	double AimingSensitivityMin = 10.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aiming")
	double AimingSensitivityMax = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aiming")
	float MappedValueMin = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aiming")
	float MappedValueMax = 10.0f;

	FTimerHandle JumpTimerHandle;
	FTimerHandle TimerDrawPath;
	FTimerHandle RecallTimerHandle;
	FGameplayTagContainer GameplayTagContainer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PredictParamsSimFrequency = 13.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAiming = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlateformePrediction = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	float PlateformeAliveDistance = 10000.0f;
	float BackwardSpace = 20.0f; // Space between bob and the back of the player (bigger = farther)
	float FrequencyOfWaves = 5.0f; // Greater number = more waves for the same speed
	float AmplitudeWaves = 3.12f;
	float TimerDeactivateJump = 0.4f;
	FVector WallNormal;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAimComponent* AimComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	float LaunchSpeed = 1000.0f;
	// Spline
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile")
	USplineComponent* ProjectileSpline;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="splie")
	FVector LastSplinePointLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* PointMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* PlateformeMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USplineMeshComponent*> SplineMeshArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile")
	TArray<USplineMeshComponent*> SplineMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USplineMeshComponent* SplineMeshNormal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USplineMeshComponent* SplineMeshPlateforme;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NbSplinePoint = 0;

	// Plateforme
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Plateforme")
	TSubclassOf<AActor> PlateformeClass;
	bool bIsOnPlateforme = false;
	bool bJumpedAbovePlateforme = false;
	bool bIsOverlappingPlateforme = false;

	/*
	float MaxPlatform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float currentPlatformNumber;
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NbPlateformMax = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NbPlateformCurrent = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PredictParamsMaxSimTime = 1.5f;
	bool bIsJumping = false;
	FRotator RotationWhenShoot;
	// Companion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileComponent")
	float TimeBobAlive = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCompanionIsTransformed = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reference")
	bool bIsCompanionDetached = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileComponent")
	float CompanionSpeed;
	// Sliding
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Slide")
	bool bIsWallSliding = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Slide")
	float WallSlideRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Slide")
	float WallSlideSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Slide")
	float WallJumpHorizontalForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Slide")
	float WallJumpVerticalForce;
	bool canRotateSliding = true;
	void setWallSliding(bool bIsWallSliding);
	// Decals
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDecalsAimComponent* DecalsAimComponent;
	bool bIsCompanionThrown = false;
	// Loops
	int Loop = 0;

	/** DELEGATES */
	UPROPERTY(BlueprintAssignable, Category = "Wall Slide")
	FOnWallSlideChanged OnWallSlideChanged;

	/** Functions */
	void AdjustCompanionSpeed(float Adjustment);
	UFUNCTION(BlueprintCallable)
	void IncreaseCompanionSpeed();
	UFUNCTION(BlueprintCallable)
	void DecreaseCompanionSpeed();
	UFUNCTION(BlueprintNativeEvent)
	void AddSplineMeshes();
	UFUNCTION(BlueprintCallable)
	void UpdateProjectionSpline(USplineMeshComponent* MeshNormal, USplineMeshComponent* MeshPlateforme);
	UFUNCTION(BlueprintCallable)
	void ConfigureSplineMeshSegments(int32 Index, USplineMeshComponent* MeshComp);
	void AddSplineMeshPlateforme(FVector& CurrentLocation, USplineMeshComponent* MeshComp) const;
	void AddSplineMeshArrow(const FVector& CurrentLocation, const FVector& CurrentTangent, const FVector& NextLocation,
	                               const FVector& NextTangent, USplineMeshComponent* MeshComp);
	UFUNCTION(BlueprintCallable)
	void SpawnPlateforme();
	FPredictProjectilePathResult PredictProjectilePath();
	FVector GetLaunchVelocity() const;
	UFUNCTION(BlueprintCallable)
	void ClearSplineLine();
	UFUNCTION(BlueprintCallable)
	void AttachCompanionToComponent(AActor* TargetActor, USceneComponent* Parent);
	UFUNCTION(BlueprintCallable)
	void RecallCompanion();
	UFUNCTION(BlueprintCallable)
	void CheckPlateformeDistance();
	UFUNCTION(BlueprintCallable)
	void StartTimerBob();
	void PlayerDeath_Implementation() const;
	UFUNCTION(BlueprintNativeEvent)
	void RecallIfNotPlatform();
	virtual void Tick(float DeltaSeconds) override;
	void TryWallHitCheck(FHitResult Hit, FCollisionQueryParams TraceParams, FVector Start, FVector End,
	                     FString Direction);
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
	                       FVector HitLocation, FVector HitNormal, FVector NormalImpulse,
	                       const FHitResult& Hit) override;
	void ApplyWallSlide();
	virtual void Landed(const FHitResult& Hit) override;
	UFUNCTION(BlueprintCallable)
	void Throw();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	virtual void Jump() override;
	virtual void StopJumping() override;
	void StopJumpingTimer();

	UFUNCTION(BlueprintCallable)
	void Aim();
	UFUNCTION(BlueprintCallable)
	void StopAiming();


	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
