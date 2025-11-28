// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class MultiPlayPractice : ModuleRules
{
	public MultiPlayPractice(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
		
			"UMG", "Slate", "SlateCore",
		});

		PrivateDependencyModuleNames.AddRange(new string[] 
		{
			"GameplayAbilities", "GameplayTags", "GameplayTasks",
		});

		PublicIncludePaths.AddRange(new string[] { "MultiPlayPractice" });
	}
}
