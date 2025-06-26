/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "RenderHardwareInterfaceSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/ConfigCacheIni.h"


FS_SettingSolverSettingRegistration URenderHardwareInterfaceSetting::GetSolverRegistrationInfo_Implementation() const {
	return { 
		UCustomSettingsUtils::SettingRenderHardwareInterface,
		{
			{UCustomSettingsUtils::ValueRHIDX11},
			{UCustomSettingsUtils::ValueRHIDX12},
			{UCustomSettingsUtils::ValueRHIVulkan}
		}, 
		true
	};
}

FString URenderHardwareInterfaceSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::GetRHIModeAsString(UCustomSettingsUtils::GetCurrentRenderHardwareInterface());
}

bool URenderHardwareInterfaceSetting::IsValidValue_Implementation(const FString& InValue) const {
	return Super::IsValidValue_Implementation(InValue) && UCustomSettingsUtils::IsRenderHardwareInterfaceSupported(UCustomSettingsUtils::GetStringAsRHIMode(InValue));
}

bool URenderHardwareInterfaceSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	return UCustomSettingsUtils::GetCurrentRenderHardwareInterface() == UCustomSettingsUtils::GetStringAsRHIMode(GetAppliedValue());
}

void URenderHardwareInterfaceSetting::ApplyValueToSystem_Implementation() {
	if (!GConfig) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GConfig invalid."));
		return;
	}

	// TODO This should be processed when reloading the app on every implemented platform.
	GConfig->SetString(*UCustomSettingsUtils::SectionWindowsTargetSettings, *UCustomSettingsUtils::DefaultGraphicsRHI, *GetAppliedValue(), GEngineIni);
	GConfig->Flush(true, GEngineIni);
}