/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "FOVSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"


FS_SettingSolverSettingRegistration UFOVSetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingFOV,
		{},
		true
	};
}

FString UFOVSetting::GetSaveModeValue_Implementation() const {
	return TEXT("88");
}

bool UFOVSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	// The FOV setting is not applied here. A PlayerController or PlayerCameraManager can respond to this desired FOV.
	return true;
}

void UFOVSetting::ApplyValueToSystem_Implementation() {
	// The FOV setting is not applied here. A PlayerController or PlayerCameraManager can respond to this desired FOV.
}

float UFOVSetting::GetStepSize_Implementation() const {
	return 1.f;
}

float UFOVSetting::GetMinValue_Implementation() const {
	return 20.f;
}

float UFOVSetting::GetMaxValue_Implementation() const {
	return 170.f;
}