/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "LanguageSetting.h"
#include "CustomSettingsUtils.h"
#include "LocalizationUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "Kismet/KismetInternationalizationLibrary.h"


FS_SettingSolverSettingRegistration ULanguageSetting::GetSolverRegistrationInfo_Implementation() const {
	FS_SettingSolverSettingRegistration Regist = {
		UCustomSettingsUtils::SettingLanguage,
		{},
		true
	};
	
	for (const FString& LockX : FTextLocalizationManager::Get().GetLocalizedCultureNames(ELocalizationLoadFlags::Game)) {
		Regist.ValueRegistrations.Add(LockX);
	}

	return Regist;
}

FString ULanguageSetting::GetSaveModeValue_Implementation() const {
	return ULocalizationUtils::FindPreferredLocalizedDefaultLocale();
}

bool ULanguageSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	return UKismetInternationalizationLibrary::GetCurrentLanguage() == GetAppliedValue();
}

void ULanguageSetting::ApplyValueToSystem_Implementation() {
	UKismetInternationalizationLibrary::SetCurrentLanguage(GetAppliedValue());
}
