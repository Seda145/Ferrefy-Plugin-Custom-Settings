/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "RayTracedTranslucencySetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Engine.h"


FS_SettingSolverSettingRegistration URayTracedTranslucencySetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingRaytracedTranslucency,
		{
			{UCustomSettingsUtils::ValueOff},
			{UCustomSettingsUtils::ValueOn}
		},
		true
	};
}

FString URayTracedTranslucencySetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueOff;
}

bool URayTracedTranslucencySetting::IsValidValue_Implementation(const FString& InValue) const {
	if (Super::IsValidValue_Implementation(InValue)) {
		if (InValue == GetSaveModeValue_Implementation() || UCustomSettingsUtils::GetCurrentRenderHardwareInterface() == E_RHIMode::RHI_DX12) {
			return true;
		}
	}

	return false;
}

bool URayTracedTranslucencySetting::DoesAppliedValueMatchSystemState_Implementation() const {
	const IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Translucency"));
	if (CVar == nullptr) {
		return false;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueOn) {
		return CVar->GetInt() == 1;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueOff) {
		return CVar->GetInt() == 0;
	}

	return false;
}

void URayTracedTranslucencySetting::ApplyValueToSystem_Implementation() {
	if (!GEngine) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GEngine invalid."));
		return;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueOn) {
		GEngine->Exec(GetWorld(), TEXT("r.RayTracing.Translucency 1"));
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueOff) {
		GEngine->Exec(GetWorld(), TEXT("r.RayTracing.Translucency 0"));
	}
}