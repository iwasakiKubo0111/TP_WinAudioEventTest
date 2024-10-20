// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class WinWAudioEventTest : ModuleRules
{
	public WinWAudioEventTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" , "XRBase" , "OpenXRHMD" , "WindowsPlatformFeatures" , "AudioMixer"});
    }
}
