// Copyright 2023 - juaxix [xixgames] & giodestone | All Rights Reserved

using UnrealBuildTool;
using System.Collections.Generic;

public class CMP302GrappleHookEditorTarget : TargetRules
{
	public CMP302GrappleHookEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.Add("CMP302GrappleHook");
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
	}
}
