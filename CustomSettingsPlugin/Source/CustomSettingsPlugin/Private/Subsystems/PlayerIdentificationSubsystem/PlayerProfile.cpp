/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "PlayerProfile.h"
#include "LogCustomSettingsPlugin.h"
 

// Setup

bool UPlayerProfile::SetupOnCall(FS_PlayerProfileSaveGameData InSaveGameData) {
	return SetupOnCall(InSaveGameData.ProfileName);
}

bool UPlayerProfile::SetupOnCall(FString InProfileName) {
	if (bSetupRan) {
		UE_LOG(LogCustomSettingsPlugin, Warning, TEXT("SetupOnCall, can not run twice."));
		return false;
	}

	FString SanitizedProfileName = "";
	if (!SanitizeStringAsProfileName(InProfileName, SanitizedProfileName)) {
		return false;
	}
	ProfileName = SanitizedProfileName;

	bSetupRan = true;
	return true;
}

// PlayerProfile

FString UPlayerProfile::GetProfileName() const {
	return ProfileName;
}

bool UPlayerProfile::SanitizeStringAsProfileName(const FString& InProfileName, FString& OutProfileName) {
	FString SanitizedString = "";

	// This string is sanitized so that it can be used as an INI section.

	for (const TCHAR& CharX : InProfileName.GetCharArray()) {
		if (FChar::IsAlpha(CharX)
			|| FChar::IsDigit(CharX)
			|| FChar::IsUnderscore(CharX)
			) {
			SanitizedString.AppendChar(CharX);
		}
	}

	OutProfileName = SanitizedString;

	if (OutProfileName.IsEmpty()) {
		UE_LOG(LogCustomSettingsPlugin, Warning, TEXT("Sanitized profile name is empty."));
		return false;
	}

	return true;
}

// SaveGame

const FS_PlayerProfileSaveGameData UPlayerProfile::GetSaveGameData() const {
	// No validation here. If the setup failed we should not use this object at all.
	check(bSetupRan);
	return FS_PlayerProfileSaveGameData(
		bSetupRan
		, GetProfileName()
	);
}