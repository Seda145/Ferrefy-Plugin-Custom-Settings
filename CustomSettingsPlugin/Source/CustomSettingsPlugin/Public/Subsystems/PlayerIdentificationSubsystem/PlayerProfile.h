/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"

#include "PlayerProfile.generated.h"


USTRUCT()
struct CUSTOMSETTINGSPLUGIN_API FS_PlayerProfileSaveGameData {
	GENERATED_BODY()

	UPROPERTY()
		bool bSetupRan;

	/* Name of the profile. Must be unique. Characters must be alpha / digit / underscore. */
	UPROPERTY()
		FString ProfileName;

	FS_PlayerProfileSaveGameData()
		: bSetupRan (false)
		, ProfileName ("")
	{}
	FS_PlayerProfileSaveGameData(bool bInSetupRan, FString InProfileName)
		: bSetupRan (bInSetupRan)
		, ProfileName (InProfileName)
	{}

	bool operator==(const FS_PlayerProfileSaveGameData& Other) const {
		return (
			bSetupRan == Other.bSetupRan
			&& ProfileName == Other.ProfileName
		);
	}
};


UCLASS(BlueprintType)
class CUSTOMSETTINGSPLUGIN_API UPlayerProfile : public UObject
{
    GENERATED_BODY()

private:

	UPROPERTY(SaveGame)
		bool bSetupRan = false;

	UPROPERTY(SaveGame)
		FString ProfileName = "";

protected:

public:

private:

protected:

public:

	// Setup

	bool SetupOnCall(FS_PlayerProfileSaveGameData InSaveGameData);

	bool SetupOnCall(FString InProfileName);

	// PlayerProfile

	/* Get the name for this profile. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "PlayerProfile")
		FString GetProfileName() const;

	/* Sanitize input for a profile name. Returns true is sanitized name is valid. */
	UFUNCTION(BlueprintCallable, Category = "PlayerProfile")
		static bool SanitizeStringAsProfileName(const FString& InProfileName, FString& OutProfileName);

	//Saveg

	const FS_PlayerProfileSaveGameData GetSaveGameData() const;

};

