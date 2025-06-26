/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "GamepadRumbleSetting.h"
#include "CustomSettingsUtils.h"


// Setting

FS_SettingSolverSettingRegistration UGamepadRumbleSetting::GetSolverRegistrationInfo_Implementation() const {
	return { 
		UCustomSettingsUtils::SettingGamepadRumble,
		{
			{UCustomSettingsUtils::ValueOff},
			{UCustomSettingsUtils::ValueOn}
		},
		false
	};
}

FString UGamepadRumbleSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueOff;
}

bool UGamepadRumbleSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead. 
	return true;
}

void UGamepadRumbleSetting::ApplyValueToSystem_Implementation() {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead.
}