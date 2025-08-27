// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TeenyTinyLucy : ModuleRules
{
	public TeenyTinyLucy(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule", "NavigationSystem", "MovieScene",
			"GameplayTags", "GameSubsystemPlugin", "UMG", "IconsPack"
		});
	}
}