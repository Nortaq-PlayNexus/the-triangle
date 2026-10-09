// Copyright (c) 2026 The Triangle project. Editor build target.
using UnrealBuildTool;

public class TriangleEditorTarget : TargetRules
{
	public TriangleEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Triangle");

		// Build only the plugins explicitly enabled by Triangle.uproject. This
		// avoids unrelated optional/precompiled editor plugins on the target PC.
		bAllowEnginePluginsEnabledByDefault = false;
	}
}
