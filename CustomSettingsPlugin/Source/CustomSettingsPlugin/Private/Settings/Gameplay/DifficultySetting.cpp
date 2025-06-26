/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "DifficultySetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"


FS_SettingSolverSettingRegistration UDifficultySetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingDifficulty,
		{
			{UCustomSettingsUtils::ValueEasy},
			{UCustomSettingsUtils::ValueNormal},
			{UCustomSettingsUtils::ValueHard},
			{UCustomSettingsUtils::ValueExpert}
		},
		true
	};
}

FString UDifficultySetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueNormal;
}

bool UDifficultySetting::DoesAppliedValueMatchSystemState_Implementation() const {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead.
	return true;
}

void UDifficultySetting::ApplyValueToSystem_Implementation() {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead.
}