/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
using System.IO;
using UnrealBuildTool;

public class CustomSettingsPlugin : ModuleRules {
	public CustomSettingsPlugin(ReadOnlyTargetRules Target) : base(Target) {
		PrivatePCHHeaderFile = "Public/CustomSettingsPlugin.h";
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
		IWYUSupport = IWYUSupport.Full;

		// While testing if all headers are included properly, set the following:
		// PCHUsage = PCHUsageMode.NoSharedPCHs;
		// bUseUnity = false;

		IncludeSubDirectoriesRecursive(ModuleDirectory + "\\Private", true);
		IncludeSubDirectoriesRecursive(ModuleDirectory + "\\Public", false);
		
		PrivateDependencyModuleNames.AddRange(new string[] {
			"RHI"
			, "Slate"
			, "SlateCore"
		});

		PublicDependencyModuleNames.AddRange(new string[] {
			"ApplicationCore" 
			, "Core" 
			, "CoreUObject"
			, "DeveloperSettings"
			, "Engine"
			, "InputCore"
			, "InteractiveToolsFramework"
			, "UMG"
			, "SynthBenchmark"
			// Plugins
			, "UtilityPlugin"
			, "UIAdditionsPlugin"
		});
	}

	private void IncludeSubDirectoriesRecursive(string DirectoryPathToSearch, bool bIsPrivate) {
		foreach (string DirectoryPath in Directory.GetDirectories(DirectoryPathToSearch)) {
			if (bIsPrivate) {
				PrivateIncludePaths.Add(DirectoryPath);
			}
			else {
				PublicIncludePaths.Add(DirectoryPath);
			}
			IncludeSubDirectoriesRecursive(DirectoryPath, bIsPrivate);
		}
	}
}
