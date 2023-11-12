// Copyright 2023 - juaxix [xixgames] & giodestone | All Rights Reserved

#include "GrappleComponent.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CMP302GrappleHookCharacter.h"
#include "CableComponent.h"
#include "Components/CapsuleComponent.h"

UGrappleComponent::UGrappleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGrappleComponent::OnGrappleFired()
{
	if (PreventGrappleCancel || bIsWhipping)
	{
		// Don't do anything if not allowed to prevent the cancel, or the grapple is whipping.
		return;
	}

	if (bIsGrappling)
	{
		StopLerping(true);
		return;
	}

	if (!IsValid(PlayerCamera) || !IsValid(PlayerCharacter))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	/* Create line trace. */
	const FVector Start = PlayerCamera->GetComponentLocation();
	const FVector Forward = PlayerCamera->GetForwardVector();
	const FVector End = (Forward * GrappleAndWhipRange) + Start;

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(PlayerCharacter->GetParentActor()); // Ignore self if hit
	
	const bool bHasHitSomething = World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
	if (bHasHitSomething && IsValid(HitResult.GetActor()))
	{
		/*Check that can grapple to this actor.*/
		if (HitResult.GetActor()->ActorHasTag(NoGrappleTag))
		{
			return;
		}
		
		/*Whip Component if its whippable.*/
		if (HitResult.GetActor()->ActorHasTag(WhipableTag))
		{
			bIsWhipping = true;
			
			World->GetTimerManager().SetTimer(GrappleTimeToWhipHandle, this, &UGrappleComponent::WhipComponent, WhipDelay); //Whip in a moment so we can lerp across there.

			WhippedComponent = HitResult.GetComponent();
			WhipLocation = HitResult.ImpactPoint;

			Cable->SetVisibility(true);
			Cable->SetWorldLocation(WhipLocation);
			
			return;
		}

		/* Otherwise Grapple To Location*/
		GrappleDistance = HitResult.Distance; // Save distance for later.

		if (!PlayerCharacter->GetMovementComponent()->IsFlying() && !PlayerCharacter->GetMovementComponent()->IsFalling())
		{
			// Launch up if not already floating.
			PlayerCharacter->LaunchCharacter(FVector::UpVector * GrappleInitialLaunchVelocity, false, true);
		}
		
		/* Begin grappling in some delay. */
		World->GetTimerManager().SetTimer(StartGrapplingTimerHandle, this, &UGrappleComponent::GrappleBeginLerping, GrappleBeginGrapplingDelay);

		GrappleToLocation = HitResult.Location;
		PreventGrappleCancel = true;
	}
}

void UGrappleComponent::OnPlayerDied()
{
	StopLerping(false);
}


void UGrappleComponent::GrappleBeginLerping()
{
	GrappleStartLocation = PlayerCharacter->GetPawnViewLocation(); // Start lerping from here.
	PreventGrappleCancel = false; // Allow the player to cancel.
	bIsGrappling = true; // Begin Lerping

	GrappleLerpProgress = 0.f;
	GrappleIncrements = GrappleSpeed / (GrappleToLocation - GrappleStartLocation).Size(); // For a constant velocity.

	Cable->SetVisibility(true);
	Cable->SetWorldLocation(GrappleToLocation);
	
	PlayerCharacter->Falling();
}

void UGrappleComponent::StopLerping(bool bApplyVelocity)
{
	bIsGrappling = false;
	Cable->SetVisibility(false);

	PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);

	if (GrappleDistance > GrappleStopLerpingAtDistanceFromTarget && bApplyVelocity)
	{
		ApplyVelocityWithRope();
	}
}

void UGrappleComponent::ApplyVelocityWithRope()
{
	/*Launch player in direction of grapple.*/
	const FVector MovementVector = (GrappleToLocation - GrappleStartLocation).GetSafeNormal(0.f);

	PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	
	PlayerCharacter->LaunchCharacter(MovementVector * GrappleSpeed, true, true);
}

bool UGrappleComponent::IsOverlapCapsuleOverlappingWithAnotherObject()
{
	TArray<UPrimitiveComponent*> OverlappingComponents;
	OverlapCapsule->GetOverlappingComponents(OverlappingComponents);
	
	if (!OverlappingComponents.IsEmpty())
	{
		for (const UPrimitiveComponent* Component : OverlappingComponents)
		{
			if (!Component->ComponentHasTag("Player"))
			{
				return true;
			}
		}
	}

	return false;
}

void UGrappleComponent::WhipComponent()
{	
	WhippedComponent->AddForceAtLocation((PlayerCamera->GetComponentLocation() - WhipLocation).GetSafeNormal(0.f) * WhipForce, WhipLocation);
	Cable->SetWorldLocation(Cable->GetComponentLocation()); // Move cable back.
	GetWorld()->GetTimerManager().SetTimer(GrappleFinishWhippingHandle, this, &UGrappleComponent::FinishWhippingComponent, WhipDelay); // Make th
}

void UGrappleComponent::FinishWhippingComponent()
{
	bIsWhipping = false;
	Cable->SetVisibility(false);
}

// Called every frame
void UGrappleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsGrappling)
	{
		const FVector NewLerpPos = FMath::Lerp(GrappleStartLocation, GrappleToLocation, GrappleLerpProgress); // Where the player will be next.

		/* Check if there will be anything in the way when moving to the location. */
		FHitResult sweepResult;
		// todo auto movementVector = (GrappleStartLocation - GrappleToLocation).GetSafeNormal(0.f);
		PlayerCharacter->SetActorLocation(NewLerpPos, true, &sweepResult, ETeleportType::ResetPhysics);

		if (sweepResult.IsValidBlockingHit()) // If would hit something on the way.
		{	
			StopLerping(false);
			return;
		}

		/* Check the capsule if the player has hit anything because the sweep isn't fully accurate. */
		if (IsOverlapCapsuleOverlappingWithAnotherObject())
		{
			StopLerping(false);
			return;
		}

		/* Stop lerping if we're there, or colliding with something .*/
		if ((GrappleToLocation - PlayerCharacter->GetActorLocation()).Size() < GrappleStopLerpingAtDistanceFromTarget ||
			GrappleLerpProgress >= 1.f)
		{
			StopLerping(false);
		}
		
		/* Haven't hit anything, add an increment to the grapple */
		GrappleLerpProgress += GrappleIncrements * DeltaTime;

		/* Need to update every frame because the location is based on relative to stop the rope from spazzing.  */
		Cable->SetWorldLocation(GrappleToLocation);
	}
}

