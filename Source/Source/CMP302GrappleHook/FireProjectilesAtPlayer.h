// Copyright 2023 - juaxix [xixgames] & giodestone | All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/SoftObjectPath.h"
#include "FireProjectilesAtPlayer.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CMP302GRAPPLEHOOK_API UFireProjectilesAtPlayer : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Projectile)
	TSubclassOf<class ACMP302GrappleHookProjectile> ProjectileClass;
	
	UPROPERTY(BlueprintReadWrite, Category = "Turret")
	class UArrowComponent* LookFrom = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Turret")
	class UStaticMeshComponent* Body = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Turret")
	class UStaticMeshComponent* TurretMountLeft = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Turret")
	class UStaticMeshComponent* TurretMountRight = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Turret")
	TArray<class UArrowComponent*> FirePoints;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret")
	float FireRateBetweenRounds = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret")
	float LookAngle = 55.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret")
	float FireForce = 100000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Turret")
	float NoLongerUprightDegrees = 35.f; // When the turret will switch off when no longer upright.

protected:
	FTimerHandle FireProjectileHandle;

	bool bIsFiringProjectile = false;
	int32 CurrentFirePoint = 0; // For alternating between fire points.
	
public:	
	UFireProjectilesAtPlayer();

protected:

	// If the turret is upright.
	UFUNCTION(BlueprintCallable, Category = "Turret")
	bool IsUpright();
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Turret")
	void FireProjectileAtPlayer();
};
