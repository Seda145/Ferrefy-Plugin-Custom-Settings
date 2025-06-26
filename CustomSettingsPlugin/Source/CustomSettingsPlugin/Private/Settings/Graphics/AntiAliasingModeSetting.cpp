/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "AntiAliasingModeSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Engine.h"


FS_SettingSolverSettingRegistration UAntiAliasingModeSetting::GetSolverRegistrationInfo_Implementation() const {
	return { 
		UCustomSettingsUtils::SettingAntiAliasingMode,
		{
			{UCustomSettingsUtils::ValueNone},
			{UCustomSettingsUtils::ValueFXAA},
			{UCustomSettingsUtils::ValueTAA}
		},
		true
	};
}

FString UAntiAliasingModeSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueNone;
}

bool UAntiAliasingModeSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	const TConsoleVariableData<int32>* CVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.AntiAliasingMethod"));
	check(CVar != nullptr);

	if (GetAppliedValue() == UCustomSettingsUtils::ValueNone) {
		return CVar->GetValueOnAnyThread() == 0;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueFXAA) {
		return CVar->GetValueOnAnyThread() == 1;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueTAA) {
		return CVar->GetValueOnAnyThread() == 2;
	}

	return false;
}

void UAntiAliasingModeSetting::ApplyValueToSystem_Implementation() {
	if (!GEngine) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GEngine invalid."));
		return;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueNone) {
		GEngine->Exec(GetWorld(), TEXT("r.AntiAliasingMethod 0"));
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueFXAA) {
		GEngine->Exec(GetWorld(), TEXT("r.AntiAliasingMethod 1"));
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueTAA) {
		GEngine->Exec(GetWorld(), TEXT("r.AntiAliasingMethod 2"));
	}
}
