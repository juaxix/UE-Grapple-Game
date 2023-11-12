// Copyright 2023 - juaxix [xixgames] & giodestone | All Rights Reserved

using UnrealBuildTool;
using System.Collections.Generic;

public class CMP302GrappleHookTarget : TargetRules
{
	public CMP302GrappleHookTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		ExtraModuleNames.Add("CMP302GrappleHook");
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		ShadowVariableWarningLevel = WarningLevel.Error;
	}
}
