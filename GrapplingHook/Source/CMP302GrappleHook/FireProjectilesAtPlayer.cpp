// Copyright 2023 - juaxix [xixgames] & giodestone | All Rights Reserved

#include "FireProjectilesAtPlayer.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "CMP302GrappleHookProjectile.h"
#include "Kismet/KismetMathLibrary.h"

UFireProjectilesAtPlayer::UFireProjectilesAtPlayer()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UFireProjectilesAtPlayer::IsUpright()
{
	return FMath::Abs(FVector::DotProduct(GetOwner()->GetActorUpVector(), FVector::UpVector)) > FMath::Cos(NoLongerUprightDegrees); // Use Cosine rule to check whether the object is up.
}


// Called every frame
void UFireProjectilesAtPlayer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UWorld* World = GetWorld();
	if (LookFrom == nullptr || Body == nullptr || TurretMountLeft == nullptr || TurretMountRight == nullptr || FirePoints.Num() < 1)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::Red, TEXT("FireProjectilesAtPlayer doesn't have all of its references to components set. Make sure all are set inside of the construction scrip inside of the Turret Blueprint."));
		return;
	}
	
	if (!IsUpright() || !IsValid(LookFrom))
	{
		// Check if turret isnt upright.
		return;
	}

	if (bIsFiringProjectile)
	{
		// Already firing the projectile, don't fire again!
		return;
	}

	ACharacter* Character = IsValid(World->GetFirstPlayerController()) ? World->GetFirstPlayerController()->GetCharacter() : nullptr;
	if (!IsValid(Character))
	{
		// Safety check for if the player got deleted - No error needed as the lack of a player is already noticeable.
		return;
	}
	
	/* Check if the player is within the firing angle. */
	const FVector PlayerLocation = Character->GetActorLocation();
	const FVector TurretLocation = LookFrom->GetComponentLocation();

	FVector TurretToPlayer = (PlayerLocation - TurretLocation);
	TurretToPlayer.Normalize(0.f);

	const double CosOfTurretToPlayer = FVector::DotProduct(TurretToPlayer, LookFrom->GetForwardVector()); // Using Cosine rule.
	if (CosOfTurretToPlayer < FMath::Cos(FMath::DegreesToRadians(LookAngle / 2.f))) 
	{
		// If not in cone of vision.
		return;
	}

	/* Check if nothing is obstructing path to player. */
	FHitResult hitResult;
	const bool bHitSomething = World->LineTraceSingleByChannel(hitResult, LookFrom->GetComponentLocation(), PlayerLocation, ECC_Visibility);

	if (!bHitSomething) //If not obstructed by something, fire projectile.
	{
		World->GetTimerManager().SetTimer(FireProjectileHandle, this, &UFireProjectilesAtPlayer::FireProjectileAtPlayer, FireRateBetweenRounds);
		bIsFiringProjectile = true;
	}
}

void UFireProjectilesAtPlayer::FireProjectileAtPlayer()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	if (CurrentFirePoint >= FirePoints.Num())
	{
		CurrentFirePoint = 0;
	}

	const FVector PlayerLocation = World->GetFirstPlayerController()->GetCharacter()->GetActorLocation();
	const UArrowComponent* FirePointArrow = FirePoints[CurrentFirePoint];
	const FVector FirePointLocation = FirePointArrow->GetComponentLocation();

	if (ProjectileClass == nullptr)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(10, 10.f, FColor::Yellow, "Error - projectile set to null at FireProjectilesAtPlayer class (Check Blueprint with item).");
		}

		return;
	}

	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	const ACMP302GrappleHookProjectile* FiredProjectile = World->SpawnActor<ACMP302GrappleHookProjectile>(ProjectileClass, FirePointLocation, FirePointArrow->GetComponentRotation(), ActorSpawnParameters);

	FVector TestDirection = UKismetMathLibrary::FindLookAtRotation(FirePointLocation, PlayerLocation).Vector();
	if (!TestDirection.Normalize(0.f))
	{
		return;
	}
	
	if (UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(FiredProjectile->GetComponentByClass(UStaticMeshComponent::StaticClass())))
	{
		Mesh->AddImpulse(TestDirection * FireForce);
	}

	CurrentFirePoint++;
	bIsFiringProjectile = false;
}

