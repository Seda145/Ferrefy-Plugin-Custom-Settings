/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "LensFlaresSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Engine.h"


FS_SettingSolverSettingRegistration ULensFlaresSetting::GetSolverRegistrationInfo_Implementation() const {
	return { 
		UCustomSettingsUtils::SettingLensFlares,
		{
			{UCustomSettingsUtils::ValueOff},
			{UCustomSettingsUtils::ValueOn}
		}, 
		true 
	};
}

FString ULensFlaresSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueOn;
}

bool ULensFlaresSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	const TConsoleVariableData<int32>* CVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.DefaultFeature.LensFlare"));
	if (CVar == nullptr) {
		return false;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueOn) {
		return CVar->GetValueOnAnyThread() == 1;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueOff) {
		return CVar->GetValueOnAnyThread() == 0;
	}

	return false;
}

void ULensFlaresSetting::ApplyValueToSystem_Implementation() {
	if (!GEngine) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GEngine invalid."));
		return;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueOn) {
		GEngine->Exec(GetWorld(), TEXT("r.DefaultFeature.LensFlare 1"));
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueOff) {
		GEngine->Exec(GetWorld(), TEXT("r.DefaultFeature.LensFlare 0"));
	}
}