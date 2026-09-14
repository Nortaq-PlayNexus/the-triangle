// Copyright (c) 2026 The Triangle project. Engine build target.
using UnrealBuildTool;
using System.Collections.Generic;

public class TriangleTarget : TargetRules
{
	public TriangleTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Triangle");
	}
}