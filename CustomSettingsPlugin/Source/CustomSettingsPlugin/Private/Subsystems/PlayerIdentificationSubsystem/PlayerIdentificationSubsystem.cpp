/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "PlayerIdentificationSubsystem.h"
#include "LogCustomSettingsPlugin.h"
#include "CustomSettingsUtils.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "Engine/Engine.h"
#include "Misc/ConfigCacheIni.h"


// USubsystem
void UPlayerIdentificationSubsystem::Initialize(FSubsystemCollectionBase &Collection) {
	Super::Initialize(Collection);

	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Initializing."));

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	check(IsValid(GI));

	LoadProfiles();

	// Bind to when GameInstance adds or removes a player.
	GI->OnLocalPlayerAddedEvent.AddUObject(this, &UPlayerIdentificationSubsystem::ActOnLocalPlayerAdded);
	GI->OnLocalPlayerRemovedEvent.AddUObject(this, &UPlayerIdentificationSubsystem::ActOnLocalPlayerRemoved);

	// ? todo can this happen at this point? If a player has already been added, process them.
	for (ULocalPlayer* PlayerX : GI->GetLocalPlayers()) {
		ActOnLocalPlayerAdded(PlayerX);
	}
}
void UPlayerIdentificationSubsystem::Deinitialize() {
	Super::Deinitialize();
}

// Profiles

UPlayerProfile* UPlayerIdentificationSubsystem::AddProfile(const FString& InProfileName) {
	// Sanitize the name to only acceptible characters, compatible with ini syntax as well.
	FString SanitizedProfileName = "";
	if (!UPlayerProfile::SanitizeStringAsProfileName(InProfileName, SanitizedProfileName)) {
		UE_LOG(LogCustomSettingsPlugin, Warning, TEXT("Failed, InProfileName %s could not be sanitized"), *InProfileName);
		return nullptr;
	}

	UPlayerProfile* Profile = GetProfileByName(SanitizedProfileName);
	if (IsValid(Profile)) {
		UE_LOG(LogCustomSettingsPlugin, Warning, TEXT("Failed, Profile %s already exists."), *SanitizedProfileName);
		return nullptr;
	}

	// At this point, it was not found and can be added.
	Profile = NewObject<UPlayerProfile>(this);
	if (!Profile->SetupOnCall(InProfileName)) {
		UE_LOG(LogCustomSettingsPlugin, Warning, TEXT("Failed, Profile %s could not be set up."), *SanitizedProfileName);
		// Do nothing with the profile, let GC handle it.
		return nullptr;
	}

	Profiles.Add(Profile);
	SaveProfiles();
	OnProfileAdded.Broadcast(Profile);
	return Profile;
}

bool UPlayerIdentificationSubsystem::RemoveProfile(UPlayerProfile* InProfile) {
	if (!IsValid(InProfile)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Failed, PlayerProfile is invalid."));
		return false;
	}
	if (!Profiles.Contains(InProfile)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Failed, PlayerProfile does not exist: %s"), *InProfile->GetProfileName());
		return false;
	}
	if (ProfileAssignments.Contains(InProfile)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Failed, Can not remove a profile which is assigned to a player: %s"), *InProfile->GetProfileName());
		return false;
	}

	if (Profiles.Remove(InProfile) > 0) {
		SaveProfiles();
		OnProfileRemoved.Broadcast(InProfile);
		return true;
	}

	return false;
}

