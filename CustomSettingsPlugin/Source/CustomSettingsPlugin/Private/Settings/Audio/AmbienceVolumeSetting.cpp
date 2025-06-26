/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "AmbienceVolumeSetting.h"
#include "LogCustomSettingsPlugin.h"
#include "CustomSettingsUtils.h"
#include "Templates/SharedPointer.h"
#include "Sound/SoundClass.h"
#include "UObject/ConstructorHelpers.h"


// Setup

UAmbienceVolumeSetting::UAmbienceVolumeSetting() {
	ConstructorHelpers::FObjectFinderOptional<USoundClass> Obj(TEXT("/CustomSettingsPlugin/SoundClasses/SCLASS_Ambience.SCLASS_Ambience"));
	if (Obj.Succeeded()) {
		SoundClass = Obj.Get();
	}
	else {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Could not find asset for setting's sound class."));
	}
}

// Setting

FS_SettingSolverSettingRegistration UAmbienceVolumeSetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingAmbienceVolume,
		{},
		true
	};
}

FString UAmbienceVolumeSetting::GetSaveModeValue_Implementation() const {
	return TEXT("1");
}

bool UAmbienceVolumeSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	if (!IsValid(SoundClass)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid sound class."));
		return false;
	}

	if (FMath::IsNearlyEqual(UCustomSettingsUtils::StringToFloat(GetAppliedValue()), SoundClass->Properties.Volume)) {
		return true;
	}

	return false;
}

void UAmbienceVolumeSetting::ApplyValueToSystem_Implementation() {
	// Get the correct data type. Watch how GConfig->Get... does this too:
	float FloatValue = FCString::Atof(*GetAppliedValue());

	// Clamp the value
	FloatValue = FMath::Clamp(FloatValue, GetMinValue_Implementation(), GetMaxValue_Implementation());

	// Perform the setting's action.
	if (!IsValid(SoundClass)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid sound class."));
		return;
	}
	SoundClass->Properties.Volume = FloatValue;
}

float UAmbienceVolumeSetting::GetStepSize_Implementation() const {
	return 0.1f;
}

float UAmbienceVolumeSetting::GetMinValue_Implementation() const {
	return 0.f;
}

float UAmbienceVolumeSetting::GetMaxValue_Implementation() const {
	return 1.f;
}