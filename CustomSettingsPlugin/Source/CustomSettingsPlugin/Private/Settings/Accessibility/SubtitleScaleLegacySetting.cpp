/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "SubtitleScaleLegacySetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Engine/Font.h"
#include "Engine.h"


FS_SettingSolverSettingRegistration USubtitleScaleLegacySetting::GetSolverRegistrationInfo_Implementation() const {
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

FString USubtitleScaleLegacySetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueNormal;
}

bool USubtitleScaleLegacySetting::DoesAppliedValueMatchSystemState_Implementation() const {
	if (!GEngine || !IsValid(GEngine->GetSubtitleFont())) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GEngine or font invalid."));
		return false;
	}

	if (GetAppliedValue() == UCustomSettingsUtils::ValueNormal) {
		return GEngine->GetSubtitleFont()->LegacyFontSize == 10;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueLarge) {
		return GEngine->GetSubtitleFont()->LegacyFontSize == 12;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueExtraLarge) {
		return GEngine->GetSubtitleFont()->LegacyFontSize == 15;
	}

	return false;
}

void USubtitleScaleLegacySetting::ApplyValueToSystem_Implementation() {
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
		GEngine->GetSubtitleFont()->LegacyFontSize = 10;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueLarge) {
		GEngine->GetSubtitleFont()->LegacyFontSize = 12;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::ValueExtraLarge) {
		GEngine->GetSubtitleFont()->LegacyFontSize = 15;
	}
}