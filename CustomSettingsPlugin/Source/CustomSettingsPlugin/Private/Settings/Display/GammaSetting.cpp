/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "GammaSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Engine.h"


FS_SettingSolverSettingRegistration UGammaSetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingGamma,
		{},
		true
	};
}

FString UGammaSetting::GetSaveModeValue_Implementation() const {
	return TEXT("2.2");
}

bool UGammaSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	if (!IsValid(GEngine)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GEngine is invalid"));
		return false;
	}

	return FMath::IsNearlyEqual(GEngine->DisplayGamma, UCustomSettingsUtils::StringToFloat(GetAppliedValue()));
}

void UGammaSetting::ApplyValueToSystem_Implementation() {
	if (!IsValid(GEngine)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GEngine is invalid"));
		return;
	}

	GEngine->DisplayGamma = UCustomSettingsUtils::StringToFloat(GetAppliedValue());
}

float UGammaSetting::GetStepSize_Implementation() const {
	return 0.1f;
}

float UGammaSetting::GetMinValue_Implementation() const {
	return 0.5f;
}

float UGammaSetting::GetMaxValue_Implementation() const {
	return 5.5f;
}