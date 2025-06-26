/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "WindowModeSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Engine.h"


FS_SettingSolverSettingRegistration UWindowModeSetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingWindowMode,
		{ 
			{UCustomSettingsUtils::ValueFullscreen},
			{UCustomSettingsUtils::ValueWindowedFullscreen},
			{UCustomSettingsUtils::ValueWindowed}
		}
		, true
	};
}

FString UWindowModeSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueFullscreen;
}

bool UWindowModeSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	return GetAppliedValue() == UCustomSettingsUtils::GetStringFromWindowMode(GSystemResolution.WindowMode);
}

void UWindowModeSetting::ApplyValueToSystem_Implementation() {
	GSystemResolution.RequestResolutionChange(GSystemResolution.ResX, GSystemResolution.ResY, UCustomSettingsUtils::GetWindowModeFromString(GetAppliedValue()));
}