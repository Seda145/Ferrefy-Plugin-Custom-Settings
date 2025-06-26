/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "GamepadXAxisRotateSensitivitySetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"


FS_SettingSolverSettingRegistration UGamepadXAxisRotateSensitivitySetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingGamepadXAxisRotateSensitivity,
		{},
		false
	};
}

FString UGamepadXAxisRotateSensitivitySetting::GetSaveModeValue_Implementation() const {
	return TEXT("1");
}

bool UGamepadXAxisRotateSensitivitySetting::DoesAppliedValueMatchSystemState_Implementation() const {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead. 
	return true;
}

void UGamepadXAxisRotateSensitivitySetting::ApplyValueToSystem_Implementation() {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead.
}

float UGamepadXAxisRotateSensitivitySetting::GetStepSize_Implementation() const {
	return 0.1f;
}

float UGamepadXAxisRotateSensitivitySetting::GetMinValue_Implementation() const {
	return 0.1f;
}

float UGamepadXAxisRotateSensitivitySetting::GetMaxValue_Implementation() const {
	return 4.f;
}