bool UPlayerIdentificationSubsystem::AssignProfileToPlayer(UPlayerProfile* InProfile, ULocalPlayer* InPlayer) {
	if (!IsValid(InProfile) || !IsValid(InPlayer)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Failed. InPlayerProfile or InPlayer invalid."));
		return false;
	}
	if (IsProfileAssignedToAnyPlayer(InProfile)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Failed. InProfile is already assigned to a player: %s"), *InProfile->GetProfileName());
		return false;
	}

	if (IsPlayerAssignedToAnyProfile(InPlayer)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Failed. InPlayer is already assigned to a profile, unassign it first: %s"), *InProfile->GetProfileName());
		return false;
	}

	// If we reach this point, the profile exists, is not assigned and the player has not been assigned to any profile.
	ProfileAssignments.Add(InProfile, InPlayer);

	if (GConfig) {
		// Save the last used profile name in the INI so we can use it as the default next time we start the application.
		FString LastUsedProfileEntry = UCustomSettingsUtils::SettingLastUsedCustomSettingsProfileP0;
		switch (InPlayer->GetLocalPlayerIndex()) {
			case (0):
				LastUsedProfileEntry = UCustomSettingsUtils::SettingLastUsedCustomSettingsProfileP0;
				break;
			case (1):
				LastUsedProfileEntry = UCustomSettingsUtils::SettingLastUsedCustomSettingsProfileP1;
				break;
			case (2):
				LastUsedProfileEntry = UCustomSettingsUtils::SettingLastUsedCustomSettingsProfileP2;
				break;
			case (3):
				LastUsedProfileEntry = UCustomSettingsUtils::SettingLastUsedCustomSettingsProfileP3;
				break;
			default:
				unimplemented();
		}
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("For player index: %d, found last used profile in ini: %s"), InPlayer->GetLocalPlayerIndex(), *LastUsedProfileEntry);
		GConfig->SetString(*UCustomSettingsUtils::SectionCustomSettingsProfile, *LastUsedProfileEntry, *InProfile->GetProfileName(), GGameUserSettingsIni);
		GConfig->Flush(true, GGameUserSettingsIni);
	}

	UE_LOG(LogCustomSettingsPlugin, Verbose, TEXT("Assigned profile: %s, To player: %d"), *InProfile->GetProfileName(), InPlayer->GetLocalPlayerIndex());
	OnPlayerAssignedToProfile.Broadcast(InProfile, InPlayer);
	return true;
}

bool UPlayerIdentificationSubsystem::UnAssignProfile(UPlayerProfile* InProfile) {
	if (!IsValid(InProfile) || !IsProfileAssignedToAnyPlayer(InProfile)) {
		UE_LOG(LogCustomSettingsPlugin, Verbose, TEXT("InProfile invalid or not assigned to any player."));
		return false;
	}

	// At this point the profile exists and was present in the assignment map, so it can be removed.
	ULocalPlayer* Player = ProfileAssignments.FindAndRemoveChecked(InProfile);
	OnPlayerUnAssignedFromProfile.Broadcast(InProfile, Player);
	UE_LOG(LogCustomSettingsPlugin, Verbose, TEXT("UnAssigned profile: %s, From player: %d"), *InProfile->GetProfileName(), Player->GetLocalPlayerIndex());
	return true;
}

UPlayerProfile* UPlayerIdentificationSubsystem::GetProfileAssignedToPlayer(const ULocalPlayer* InPlayer) const {
	if (!IsValid(InPlayer)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("InPlayer invalid."));
		return nullptr;
	}

	for (const TPair<UPlayerProfile*, ULocalPlayer*>& pair : ProfileAssignments) {
		if (pair.Value == InPlayer) {
			// found the profile
			return pair.Key;
		}
	}

	UE_LOG(LogCustomSettingsPlugin, Verbose, TEXT("No assigned profile for this player."));
	return nullptr;
}

bool UPlayerIdentificationSubsystem::IsPlayerAssignedToAnyProfile(const ULocalPlayer* InPlayer) const {
	return GetProfileAssignedToPlayer(InPlayer) != nullptr;
}

ULocalPlayer* UPlayerIdentificationSubsystem::GetPlayerAssignedToProfile(const UPlayerProfile* InProfile) const {
	if (!IsValid(InProfile)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Failed, invalid InProfile."));
		return nullptr;
	}
	ULocalPlayer* const* LocalPlayer = ProfileAssignments.Find(InProfile);
	if (LocalPlayer) {
		return *LocalPlayer;
	}
	return nullptr;
}

bool UPlayerIdentificationSubsystem::IsProfileAssignedToAnyPlayer(const UPlayerProfile* InProfile) const {
	return GetPlayerAssignedToProfile(InProfile) != nullptr;
}

bool UPlayerIdentificationSubsystem::AreAllPlayersAssignedToAProfile() const {
	const UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (IsValid(GI)) {
		for (const ULocalPlayer* PlayerX : GI->GetLocalPlayers()) {
			if (!IsPlayerAssignedToAnyProfile(PlayerX)) {
				return false;
			}
		}
		return true;
	}
	return false;
}

UPlayerProfile* UPlayerIdentificationSubsystem::GetProfileByName(const FString& InProfileName) const {
	UPlayerProfile* MatchingProfile = nullptr;
	for (UPlayerProfile* ProfileX : Profiles) {
		if (ProfileX->GetProfileName() == InProfileName) {
			MatchingProfile = ProfileX;
			break;
		}
	}

	if (!IsValid(MatchingProfile)) {
		UE_LOG(LogCustomSettingsPlugin, Verbose, TEXT("No matching profile was found: %s"), *InProfileName);
		return nullptr;
	}

	return MatchingProfile;
}

const TArray<UPlayerProfile*>& UPlayerIdentificationSubsystem::GetProfiles() const {
	return Profiles;
}

