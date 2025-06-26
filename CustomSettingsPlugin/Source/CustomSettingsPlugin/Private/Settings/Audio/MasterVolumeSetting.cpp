/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "MasterVolumeSetting.h"
#include "LogCustomSettingsPlugin.h"
#include "Templates/SharedPointer.h"
#include "CustomSettingsUtils.h"
#include "Sound/SoundClass.h"
#include "UObject/ConstructorHelpers.h"


// Setup

UMasterVolumeSetting::UMasterVolumeSetting() {
	ConstructorHelpers::FObjectFinderOptional<USoundClass> Obj(TEXT("/CustomSettingsPlugin/SoundClasses/SCLASS_Master.SCLASS_Master"));
	if (Obj.Succeeded()) {
		SoundClass = Obj.Get();
	}
	else {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Could not find asset for setting's sound class."));
	}
}

// Setting

FS_SettingSolverSettingRegistration UMasterVolumeSetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingMasterVolume,
		{},
		true
	};
}

FString UMasterVolumeSetting::GetSaveModeValue_Implementation() const {
	return TEXT("1");
}

bool UMasterVolumeSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	if (!IsValid(SoundClass)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid sound class."));
		return false;
	}

	if (FMath::IsNearlyEqual(UCustomSettingsUtils::StringToFloat(GetAppliedValue()), SoundClass->Properties.Volume)) {
		return true;
	}

	return false;
}

void UMasterVolumeSetting::ApplyValueToSystem_Implementation() {
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

float UMasterVolumeSetting::GetStepSize_Implementation() const {
	return 0.1f;
}

float UMasterVolumeSetting::GetMinValue_Implementation() const {
	return 0.f;
}

float UMasterVolumeSetting::GetMaxValue_Implementation() const {
	return 1.f;
}