/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "VoiceVolumeSetting.h"
#include "LogCustomSettingsPlugin.h"
#include "Templates/SharedPointer.h"
#include "Sound/SoundClass.h"
#include "CustomSettingsUtils.h"
#include "UObject/ConstructorHelpers.h"


// Setup

UVoiceVolumeSetting::UVoiceVolumeSetting() {
	ConstructorHelpers::FObjectFinderOptional<USoundClass> Obj(TEXT("/CustomSettingsPlugin/SoundClasses/SCLASS_Voice.SCLASS_Voice"));
	if (Obj.Succeeded()) {
		SoundClass = Obj.Get();
	}
	else {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Could not find asset for setting's sound class."));
	}
}

// Setting

FS_SettingSolverSettingRegistration UVoiceVolumeSetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingVoiceVolume,
		{},
		true
	};
}

FString UVoiceVolumeSetting::GetSaveModeValue_Implementation() const {
	return TEXT("1");
}

bool UVoiceVolumeSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	if (!IsValid(SoundClass)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid sound class."));
		return false;
	}

	if (FMath::IsNearlyEqual(UCustomSettingsUtils::StringToFloat(GetAppliedValue()), SoundClass->Properties.Volume)) {
		return true;
	}

	return false;
}

void UVoiceVolumeSetting::ApplyValueToSystem_Implementation() {
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

float UVoiceVolumeSetting::GetStepSize_Implementation() const {
	return 0.1f;
}

float UVoiceVolumeSetting::GetMinValue_Implementation() const {
	return 0.f;
}

float UVoiceVolumeSetting::GetMaxValue_Implementation() const {
	return 1.f;
}