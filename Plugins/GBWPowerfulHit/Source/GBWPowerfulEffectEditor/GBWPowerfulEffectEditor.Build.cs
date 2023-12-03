// Copyright GanBowen 2022-2023. All Rights Reserved.

using UnrealBuildTool;

public class GBWPowerfulEffectEditor : ModuleRules
{
	public GBWPowerfulEffectEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Engine" });
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "GBWPowerfulEffect"
		});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new[]
			{
				"AnimGraph", "AnimGraphRuntime", "BlueprintGraph", "ToolMenus", "SlateCore", "UnrealEd", "Kismet","KismetCompiler"
			});
		}
	}
}
