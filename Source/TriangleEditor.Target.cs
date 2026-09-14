// Copyright (c) 2026 The Triangle project. Editor build target.
using UnrealBuildTool;
using System.Collections.Generic;

public class TriangleEditorTarget : TargetRules
{
	public TriangleEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Triangle");
	}
}
