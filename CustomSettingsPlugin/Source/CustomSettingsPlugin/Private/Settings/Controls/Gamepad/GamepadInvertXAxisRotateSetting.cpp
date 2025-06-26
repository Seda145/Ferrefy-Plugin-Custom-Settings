/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "GamepadInvertXAxisRotateSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"


FS_SettingSolverSettingRegistration UGamepadInvertXAxisRotateSetting::GetSolverRegistrationInfo_Implementation() const {
	return { 
		UCustomSettingsUtils::SettingGamepadInvertXAxisRotate,
		{
			{UCustomSettingsUtils::ValueOff},
			{UCustomSettingsUtils::ValueOn}		
		},
		false
	};
}

FString UGamepadInvertXAxisRotateSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueOff;
}

bool UGamepadInvertXAxisRotateSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead. 
	return true;
}

void UGamepadInvertXAxisRotateSetting::ApplyValueToSystem_Implementation() {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead.
}