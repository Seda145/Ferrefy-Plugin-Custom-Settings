/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "RayTracedGlobalIlluminationSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Engine.h"


FS_SettingSolverSettingRegistration URayTracedGlobalIlluminationSetting::GetSolverRegistrationInfo_Implementation() const {
	return { 
		UCustomSettingsUtils::SettingRaytracedGlobalIllumination,
		{
			{UCustomSettingsUtils::ValueOff},
			{UCustomSettingsUtils::ValueOn}
		}
		, true
	};
}

FString URayTracedGlobalIlluminationSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueOff;
}

bool URayTracedGlobalIlluminationSetting::IsValidValue_Implementation(const FString& InValue) const {
	if (Super::IsValidValue_Implementation(InValue)) {
		if (InValue == GetSaveModeValue_Implementation() || UCustomSettingsUtils::GetCurrentRenderHardwareInterface() == E_RHIMode::RHI_DX12) {
			return true;
		}
	}

	return false;
}

bool URayTracedGlobalIlluminationSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	const IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.GlobalIllumination"));
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

void URayTracedGlobalIlluminationSetting::ApplyValueToSystem_Implementation() {
	if (!GEngine) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GEngine invalid."));
		return;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueOn) {
		GEngine->Exec(GetWorld(), TEXT("r.RayTracing.GlobalIllumination 1"));
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueOff) {
		GEngine->Exec(GetWorld(), TEXT("r.RayTracing.GlobalIllumination 0"));
	}
}