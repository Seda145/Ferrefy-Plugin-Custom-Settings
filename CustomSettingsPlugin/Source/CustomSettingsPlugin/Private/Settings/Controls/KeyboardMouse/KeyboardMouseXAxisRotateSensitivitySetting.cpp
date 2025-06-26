/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "KeyboardMouseXAxisRotateSensitivitySetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"


FS_SettingSolverSettingRegistration UKeyboardMouseXAxisRotateSensitivitySetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingKeyboardMouseXAxisRotateSensitivity,
		{},
		false
	};
}

FString UKeyboardMouseXAxisRotateSensitivitySetting::GetSaveModeValue_Implementation() const {
	return TEXT("1");
}

bool UKeyboardMouseXAxisRotateSensitivitySetting::DoesAppliedValueMatchSystemState_Implementation() const {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead. 
	return true;
}

void UKeyboardMouseXAxisRotateSensitivitySetting::ApplyValueToSystem_Implementation() {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead.
}

float UKeyboardMouseXAxisRotateSensitivitySetting::GetStepSize_Implementation() const {
	return 0.1f;
}

float UKeyboardMouseXAxisRotateSensitivitySetting::GetMinValue_Implementation() const {
	return 0.1f;
}

float UKeyboardMouseXAxisRotateSensitivitySetting::GetMaxValue_Implementation() const {
	return 4.f;
}