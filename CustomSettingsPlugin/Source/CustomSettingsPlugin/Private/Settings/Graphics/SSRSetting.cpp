/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "SSRSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Engine.h"


FS_SettingSolverSettingRegistration USSRSetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingSSR,
		{
			{UCustomSettingsUtils::ValueOff},
			{UCustomSettingsUtils::ValueOn}
		},
		true
	};
}

FString USSRSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueOn;
}

bool USSRSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	const TConsoleVariableData<int32>* CVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.SSR.Quality"));
	if (CVar == nullptr) {
		return false;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueOn) {
		return CVar->GetValueOnAnyThread() == 3;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueOff) {
		return CVar->GetValueOnAnyThread() == 0;
	}

	return false;
}

void USSRSetting::ApplyValueToSystem_Implementation() {
	if (!GEngine) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GEngine invalid."));
		return;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueOn) {
		GEngine->Exec(GetWorld(), TEXT("r.SSR.Quality 3"));
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueOff) {
		GEngine->Exec(GetWorld(), TEXT("r.SSR.Quality 0"));
	}
}