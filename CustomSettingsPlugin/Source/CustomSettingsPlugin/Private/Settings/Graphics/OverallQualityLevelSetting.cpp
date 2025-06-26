/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "OverallQualityLevelSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "PlayerSettingsSubsystem.h"


FS_SettingSolverSettingRegistration UOverallQualityLevelSetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingOverallQualityLevel,
		{ 
			{UCustomSettingsUtils::ValueCustom},
			{UCustomSettingsUtils::ValueLow},
			{UCustomSettingsUtils::ValueMedium},
			{UCustomSettingsUtils::ValueHigh},
			{UCustomSettingsUtils::ValueEpic},
			{UCustomSettingsUtils::ValueCinematic}
		},
		false
	};
}

FString UOverallQualityLevelSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueCustom;
}

bool UOverallQualityLevelSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	// Irrelevant. We are either: 
	// - 'custom' (can't be tested)
	// - a preset quality, but responding settings test themselves.
	// So there is no need.
	return true;
}