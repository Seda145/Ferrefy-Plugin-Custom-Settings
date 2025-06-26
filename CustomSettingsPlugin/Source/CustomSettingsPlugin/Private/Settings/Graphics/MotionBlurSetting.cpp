/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "MotionBlurSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Engine.h"


FS_SettingSolverSettingRegistration UMotionBlurSetting::GetSolverRegistrationInfo_Implementation() const {
	return { 
		UCustomSettingsUtils::SettingMotionBlur,
		{
			{UCustomSettingsUtils::ValueOff},
			{UCustomSettingsUtils::ValueOn}
		},
		true
	};
}

FString UMotionBlurSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueOff;
}

bool UMotionBlurSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	const TConsoleVariableData<float>* CVar = IConsoleManager::Get().FindTConsoleVariableDataFloat(TEXT("r.MotionBlur.Amount"));
	if (CVar == nullptr) {
		return false;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueOn) {
		return FMath::IsNearlyEqual(CVar->GetValueOnAnyThread(), 1);
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueOff) {
		return FMath::IsNearlyEqual(CVar->GetValueOnAnyThread(), 0);
	}

	return false;
}

void UMotionBlurSetting::ApplyValueToSystem_Implementation() {
	if (!GEngine) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GEngine invalid."));
		return;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueOn) {
		GEngine->Exec(GetWorld(), TEXT("r.MotionBlur.Amount 1"));
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueOff) {
		GEngine->Exec(GetWorld(), TEXT("r.MotionBlur.Amount 0"));
	}
}