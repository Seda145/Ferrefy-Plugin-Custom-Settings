/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "KeyboardMouseInvertYAxisRotateSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"


FS_SettingSolverSettingRegistration UKeyboardMouseInvertYAxisRotateSetting::GetSolverRegistrationInfo_Implementation() const {
	return { 
		UCustomSettingsUtils::SettingKeyboardMouseInvertYAxisRotate,
		{
			{UCustomSettingsUtils::ValueOff},
			{UCustomSettingsUtils::ValueOn}
		},
		false
	};
}

FString UKeyboardMouseInvertYAxisRotateSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueOff;
}

bool UKeyboardMouseInvertYAxisRotateSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead. 
	return true;
}

void UKeyboardMouseInvertYAxisRotateSetting::ApplyValueToSystem_Implementation() {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead.
}