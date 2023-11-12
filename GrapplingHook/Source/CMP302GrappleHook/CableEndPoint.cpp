// Copyright 2023 - juaxix [xixgames] & giodestone | All Rights Reserved

#include "CableEndPoint.h"

ACableEndPoint::ACableEndPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	AttachmentPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Attachment")); //Just create an attachment Point
}

