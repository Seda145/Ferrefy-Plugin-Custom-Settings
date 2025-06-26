/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "WeatherVolumeSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Templates/SharedPointer.h"
#include "Sound/SoundClass.h"
#include "UObject/ConstructorHelpers.h"


// Setup

UWeatherVolumeSetting::UWeatherVolumeSetting() {
	ConstructorHelpers::FObjectFinderOptional<USoundClass> Obj(TEXT("/CustomSettingsPlugin/SoundClasses/SCLASS_Weather.SCLASS_Weather"));
	if (Obj.Succeeded()) {
		SoundClass = Obj.Get();
	}
	else {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Could not find asset for setting's sound class."));
	}
}

// Setting

FS_SettingSolverSettingRegistration UWeatherVolumeSetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingWeatherVolume,
		{},
		true
	};
}

FString UWeatherVolumeSetting::GetSaveModeValue_Implementation() const {
	return TEXT("1");
}

bool UWeatherVolumeSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	if (!IsValid(SoundClass)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid sound class."));
		return false;
	}

	if (FMath::IsNearlyEqual(UCustomSettingsUtils::StringToFloat(GetAppliedValue()), SoundClass->Properties.Volume)) {
		return true;
	}

	return false;
}

void UWeatherVolumeSetting::ApplyValueToSystem_Implementation() {
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

float UWeatherVolumeSetting::GetStepSize_Implementation() const {
	return 0.1f;
}

float UWeatherVolumeSetting::GetMinValue_Implementation() const {
	return 0.f;
}

float UWeatherVolumeSetting::GetMaxValue_Implementation() const {
	return 1.f;
}