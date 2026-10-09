// Copyright (c) 2026 The Triangle project. Engine build target.
using UnrealBuildTool;

public class TriangleTarget : TargetRules
{
	public TriangleTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Triangle");

		// Keep the standalone build limited to the plugins explicitly required by
		// the project; optional engine plugins are not part of this slice.
		bAllowEnginePluginsEnabledByDefault = false;
	}
}
