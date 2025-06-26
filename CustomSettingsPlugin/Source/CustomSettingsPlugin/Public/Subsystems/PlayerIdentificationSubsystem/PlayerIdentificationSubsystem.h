/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "PlayerProfile.h"
#include "GameFramework/SaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "PlayerIdentificationSubsystem.generated.h"

class ULocalPlayer;


UCLASS()
class CUSTOMSETTINGSPLUGIN_API UPlayerProfilesSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
		TArray<FS_PlayerProfileSaveGameData> PlayerProfilesData;

};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerProfileEvent, UPlayerProfile*, PlayerProfile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerAssignmentEvent, UPlayerProfile*, PlayerProfile, ULocalPlayer*, Player);

UCLASS()
class CUSTOMSETTINGSPLUGIN_API UPlayerIdentificationSubsystem : public UGameInstanceSubsystem {
    GENERATED_BODY()

private:

	const FString ProfilesSaveGameName = "Profiles";

	/* Contains profiles of players and profiles used by the system. */
	UPROPERTY()
		TArray<UPlayerProfile*> Profiles;

	UPROPERTY()
		TMap<UPlayerProfile*, ULocalPlayer*> ProfileAssignments;

protected:

public:

	// Delegates

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FOnPlayerProfileEvent OnProfileAdded;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FOnPlayerProfileEvent OnProfileRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FOnPlayerAssignmentEvent OnPlayerAssignedToProfile;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FOnPlayerAssignmentEvent OnPlayerUnAssignedFromProfile;

private:

	// Delegates | GameInstance

	UFUNCTION()
		void ActOnLocalPlayerAdded(ULocalPlayer* InPlayer);

	UFUNCTION()
		void ActOnLocalPlayerRemoved(ULocalPlayer* InPlayer);

	// Load or save profiles
	bool LoadProfiles();
	bool SaveProfiles();

protected:

public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	// Profiles

	/* Add a new profile identified by name. Returns nullptr if it failed. Profile may not already exist. Profile name must be sanitized successfully beforehand or during this function (allowed alpha, numeric and underscore chars), the word Default is not allowed. */
	UFUNCTION(BlueprintCallable, Category = "Profiles")
		UPlayerProfile* AddProfile(const FString& InProfileName);

	/* Remove a profile by name. Returns false if it failed. Profile must exist and not be assigned */
	UFUNCTION(BlueprintCallable, Category = "Profiles")
		bool RemoveProfile(UPlayerProfile* InProfile);

	/* Assign a profile to a player. Returns false if it failed. Profile must exist and not already be assigned. Player must not already be assigned. */
	UFUNCTION(BlueprintCallable, Category = "Profiles")
		bool AssignProfileToPlayer(UPlayerProfile* InProfile, ULocalPlayer* InPlayer);

	/* Unassign a player profile. Returns false if it failed. Profile must exist and be assigned to any player. */
	UFUNCTION(BlueprintCallable, Category = "Profiles")
		bool UnAssignProfile(UPlayerProfile* InProfile);

	/* Get the profile assigned to this player. Returns nullptr if it fails. Player must be assigned to a profile. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Profiles")
		UPlayerProfile* GetProfileAssignedToPlayer(const ULocalPlayer* InPlayer) const;

	/* Returns true if InPlayerProfile is valid and assigned to any player. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Profiles")
		bool IsPlayerAssignedToAnyProfile(const ULocalPlayer* InPlayer) const;

	/* Get the player assigned to this profile. Returns nullptr if it fails. Profile must be assigned. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Profiles")
		ULocalPlayer* GetPlayerAssignedToProfile(const UPlayerProfile* InProfile) const;

	/* Returns true if InPlayerProfile is valid and assigned to any player. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Profiles")
		bool IsProfileAssignedToAnyPlayer(const UPlayerProfile* InProfile) const;

	/* Returns true if all players are currently assigned to a profile. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Profiles")
		bool AreAllPlayersAssignedToAProfile() const;

	/* Get a profile by its name. Returns nullptr if it failed. Profile must exist and input must be valid. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Profiles")
		UPlayerProfile* GetProfileByName(const FString& InProfileName) const;

	/* Get all existing profiles. Optionally include system profiles. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Profiles")
		const TArray<UPlayerProfile*>& GetProfiles() const;

};

