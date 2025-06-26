/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "CustomSettingsPluginEditor.h"
#include "Modules/ModuleManager.h"
#include "CustomSettingsPluginEditorInstaller.h"


void FCustomSettingsPluginEditorModule::StartupModule() {
	FCustomSettingsPluginEditorInstaller().RunAutomatedInstaller();
}


IMPLEMENT_MODULE(FCustomSettingsPluginEditorModule, CustomSettingsPluginEditor)