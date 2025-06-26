/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "RayTracedReflectionsSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Engine.h"


FS_SettingSolverSettingRegistration URayTracedReflectionsSetting::GetSolverRegistrationInfo_Implementation() const {
	return { 
		UCustomSettingsUtils::SettingRaytracedReflections,
		{
			{UCustomSettingsUtils::ValueOff},
			{UCustomSettingsUtils::ValueOn}
		},
		true
	};
}

FString URayTracedReflectionsSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueOff;
}

bool URayTracedReflectionsSetting::IsValidValue_Implementation(const FString& InValue) const {
	if (Super::IsValidValue_Implementation(InValue)) {
		if (InValue == GetSaveModeValue_Implementation() || UCustomSettingsUtils::GetCurrentRenderHardwareInterface() == E_RHIMode::RHI_DX12) {
			return true;
		}
	}

	return false;
}

bool URayTracedReflectionsSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	const IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Reflections"));
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

void URayTracedReflectionsSetting::ApplyValueToSystem_Implementation() {
	// TODO Value driven by post process volume?
	// "r.RayTracing.Reflections -1"

	if (!GEngine) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GEngine invalid."));
		return;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueOn) {
		GEngine->Exec(GetWorld(), TEXT("r.RayTracing.Reflections 1"));
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueOff) {
		GEngine->Exec(GetWorld(), TEXT("r.RayTracing.Reflections 0"));
	}
}