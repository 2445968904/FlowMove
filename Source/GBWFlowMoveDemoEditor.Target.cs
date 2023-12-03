// Copyright GanBowen 2022-2023. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class GBWFlowMoveDemoEditorTarget : TargetRules
{
	public GBWFlowMoveDemoEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange( new string[] { "GBWFlowMoveDemo" } );
	}
}
