/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "LockFPSSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Engine.h"


FS_SettingSolverSettingRegistration ULockFPSSetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingLockFPS,
		{
			{UCustomSettingsUtils::ValueUnlimited},
			{UCustomSettingsUtils::Value30},
			{UCustomSettingsUtils::Value60},
			{UCustomSettingsUtils::Value90},
			{UCustomSettingsUtils::Value120},
			{UCustomSettingsUtils::Value240}
		},
		true
	};
}

FString ULockFPSSetting::GetSaveModeValue_Implementation() const {
	return UCustomSettingsUtils::ValueUnlimited;
}

bool ULockFPSSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	if (GEngine == nullptr) {
		return false;
	}

	const int32 MaxFPS = GEngine->GetMaxFPS();
	if (GetAppliedValue() == UCustomSettingsUtils::ValueUnlimited) {
		return MaxFPS == 0;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::Value30) {
		return MaxFPS == 30;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::Value60) {
		return MaxFPS == 60;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::Value90) {
		return MaxFPS == 90;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::Value120) {
		return MaxFPS == 120;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::Value240) {
		return MaxFPS == 240;
	}
	else {
		unimplemented();
	}

	return false;
}

void ULockFPSSetting::ApplyValueToSystem_Implementation() {
	check(GEngine != nullptr);

	float NewLimit = 0;
	if (GetAppliedValue() == UCustomSettingsUtils::ValueUnlimited) {
		NewLimit = 0;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::Value30) {
		NewLimit = 30;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::Value60) {
		NewLimit = 60;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::Value90) {
		NewLimit = 90;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::Value120) {
		NewLimit = 120;
	}
	else if (GetAppliedValue() == UCustomSettingsUtils::Value240) {
		NewLimit = 240;
	}
	else {
		unimplemented();
	}

	GEngine->SetMaxFPS(NewLimit);
}