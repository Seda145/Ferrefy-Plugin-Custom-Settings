/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "GamepadYAxisRotateSensitivitySetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"


FS_SettingSolverSettingRegistration UGamepadYAxisRotateSensitivitySetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingGamepadYAxisRotateSensitivity,
		{},
		false
	};
}

FString UGamepadYAxisRotateSensitivitySetting::GetSaveModeValue_Implementation() const {
	return TEXT("1");
}

bool UGamepadYAxisRotateSensitivitySetting::DoesAppliedValueMatchSystemState_Implementation() const {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead. 
	return true;
}

void UGamepadYAxisRotateSensitivitySetting::ApplyValueToSystem_Implementation() {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead.
}

float UGamepadYAxisRotateSensitivitySetting::GetStepSize_Implementation() const {
	return 0.1f;
}

float UGamepadYAxisRotateSensitivitySetting::GetMinValue_Implementation() const {
	return 0.1f;
}

float UGamepadYAxisRotateSensitivitySetting::GetMaxValue_Implementation() const {
	return 4.f;
}