bool UPlayerIdentificationSubsystem::LoadProfiles() {
	// ? Is this index OK for consoles and general use?
	const int32 UserIndex = 0;
	if (!UGameplayStatics::DoesSaveGameExist(ProfilesSaveGameName, UserIndex)) {
		UE_LOG(LogCustomSettingsPlugin, Verbose, TEXT("Profiles savegame slot %s does not (yet) exist."), *ProfilesSaveGameName);
		return false;
	}

	const UPlayerProfilesSaveGame* ProfilesSaveGame = Cast<UPlayerProfilesSaveGame>(UGameplayStatics::LoadGameFromSlot(ProfilesSaveGameName, UserIndex));;

	if (!IsValid(ProfilesSaveGame)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Failed, could not load SGProfiles but it does exist."));
		return false;
	}

	if (Profiles.Num() > 0) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Failed, Profiles are already loaded."));
		return false;
	}

	for (const FS_PlayerProfileSaveGameData& DataX : ProfilesSaveGame->PlayerProfilesData) {
		UPlayerProfile* ProfileX = NewObject<UPlayerProfile>(this);
		if (ProfileX->SetupOnCall(DataX)) {
			Profiles.Add(ProfileX);
		}
		else {
			UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Could not load invalid profile: %s."), *DataX.ProfileName);
		}
	}

	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Loaded profiles"));
	return true;
}

bool UPlayerIdentificationSubsystem::SaveProfiles() {
	UPlayerProfilesSaveGame* ProfilesSaveGame = Cast<UPlayerProfilesSaveGame>(UGameplayStatics::CreateSaveGameObject(UPlayerProfilesSaveGame::StaticClass()));

	if (!IsValid(ProfilesSaveGame)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Failed, ProfilesSaveGame is invalid."));
		return false;
	}

	for (const UPlayerProfile* ProfileX : Profiles) {
		ProfilesSaveGame->PlayerProfilesData.Add(ProfileX->GetSaveGameData());
	}

	if (!UGameplayStatics::SaveGameToSlot(ProfilesSaveGame, ProfilesSaveGameName, 0)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Failed, UGamePlayStatics::SaveGameToSlot failed."));
		return false;
	}

	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Saved profiles"));
	return true;
}

// Delegates | GameInstance

void UPlayerIdentificationSubsystem::ActOnLocalPlayerAdded(ULocalPlayer* InPlayer) {
	if (!IsValid(InPlayer)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("InPlayer is invalid."));
		return;
	}
	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("A player was detected added: %d"), InPlayer->GetLocalPlayerIndex());

	const UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (!IsValid(GI)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GI is invalid."));
		return;
	}
	if (IsPlayerAssignedToAnyProfile(InPlayer)) {
		// Irrelevant, we only need to restore to a profile if the player has none yet.
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("The player is already assigned to a profile."));
		return;	
	}

	// Attempt to restore the last used profile
	if (!GConfig) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GConfig is invalid."));
		return;
	}

	FString LastUsedProfile = "";
	FString LastUsedProfileEntry = UCustomSettingsUtils::SettingLastUsedCustomSettingsProfileP0;
	switch (InPlayer->GetLocalPlayerIndex()) {
	case (0):
		LastUsedProfileEntry = UCustomSettingsUtils::SettingLastUsedCustomSettingsProfileP0;
		break;
	case (1):
		LastUsedProfileEntry = UCustomSettingsUtils::SettingLastUsedCustomSettingsProfileP1;
		break;
	case (2):
		LastUsedProfileEntry = UCustomSettingsUtils::SettingLastUsedCustomSettingsProfileP2;
		break;
	case (3):
		LastUsedProfileEntry = UCustomSettingsUtils::SettingLastUsedCustomSettingsProfileP3;
		break;
	default:
		unimplemented();
	}
	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("For player index: %d, found last used profile in ini: %s"), InPlayer->GetLocalPlayerIndex(), *LastUsedProfileEntry);
	GConfig->GetString(*UCustomSettingsUtils::SectionCustomSettingsProfile, *LastUsedProfileEntry, LastUsedProfile, GGameUserSettingsIni);
	// Attempt to assign a last used profile, if there is any. Else do nothing.
	if (!LastUsedProfile.IsEmpty()) {
		AssignProfileToPlayer(GetProfileByName(LastUsedProfile), InPlayer);
	}
}

void UPlayerIdentificationSubsystem::ActOnLocalPlayerRemoved(ULocalPlayer* InPlayer) {
	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("A player was removed."));
	UnAssignProfile(GetProfileAssignedToPlayer(InPlayer));
}