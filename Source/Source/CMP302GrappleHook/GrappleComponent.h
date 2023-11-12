// Copyright 2023 - juaxix [xixgames] & giodestone | All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrappleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CMP302GRAPPLEHOOK_API UGrappleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/* References */
	UPROPERTY()
	class UCameraComponent* PlayerCamera = nullptr;

	UPROPERTY()
	class ACMP302GrappleHookCharacter* PlayerCharacter = nullptr;
	
	UPROPERTY()
	class UCableComponent* Cable = nullptr;
	
	UPROPERTY()
	class UCapsuleComponent* OverlapCapsule = nullptr;
	
	/* Configurable Variables */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Hook")
	float GrappleAndWhipRange = 65000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Hook")
	float GrappleSpeed = 2000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Hook")
	float GrappleStopLerpingAtDistanceFromTarget = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Hook")
	float GrappleBeginGrapplingDelay = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category =  "Grapple Hook")
	float GrappleInitialLaunchVelocity = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Hook|Whipping")
	float WhipForce = 12500000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Hook|Whipping")
	float WhipDelay = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Hook|Whipping")
	FName WhipableTag = "Whipable"; // What is the tag of the actor to whip.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Hook")
	FName NoGrappleTag = "NoGrapple"; // Tag of actors not to connect to.
private:
	/*Whip Related Variables*/
	UPROPERTY(Transient)
	UPrimitiveComponent* WhippedComponent = nullptr;
	FTimerHandle GrappleTimeToWhipHandle;
	FTimerHandle GrappleFinishWhippingHandle;
	bool IsWhipping = false;
	FVector WhipLocation;
	
	/* Grapple Variables */
	FTimerHandle StartGrapplingTimerHandle;
	FTimerHandle GrappleApplyForceInGrappleDirectionHandle;

	float GrappleIncrements = 0.f;
	bool bIsGrappling = false;
	bool PreventGrappleCancel = false;
	float GrappleDistance = 0.f;
	FVector GrappleToLocation;
	FVector GrappleStartLocation;
	
	float GrappleLerpProgress = 0.f;
	
public:	
	// Sets default values for this component's properties
	UGrappleComponent();

	// Called by the player character when the grapple is fired.
	UFUNCTION(BlueprintCallable, Category="Grapple Hook")
	void OnGrappleFired();

	UFUNCTION(BlueprintCallable, Category = "Grapple Hook")
	void OnPlayerDied();
	
protected:
	// Begin Lerping - To be called by time position.
	UFUNCTION()
	void GrappleBeginLerping();

	// Stop Lerping and give the player some velocity so they don't just drop.
	UFUNCTION()
	void StopLerping(bool applyVelocity);

	UFUNCTION()
	void ApplyVelocityWithRope();

	// If the secondary overlap capsule is overlapping with something. Required as raycasts don't always indicate when things are being hit.
	UFUNCTION()
	bool IsOverlapCapsuleOverlappingWithAnotherObject();

	/*Whip component when reached.*/
	UFUNCTION()
	void WhipComponent();

	/*Allow the player to start start using the grapple again.*/
	UFUNCTION()
	void FinishWhippingComponent();
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
