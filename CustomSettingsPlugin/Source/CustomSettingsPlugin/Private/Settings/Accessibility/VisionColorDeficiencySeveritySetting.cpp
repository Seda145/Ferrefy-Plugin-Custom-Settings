/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "VisionColorDeficiencySeveritySetting.h"
#include "CustomSettingsUtils.h"
#include "VisionColorDeficiencyTypeSetting.h"
#include "PlayerSettingsSubsystem.h"
#include "LogCustomSettingsPlugin.h"
#include "Framework/Application/SlateApplication.h"


FS_SettingSolverSettingRegistration UVisionColorDeficiencySeveritySetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingVisionColorDeficiencySeverity,
		{},
		true
	};
}

FString UVisionColorDeficiencySeveritySetting::GetSaveModeValue_Implementation() const {
	return TEXT("1");
}

bool UVisionColorDeficiencySeveritySetting::DoesAppliedValueMatchSystemState_Implementation() const {
	// TODO. Can't access GSlateColorDeficiencySeverity, no getter on Slate. No way to test?
	return false;
}

void UVisionColorDeficiencySeveritySetting::ApplyValueToSystem_Implementation() {
	if (!IsValid(GetPlayerSettingsSubsystem())) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid GetPlayerSettingsSubsystem()"));
		return;
	}
	UCustomSetting* VisionColorDeficiencyTypeSetting = GetPlayerSettingsSubsystem()->FindSettingObject(UCustomSettingsUtils::SettingVisionColorDeficiencyType);

	// Default to normal color vision deficiency type, even if that setting is not present.
	// Possible that the other setting does not have its value set yet, or that it doesn't exist at all.
	// If it exists but hasn't had its value set, then don't do anything here as we can expect the other setting will still apply its initial value.
	// This one Slate method seems to be the only way to apply both settings.
	if (IsValid(VisionColorDeficiencyTypeSetting) && VisionColorDeficiencyTypeSetting->HasAppliedValue()) {
		EColorVisionDeficiency ColorVisionDeficiency = UCustomSettingsUtils::GetStringAsColorVisionDeficiency(VisionColorDeficiencyTypeSetting->GetAppliedValue());
		FSlateApplication::Get().GetRenderer()->SetColorVisionDeficiencyType(ColorVisionDeficiency, UCustomSettingsUtils::StringToFloat(GetAppliedValue()), true, false);
	}
	else {
		FSlateApplication::Get().GetRenderer()->SetColorVisionDeficiencyType(EColorVisionDeficiency::NormalVision, UCustomSettingsUtils::StringToFloat(GetAppliedValue()), true, false);
	}
}

float UVisionColorDeficiencySeveritySetting::GetStepSize_Implementation() const {
	return 1.f;
}

float UVisionColorDeficiencySeveritySetting::GetMinValue_Implementation() const {
	return 0.f;
}

float UVisionColorDeficiencySeveritySetting::GetMaxValue_Implementation() const {
	return 10.f;
}