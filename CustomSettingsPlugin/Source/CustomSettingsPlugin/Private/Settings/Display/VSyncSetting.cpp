/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "VSyncSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"


FS_SettingSolverSettingRegistration UVSyncSetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingVSync,
		{ 
			{UCustomSettingsUtils::ValueOff},
			{UCustomSettingsUtils::ValueOn}
		},
		true
	};
}

FString UVSyncSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueOff;
}

bool UVSyncSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	const TConsoleVariableData<int32>* CVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.VSync"));
	check(CVar != nullptr);
	bool bIsEnabled = CVar->GetValueOnGameThread() != 0;
	return bIsEnabled == UCustomSettingsUtils::StringToBool(GetAppliedValue());
}

void UVSyncSetting::ApplyValueToSystem_Implementation() {
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VSync"));
	check(CVar != nullptr);
	CVar->Set(UCustomSettingsUtils::StringToBool(GetAppliedValue()), ECVF_SetBySystemSettingsIni);
}
