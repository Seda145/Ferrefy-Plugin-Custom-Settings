/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "AntiAliasingQualitySetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "OverallQualityLevelSetting.h"
#include "PlayerSettingsSubsystem.h"


void UAntiAliasingQualitySetting::RegisterPlayerSettingsSubsystem(UPlayerSettingsSubsystem* InPlayerSettingsSubsystem) {
	Super::RegisterPlayerSettingsSubsystem(InPlayerSettingsSubsystem);

	InPlayerSettingsSubsystem->OnBenchmarkRan.AddDynamic(this, &UAntiAliasingQualitySetting::ActOnBenchmarkRan);
}

FS_SettingSolverSettingRegistration UAntiAliasingQualitySetting::GetSolverRegistrationInfo_Implementation() const {
	FS_SettingSolverSettingRegistration Regist = { 
		UCustomSettingsUtils::SettingAntiAliasingQuality,
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

FString UAntiAliasingQualitySetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueMedium;
}

bool UAntiAliasingQualitySetting::DoesAppliedValueMatchSystemState_Implementation() const {
	const TConsoleVariableData<int32>* CVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("sg.AntiAliasingQuality"));
	check(CVar != nullptr);
	return (CVar->GetValueOnGameThread() == UCustomSettingsUtils::GetQualityPresetStringAsInt(GetAppliedValue()));
}

void UAntiAliasingQualitySetting::ApplyValueToSystem_Implementation() {
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("sg.AntiAliasingQuality"));
	check(CVar != nullptr);
	CVar->Set(UCustomSettingsUtils::GetQualityPresetStringAsInt(GetAppliedValue()), ECVF_SetBySystemSettingsIni);
}

// Delegates

void UAntiAliasingQualitySetting::ActOnBenchmarkRan(float InCPUPerfIndex, float InGPUPerfIndex) {
	check(IsValid(GetPlayerSettingsSubsystem()));
	GetPlayerSettingsSubsystem()->TryApplySetting(GetSolverRegistrationInfo_Implementation().SettingName, UCustomSettingsUtils::ComputeQualityPresetStringFromPerfIndex(InCPUPerfIndex, InGPUPerfIndex));
}
