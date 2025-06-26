/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "ResolutionScalingSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"


FS_SettingSolverSettingRegistration UResolutionScalingSetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingResolutionScale,
		{},
		true
	};
}

FString UResolutionScalingSetting::GetSaveModeValue_Implementation() const {
	return TEXT("100");
}

bool UResolutionScalingSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	const TConsoleVariableData<float>* CVar = IConsoleManager::Get().FindTConsoleVariableDataFloat(TEXT("r.ScreenPercentage"));
	check(CVar != nullptr);
	return (FMath::IsNearlyEqual(CVar->GetValueOnGameThread(), UCustomSettingsUtils::StringToFloat(GetAppliedValue())));
}

void UResolutionScalingSetting::ApplyValueToSystem_Implementation() {
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage"));
	check(CVar != nullptr);
	CVar->Set(UCustomSettingsUtils::StringToFloat(GetAppliedValue()), ECVF_SetBySystemSettingsIni);
}


float UResolutionScalingSetting::GetStepSize_Implementation() const {
	return 1.0f;
}

float UResolutionScalingSetting::GetMinValue_Implementation() const {
	return 10.f;
}

float UResolutionScalingSetting::GetMaxValue_Implementation() const {
	return 100.f;
}