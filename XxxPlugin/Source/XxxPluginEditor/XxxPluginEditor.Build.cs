// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

using UnrealBuildTool;

public class XxxPluginEditor : ModuleRules
{
	public XxxPluginEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		if (Target.Configuration != UnrealTargetConfiguration.Shipping)
		{
			OptimizeCode = CodeOptimization.Never;
		}

		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add("XxxPluginEditor/Private");
		PrivateIncludePaths.Add("XxxPlugin/Private");

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"InputCore"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"XxxPlugin",
                "Engine",
				"CoreUObject",
                "Slate",
                "SlateCore",
                "UnrealEd",
				"WorkspaceMenuStructure",
				"DesktopPlatform",
                "Blutility",
                "UMG",
                "UMGEditor",
				"EditorStyle",
				"Projects",
				"ToolMenus",
			}
		);
	}
}
