/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "DisplayResolutionSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Engine.h"


FS_SettingSolverSettingRegistration UDisplayResolutionSetting::GetSolverRegistrationInfo_Implementation() const {
	FS_SettingSolverSettingRegistration Regist = {
		UCustomSettingsUtils::SettingDisplayResolution,
		{},
		true
	};

	TArray<FIntPoint> ChoicesInt = UCustomSettingsUtils::GetSupportedScreenResolutions();
	for (FIntPoint& ChoiceX : ChoicesInt) {
		Regist.ValueRegistrations.Add(UCustomSettingsUtils::ScreenResolutionIntPointToString(ChoiceX));
	}

	return Regist; 
}

FString UDisplayResolutionSetting::GetSaveModeValue_Implementation() const {
	const TArray<FIntPoint> SupportedResolutions = UCustomSettingsUtils::GetSupportedScreenResolutions();
	check(SupportedResolutions.Num() > 0);
	return UCustomSettingsUtils::ScreenResolutionIntPointToString(SupportedResolutions[0]);
}

bool UDisplayResolutionSetting::IsValidValue_Implementation(const FString& InValue) const {
	return Super::IsValidValue_Implementation(InValue) && UCustomSettingsUtils::IsScreenResolutionValid(UCustomSettingsUtils::ScreenResolutionStringToIntPoint(InValue));
}

bool UDisplayResolutionSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	return GetAppliedValue() == UCustomSettingsUtils::ScreenResolutionIntPointToString(FIntPoint(GSystemResolution.ResX, GSystemResolution.ResY));
}

void UDisplayResolutionSetting::ApplyValueToSystem_Implementation() {
	const FIntPoint NewRes = UCustomSettingsUtils::ScreenResolutionStringToIntPoint(GetAppliedValue());
	GSystemResolution.RequestResolutionChange(NewRes.X, NewRes.Y, GSystemResolution.WindowMode);
}