/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "VisionColorDeficiencyTypeSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "PlayerSettingsSubsystem.h"
#include "VisionColorDeficiencySeveritySetting.h"
#include "Framework/Application/SlateApplication.h"


FS_SettingSolverSettingRegistration UVisionColorDeficiencyTypeSetting::GetSolverRegistrationInfo_Implementation() const {
	return { 
		UCustomSettingsUtils::SettingVisionColorDeficiencyType,
		{
			{UCustomSettingsUtils::ValueNormal},
			{UCustomSettingsUtils::ValueDeuteranope},
			{UCustomSettingsUtils::ValueProtanope},
			{UCustomSettingsUtils::ValueTritanope}
		},
		true
	};
}

FString UVisionColorDeficiencyTypeSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueNormal;
}

bool UVisionColorDeficiencyTypeSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	// TODO. Can't access GSlateColorDeficiencyType, no getter on Slate. No way to test?
	return false;
}

void UVisionColorDeficiencyTypeSetting::ApplyValueToSystem_Implementation() {
	if (!IsValid(GetPlayerSettingsSubsystem())) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid GetPlayerSettingsSubsystem()"));
		return;
	}
	UVisionColorDeficiencySeveritySetting* VisionColorDeficiencySeveritySetting = Cast<UVisionColorDeficiencySeveritySetting>(GetPlayerSettingsSubsystem()->FindSettingObject(UCustomSettingsUtils::SettingVisionColorDeficiencySeverity));

	// Default to normal color vision deficiency type, even if that setting is not present.
	// Possible that the other setting does not have its value set yet, or that it doesn't exist at all.
	// If it exists but hasn't had its value set, then don't do anything here as we can expect the other setting will still apply its initial value.
	// This one Slate method seems to be the only way to apply both settings.
	if (IsValid(VisionColorDeficiencySeveritySetting) && VisionColorDeficiencySeveritySetting->HasAppliedValue()) {
		FSlateApplication::Get().GetRenderer()->SetColorVisionDeficiencyType(UCustomSettingsUtils::GetStringAsColorVisionDeficiency(GetAppliedValue()), UCustomSettingsUtils::StringToFloat(VisionColorDeficiencySeveritySetting->GetAppliedValue()), true, false);
	}
	else {
		FSlateApplication::Get().GetRenderer()->SetColorVisionDeficiencyType(UCustomSettingsUtils::GetStringAsColorVisionDeficiency(GetAppliedValue()), 1.f, true, false);
	}
}