// Copyright 2023 - juaxix [xixgames] & giodestone | All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CableEndPoint.generated.h"

// Just an empty actor which shows the end point of the cable.
UCLASS()
class CMP302GRAPPLEHOOK_API ACableEndPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ACableEndPoint();

	UPROPERTY(Transient)
	USceneComponent* AttachmentPoint = nullptr;
};
