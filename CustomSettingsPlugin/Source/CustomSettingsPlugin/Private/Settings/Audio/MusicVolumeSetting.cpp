/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "MusicVolumeSetting.h"
#include "LogCustomSettingsPlugin.h"
#include "CustomSettingsUtils.h"
#include "Templates/SharedPointer.h"
#include "Sound/SoundClass.h"
#include "UObject/ConstructorHelpers.h"


// Setup

UMusicVolumeSetting::UMusicVolumeSetting() {
	ConstructorHelpers::FObjectFinderOptional<USoundClass> Obj(TEXT("/CustomSettingsPlugin/SoundClasses/SCLASS_Music.SCLASS_Music"));
	if (Obj.Succeeded()) {
		SoundClass = Obj.Get();
	}
	else {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Could not find asset for setting's sound class."));
	}
}

// Setting

FS_SettingSolverSettingRegistration UMusicVolumeSetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingMusicVolume,
		{},
		true
	};
}

FString UMusicVolumeSetting::GetSaveModeValue_Implementation() const {
	return TEXT("1");
}

bool UMusicVolumeSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	if (!IsValid(SoundClass)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid sound class."));
		return false;
	}

	if (FMath::IsNearlyEqual(UCustomSettingsUtils::StringToFloat(GetAppliedValue()), SoundClass->Properties.Volume)) {
		return true;
	}

	return false;
}

void UMusicVolumeSetting::ApplyValueToSystem_Implementation() {
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

float UMusicVolumeSetting::GetStepSize_Implementation() const {
	return 0.1f;
}

float UMusicVolumeSetting::GetMinValue_Implementation() const {
	return 0.f;
}

float UMusicVolumeSetting::GetMaxValue_Implementation() const {
	return 1.f;
}