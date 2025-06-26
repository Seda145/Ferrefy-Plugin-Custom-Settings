/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "SubtitleModeSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Engine.h"


FS_SettingSolverSettingRegistration USubtitleModeSetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingSubtitleMode,
		{ 
			{UCustomSettingsUtils::ValueOff},
			{UCustomSettingsUtils::ValueOn}
		},
		true
	};
}

FString USubtitleModeSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueOff;
}

bool USubtitleModeSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	if (!GEngine) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GEngine invalid."));
		return false;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueOff) {
		return GEngine->bSubtitlesEnabled == false;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueOn) {
		return GEngine->bSubtitlesEnabled == true;
	}
	
	return false;
}

void USubtitleModeSetting::ApplyValueToSystem_Implementation() {
	if (!GEngine) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GEngine invalid."));
		return;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueOff) {
		GEngine->bSubtitlesEnabled = false;

	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueOn) {
		GEngine->bSubtitlesEnabled = true;
	}
}