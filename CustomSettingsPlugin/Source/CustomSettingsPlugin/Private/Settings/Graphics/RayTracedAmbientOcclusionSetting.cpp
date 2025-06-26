/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "RayTracedAmbientOcclusionSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Engine.h"


FS_SettingSolverSettingRegistration URayTracedAmbientOcclusionSetting::GetSolverRegistrationInfo_Implementation() const {
	return { 
		UCustomSettingsUtils::SettingRaytracedAmbientOcclusion,
		{
			{UCustomSettingsUtils::ValueOff},
			{UCustomSettingsUtils::ValueOn}	
		},
		true
	};
}

FString URayTracedAmbientOcclusionSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueOff;
}

bool URayTracedAmbientOcclusionSetting::IsValidValue_Implementation(const FString& InValue) const {
	if (Super::IsValidValue_Implementation(InValue)) {
		if (InValue == GetSaveModeValue_Implementation() || UCustomSettingsUtils::GetCurrentRenderHardwareInterface() == E_RHIMode::RHI_DX12) {
			return true;
		}
	}

	return false;
}

bool URayTracedAmbientOcclusionSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	const IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.AmbientOcclusion"));
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

void URayTracedAmbientOcclusionSetting::ApplyValueToSystem_Implementation() {
	if (!GEngine) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GEngine invalid."));
		return;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueOn) {
		GEngine->Exec(GetWorld(), TEXT("r.RayTracing.AmbientOcclusion 1"));
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueOff) {
		GEngine->Exec(GetWorld(), TEXT("r.RayTracing.AmbientOcclusion 0"));
	}
}