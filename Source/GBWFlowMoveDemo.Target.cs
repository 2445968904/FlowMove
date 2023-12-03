// Copyright GanBowen 2022-2023. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class GBWFlowMoveDemoTarget : TargetRules
{
	public GBWFlowMoveDemoTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange( new string[] { "GBWFlowMoveDemo" } );
	}
}
