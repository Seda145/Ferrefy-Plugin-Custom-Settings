/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "CustomSliderSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "PlayerSettingsSubsystem.h"


float UCustomSliderSetting::GetStepSize_Implementation() const {
	// Override!
	unimplemented();
	return 0.f;
}

float UCustomSliderSetting::GetMinValue_Implementation() const {
	// Override!
	unimplemented();
	return 0.f;
}

float UCustomSliderSetting::GetMaxValue_Implementation() const {
	// Override!
	unimplemented();
	return 0.f;
}

bool UCustomSliderSetting::IsValidValue_Implementation(const FString& InValue) const {
	return Super::IsValidValue_Implementation(InValue) && FMath::IsWithinInclusive(UCustomSettingsUtils::StringToFloat(InValue), GetMinValue(), GetMaxValue());
}

bool UCustomSliderSetting::FindApplicableValueInDirection(bool bInGetNextValue, FString& OutValue) const {
	// This could happen in a valid situation (setting not applied yet, or player 2 doesn't apply global settings).
	if (!HasAppliedValue()) {
		return false;
	}

	const FString SettingName = GetSolverRegistrationInfo().SettingName;;
	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Looking for applicable value for setting %s in direction %s"), *SettingName, *FString(bInGetNextValue ? "Next" : "Previous"));
	OutValue = "";
	check(IsValid(GetPlayerSettingsSubsystem()));

	const FS_SettingSolverSettingInfo& SettingInfo = GetPlayerSettingsSubsystem()->FindSettingInfo(SettingName);
	if (SettingInfo.LockedBySettings.Num() > 0) {
		// To avoid looping this pointlessly, test if the setting is locked before we get started.
		for (const FString& LockX : SettingInfo.LockedBySettings) {
			UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("No applicable value, because the setting %s is locked by: %s"), *SettingName, *LockX);
		}
		return false;
	}
	float OriginalValue = UCustomSettingsUtils::StringToFloat(GetAppliedValue());
	// Immediately get the previous or next step.
	float PrevTestValue = OriginalValue;

	while (true) {
		float TestValue = PrevTestValue + (bInGetNextValue ? GetStepSize() : -GetStepSize());
		// Clamp to slider bounds (ignoring step size), because we want to get to the first and last value.
		TestValue = FMath::Clamp(TestValue, GetMinValue(), GetMaxValue());
		if (FMath::IsNearlyEqual(TestValue, PrevTestValue)) {
			// We hit the start or the end of the slider, there is nothing more to test for in that direction.
			UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("No applicable value found there for setting %s"), *SettingName);
			return false;
		}
		else {
			const FString TestValueString = UCustomSettingsUtils::FloatToString(TestValue);
			if (GetPlayerSettingsSubsystem()->CanApplySetting(SettingName, TestValueString)) {
				// All we need, we're done
				OutValue = TestValueString;
				UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Applicable value found for setting %s, value %s"), *SettingName, *OutValue);
				return true;
			}
		}
		PrevTestValue = TestValue;
	}

	// Nothing in that direction is applicable.
	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("No applicable value found there for setting %s"), *SettingName);
	return false;
}

float UCustomSliderSetting::FindApplicableSliderValueInDirection(bool bInGetNextValue, FString& OutValue) const {
	// Because Blueprints can't deal with overloads.
	return FindApplicableValueInDirection(bInGetNextValue, OutValue);
}

bool UCustomSliderSetting::TryApplySettingInDirection(bool bInGetNextValue) {
	FString OutValue = "";
	return FindApplicableValueInDirection(bInGetNextValue, OutValue) ? GetPlayerSettingsSubsystem()->TryApplySetting(GetSolverRegistrationInfo().SettingName, OutValue) : false;
}

bool UCustomSliderSetting::TryApplySliderSettingInDirection(bool bInGetNextValue) {
	// Because Blueprints can't deal with overloads.
	return TryApplySettingInDirection(bInGetNextValue);
}