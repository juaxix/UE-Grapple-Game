// Copyright 2023 - juaxix [xixgames] & giodestone | All Rights Reserved

#include "CMP302GrappleHookGameMode.h"
#include "CMP302GrappleHookHUD.h"
#include "UObject/ConstructorHelpers.h"

ACMP302GrappleHookGameMode::ACMP302GrappleHookGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ACMP302GrappleHookHUD::StaticClass();
}