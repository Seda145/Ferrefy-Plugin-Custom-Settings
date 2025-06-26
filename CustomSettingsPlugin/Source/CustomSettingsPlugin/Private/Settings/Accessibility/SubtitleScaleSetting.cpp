/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "SubtitleScaleSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Engine/Font.h"
#include "Engine.h"


FS_SettingSolverSettingRegistration USubtitleScaleSetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingSubtitleScale,
		{ 
			{UCustomSettingsUtils::ValueNormal},
			{UCustomSettingsUtils::ValueLarge},
			{UCustomSettingsUtils::ValueExtraLarge}
		},
		true
	};
}

FString USubtitleScaleSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueNormal;
}

bool USubtitleScaleSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	if (!GEngine || !IsValid(GEngine->GetSubtitleFont())) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GEngine or font invalid."));
		return false;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueNormal) {
		return FMath::IsNearlyEqual(GEngine->GetSubtitleFont()->GetFontScalingFactor(), 1.f);
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueLarge) {
		return FMath::IsNearlyEqual(GEngine->GetSubtitleFont()->GetFontScalingFactor(), 1.25f);
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueExtraLarge) {
		return FMath::IsNearlyEqual(GEngine->GetSubtitleFont()->GetFontScalingFactor(), 1.5f);
	}

	return false;
}

void USubtitleScaleSetting::ApplyValueToSystem_Implementation() {
	if (!GEngine || !IsValid(GEngine->GetSubtitleFont())) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GEngine or font invalid."));
		return;
	}

	/* 
	* Note on Subtitle Scale setting and its legacy version setting:
	* Legacy font size works with the subtitles the engine generates on the UI when a subtitle text is added to a sound cue.
	* This situation seems only to respond to the LegacyFontSize property, with a side effect that other text using the same font scales as well(text printed to screen).
	* FontScalingFactor seems unused by it? That could be used in a custom UMG implementation, or just use the setting value for that.
	*/

	if (GetAppliedValue() == UCustomSettingsUtils::ValueNormal) {
		GEngine->GetSubtitleFont()->SetFontScalingFactor(1.f);
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueLarge) {
		GEngine->GetSubtitleFont()->SetFontScalingFactor(1.25f);
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueExtraLarge) {
		GEngine->GetSubtitleFont()->SetFontScalingFactor(1.5f);
	}
}