// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

using UnrealBuildTool;

public class XxxPlugin : ModuleRules
{
	public XxxPlugin(ReadOnlyTargetRules Target) : base(Target)
	{		
		if (Target.Configuration != UnrealTargetConfiguration.Shipping
		    && Target.Platform == UnrealTargetPlatform.Win64)
		{
			OptimizeCode = CodeOptimization.Never;
		}

		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add("XxxPlugin/Private");

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine"
			}
		);
	}
}
