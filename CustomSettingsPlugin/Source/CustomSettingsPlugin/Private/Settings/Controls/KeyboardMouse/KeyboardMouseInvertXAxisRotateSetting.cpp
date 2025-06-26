/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "KeyboardMouseInvertXAxisRotateSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"


FS_SettingSolverSettingRegistration UKeyboardMouseInvertXAxisRotateSetting::GetSolverRegistrationInfo_Implementation() const {
	return { 
		UCustomSettingsUtils::SettingKeyboardMouseInvertXAxisRotate,
		{
			{UCustomSettingsUtils::ValueOff},
			{UCustomSettingsUtils::ValueOn}
		},
		false
	};
}

FString UKeyboardMouseInvertXAxisRotateSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueOff;
}

bool UKeyboardMouseInvertXAxisRotateSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead. 
	return true;
}

void UKeyboardMouseInvertXAxisRotateSetting::ApplyValueToSystem_Implementation() {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead.
}