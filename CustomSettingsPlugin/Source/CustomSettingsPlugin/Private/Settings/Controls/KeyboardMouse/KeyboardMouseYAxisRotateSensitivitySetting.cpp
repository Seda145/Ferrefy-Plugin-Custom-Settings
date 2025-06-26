/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "KeyboardMouseYAxisRotateSensitivitySetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"


FS_SettingSolverSettingRegistration UKeyboardMouseYAxisRotateSensitivitySetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingKeyboardMouseYAxisRotateSensitivity,
		{},
		false
	};
}

FString UKeyboardMouseYAxisRotateSensitivitySetting::GetSaveModeValue_Implementation() const {
	return TEXT("1");
}

bool UKeyboardMouseYAxisRotateSensitivitySetting::DoesAppliedValueMatchSystemState_Implementation() const {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead. 
	return true;
}

void UKeyboardMouseYAxisRotateSensitivitySetting::ApplyValueToSystem_Implementation() {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead.
}

float UKeyboardMouseYAxisRotateSensitivitySetting::GetStepSize_Implementation() const {
	return 0.1f;
}

float UKeyboardMouseYAxisRotateSensitivitySetting::GetMinValue_Implementation() const {
	return 0.1f;
}

float UKeyboardMouseYAxisRotateSensitivitySetting::GetMaxValue_Implementation() const {
	return 4.f;
}