/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "ShadowQualitySetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "OverallQualityLevelSetting.h"
#include "PlayerSettingsSubsystem.h"


void UShadowQualitySetting::RegisterPlayerSettingsSubsystem(UPlayerSettingsSubsystem* InPlayerSettingsSubsystem) {
	Super::RegisterPlayerSettingsSubsystem(InPlayerSettingsSubsystem);

	InPlayerSettingsSubsystem->OnBenchmarkRan.AddDynamic(this, &UShadowQualitySetting::ActOnBenchmarkRan);
}

FS_SettingSolverSettingRegistration UShadowQualitySetting::GetSolverRegistrationInfo_Implementation() const {
	FS_SettingSolverSettingRegistration Regist = { 
		UCustomSettingsUtils::SettingShadowQuality,
		{
			{UCustomSettingsUtils::ValueLow},
			{UCustomSettingsUtils::ValueMedium},
			{UCustomSettingsUtils::ValueHigh},
			{UCustomSettingsUtils::ValueEpic},
			{UCustomSettingsUtils::ValueCinematic}
		},
		true
	};

	for (FS_SettingSolverValueRegistration& RegistX : Regist.ValueRegistrations) {
		// Applying this setting also applies value "Custom" on setting Overal Quality Level.
		RegistX.Sets.Add(UCustomSettingsUtils::SettingOverallQualityLevel, { UCustomSettingsUtils::ValueCustom, false });
		// Applying setting Overal Quality Level with a quality level on this setting, applies the quality level on this setting.
		RegistX.SetBy.Add(UCustomSettingsUtils::SettingOverallQualityLevel, { RegistX.ValueName, false });
	}

	return Regist;
}


FString UShadowQualitySetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueMedium;
}

bool UShadowQualitySetting::DoesAppliedValueMatchSystemState_Implementation() const {
	const TConsoleVariableData<int32>* CVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("sg.ShadowQuality"));
	check(CVar != nullptr);
	return (CVar->GetValueOnGameThread() == UCustomSettingsUtils::GetQualityPresetStringAsInt(GetAppliedValue()));
}

void UShadowQualitySetting::ApplyValueToSystem_Implementation() {
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("sg.ShadowQuality"));
	check(CVar != nullptr);
	CVar->Set(UCustomSettingsUtils::GetQualityPresetStringAsInt(GetAppliedValue()), ECVF_SetBySystemSettingsIni);
}

// Delegates

void UShadowQualitySetting::ActOnBenchmarkRan(float InCPUPerfIndex, float InGPUPerfIndex) {
	check(IsValid(GetPlayerSettingsSubsystem()));
	GetPlayerSettingsSubsystem()->TryApplySetting(GetSolverRegistrationInfo_Implementation().SettingName, UCustomSettingsUtils::ComputeQualityPresetStringFromPerfIndex(InCPUPerfIndex, InGPUPerfIndex));
}
