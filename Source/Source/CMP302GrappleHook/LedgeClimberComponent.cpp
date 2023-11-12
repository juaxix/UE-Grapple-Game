// Copyright 2023 - juaxix [xixgames] & giodestone | All Rights Reserved

#include "LedgeClimberComponent.h"
#include "Components/ArrowComponent.h"
#include "CMP302GrappleHookCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"


ULedgeClimberComponent::ULedgeClimberComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULedgeClimberComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsClimbing)
	{
		Climb();
	}
}

bool ULedgeClimberComponent::OnJump()
{
	FHitResult hitResult;
	if (IsArrowOnLedge(FootArrow, hitResult) || IsArrowOnLedge(ForwardArrow, hitResult)) // If either arrow is on ledge, start climbing.
	{
		StartClimbing();
		return false; // Don't allow the character to jump.
	}
	
	return true; // Allow the character to jump.
}

bool ULedgeClimberComponent::OnStopJumping()
{
	StopClimbing();

	return true; //Allow the character to jump.
}

void ULedgeClimberComponent::OnPlayerDied()
{
	StopClimbing();
}

void ULedgeClimberComponent::StartClimbing()
{
	if (bIsClimbing)
	{
		return;
	}

	PlayerCharacter->GetCharacterMovement()->GravityScale = 0.f; // Stop the player from falling.

	PlayerCharacter->GetMovementComponent()->SetPlaneConstraintNormal(FVector(-1.f, 1.f, 0.f)); //Only allow the player to move left and right, and up and down.

	bIsClimbing = true;
}

void ULedgeClimberComponent::StopClimbing()
{
	if (!bIsClimbing)
	{
		return;
	}
	
	PlayerCharacter->GetCharacterMovement()->GravityScale = 1.f; // Allow the player to fall again.

	PlayerCharacter->GetMovementComponent()->SetPlaneConstraintNormal(FVector(0.f, 0.f, 0.f)); // Allow default movement again.

	bIsClimbing = false;
}

void ULedgeClimberComponent::Climb()
{
	if (bIsClimbing)
	{
		/* Launch Character up if is climbing. */
		PlayerCharacter->LaunchCharacter(FVector(PlayerCharacter->GetActorUpVector() * ClimbSpeed), true, true);

		/* Verify that the player is touching the ledge. */
		FHitResult ArrowHit;
		if (!IsArrowOnLedge(FootArrow, ArrowHit))
		{
			if (!IsArrowOnLedge(ForwardArrow, ArrowHit))
			{
				StopClimbing();	
			
				PlayerCharacter->LaunchCharacter(PlayerCharacter->GetActorUpVector() * -GetWorld()->GetGravityZ(), false, true);
				PlayerCharacter->LaunchCharacter(((ForwardArrow->GetUpVector() * EndClimbingUpDirectionModifier) * ForwardArrow->GetForwardVector()).GetSafeNormal(0.f) * ClimbSpeed * EndClimbingLaunchMultiplier, false, false); // Launch 30 degrees relative to up forward.
			}
		}
	}
}

bool ULedgeClimberComponent::IsArrowOnLedge(const UArrowComponent* InArrow, FHitResult& OutHitResult) const
{
	const FCollisionQueryParams CollisionQueryParams;
	
	return GetWorld()->LineTraceSingleByChannel(OutHitResult, InArrow->GetComponentLocation(),
		InArrow->GetComponentLocation() + (InArrow->GetForwardVector() * MaxDistanceToWallToStartClimbing), 
		LedgeTranceChannel, CollisionQueryParams); // Return if the arrow is touching the ledge component.
}
