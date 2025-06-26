/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "AmbientOcclusionSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Engine.h"


FS_SettingSolverSettingRegistration UAmbientOcclusionSetting::GetSolverRegistrationInfo_Implementation() const {
	return { 
		UCustomSettingsUtils::SettingAmbientOcclusion,
		{
			{UCustomSettingsUtils::ValueOff},
			{UCustomSettingsUtils::ValueOn}
		},
		true
	};
}

FString UAmbientOcclusionSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueOn;
}

bool UAmbientOcclusionSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	TConsoleVariableData<int32>* CVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.DefaultFeature.AmbientOcclusion"));
	if (CVar == nullptr) {
		return false;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueOff) {
		return CVar->GetValueOnAnyThread() == 0;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueOn) {
		return CVar->GetValueOnAnyThread() == 1;
	}

	return false;
}

void UAmbientOcclusionSetting::ApplyValueToSystem_Implementation() {
	if (!GEngine) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GEngine invalid."));
		return;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueOn) {
		GEngine->Exec(GetWorld(), TEXT("r.DefaultFeature.AmbientOcclusion 1"));
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueOff) {
		GEngine->Exec(GetWorld(), TEXT("r.DefaultFeature.AmbientOcclusion 0"));
	}
}