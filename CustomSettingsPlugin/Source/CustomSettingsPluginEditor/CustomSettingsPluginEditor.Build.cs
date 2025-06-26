/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
using System.IO;
using UnrealBuildTool;

public class CustomSettingsPluginEditor : ModuleRules {
	public CustomSettingsPluginEditor(ReadOnlyTargetRules Target) : base(Target) {
		PrivatePCHHeaderFile = "Public/CustomSettingsPluginEditor.h";
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
		IWYUSupport = IWYUSupport.Full;

		// While testing if all headers are included properly, set the following:
		// PCHUsage = PCHUsageMode.NoSharedPCHs;
		// bUseUnity = false;

		IncludeSubDirectoriesRecursive(ModuleDirectory + "\\Private", true);
		IncludeSubDirectoriesRecursive(ModuleDirectory + "\\Public", false);
		
		PrivateDependencyModuleNames.AddRange(new string[] {
			"DeveloperToolSettings"
			, "UnrealEd"
		});

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core"
			, "CoreUObject"
			, "Engine"
			// Plugins
			, "UtilityPlugin"
			, "UtilityPluginEditor"
			, "CustomSettingsPlugin"
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
