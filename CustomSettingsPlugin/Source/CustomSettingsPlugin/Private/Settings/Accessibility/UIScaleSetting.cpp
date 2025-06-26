/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "UIScaleSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Engine/UserInterfaceSettings.h"


FS_SettingSolverSettingRegistration UUIScaleSetting::GetSolverRegistrationInfo_Implementation() const {
	return { 
		UCustomSettingsUtils::SettingUIScale,
		{
			{UCustomSettingsUtils::ValueNormal},
			{UCustomSettingsUtils::ValueLarge},
			{UCustomSettingsUtils::ValueExtraLarge}
		},
		true
	};
}

FString UUIScaleSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueNormal;
}

bool UUIScaleSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	UUserInterfaceSettings* UISettings = GetMutableDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass());
	if (IsValid(UISettings)) {
		if (GetAppliedValue() == UCustomSettingsUtils::ValueNormal) {
			return FMath::IsNearlyEqual(UISettings->ApplicationScale, 1.0f);
		}
		else if (GetAppliedValue() == UCustomSettingsUtils::ValueLarge) {
			return FMath::IsNearlyEqual(UISettings->ApplicationScale, 1.25f);
		}
		else if (GetAppliedValue() == UCustomSettingsUtils::ValueExtraLarge) {
			return FMath::IsNearlyEqual(UISettings->ApplicationScale, 1.5f);
		}
	}

	return false;
}

void UUIScaleSetting::ApplyValueToSystem_Implementation() {
	UUserInterfaceSettings* UISettings = GetMutableDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass());
	if (IsValid(UISettings)) {
		if (GetAppliedValue() == UCustomSettingsUtils::ValueNormal) {
			UISettings->ApplicationScale = 1.0f;
		}
		else if (GetAppliedValue() == UCustomSettingsUtils::ValueLarge) {
			UISettings->ApplicationScale = 1.25f;
		}
		else if (GetAppliedValue() == UCustomSettingsUtils::ValueExtraLarge) {
			UISettings->ApplicationScale = 1.5f;
		}
	}
}