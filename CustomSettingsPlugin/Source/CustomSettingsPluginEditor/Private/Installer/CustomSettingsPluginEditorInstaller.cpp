/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "CustomSettingsPluginEditorInstaller.h"
#include "AssetUtils.h"
#include "Engine/DataTable.h"
#include "Internationalization/StringTable.h"


// Setup

void FCustomSettingsPluginEditorInstaller::RunAutomatedInstaller() {
	UAssetUtils::AddDirectoryToScanAndCook(TEXT("/CustomSettingsPlugin/Localization/Text"), TEXT("StringTable"), UStringTable::StaticClass());
	UAssetUtils::AddDirectoryToScanAndCook(TEXT("/CustomSettingsPlugin/Data/DataTables"), TEXT("DataTable"), UDataTable::StaticClass());
}