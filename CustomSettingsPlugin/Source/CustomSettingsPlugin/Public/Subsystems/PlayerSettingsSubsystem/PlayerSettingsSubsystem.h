/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "CustomSettingSolverInfo.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Templates/SharedPointer.h"

#include "PlayerSettingsSubsystem.generated.h"

class UCustomSetting;
class UCustomSliderSetting;
class UPlayerProfile;
class ULocalPlayer;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerSettingsSubsystemEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBenchmarkRan, float, CPUPerformanceIndex, float, GPUPerformanceIndex);


/**
* This subsystem is created per player, and manages player settings.
* Sometimes also referred to as the "setting solver"
* , this subsystem registers settings and information provided by them to solve and inject complex setting behavior (requirements, incompatibilities) within this subsystem. 
* It uses a player's assigned player profile to load / save setting values, making it possible to personalize settings.
* It is not aware of other player's their subsystem. Only player 1 should apply / save settings with system global effects.
*/
UCLASS()
class CUSTOMSETTINGSPLUGIN_API UPlayerSettingsSubsystem : public ULocalPlayerSubsystem {
    GENERATED_BODY()

private:

	/* Registered setting objects, optionally with solver instructions. */
	UPROPERTY(Transient)
		FS_SettingSolverInfo SolverInfo;

	/* The profile used in the INI section for the setting system. Usable if set when a player is assigned to a profile. */
	UPROPERTY(Transient)
		FString SettingINIProfile = "";

	UPROPERTY(Transient)
		bool bIsApplyingSetting = false;

	UPROPERTY(Transient)
		bool bHasRegisteredAllSettings = false;	
		
	UPROPERTY(Transient)
		bool bHasFinishedInitialApplySettings = false;

protected:

public:

	// Delegates

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FOnPlayerSettingsSubsystemEvent OnRegisteredAllSettings;	
		
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FOnPlayerSettingsSubsystemEvent OnFinishedInitialApplySettings;	
		
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FOnBenchmarkRan OnBenchmarkRan;

private:

	// Setting solver

	const FS_SettingSolverInfo& GetSolverInfo() const;

	void RegisterSettings();

	void RunSolverTests() const;

	void SetIsApplyingSetting(bool bInIsApplyingSetting);

	void InitialApplySettings();

	void NotifySettingObjectsOfNotCompatibleChange(const FString& InSetting, const FString& InOldValue, const FString& InNewValue);

	bool FindSetBranches(const FString& InSetting, const FString& InValue, FS_SettingSolverSimSettingBranch& OutRequiredBranch, TArray<FS_SettingSolverSimSettingBranch>& OutOptionalBranches, bool bInGenerateOptionalBranches) const;

	// INI

	FString GetSettingINI() const;

	FString GetSettingINIProfile() const;

	FString GetSettingINISection() const;

	FString GetSavedSettingValue(const FString& InSetting) const;

	void RemoveSavedSettingValuesByProfileName(const FString& InProfileName);
	
	// Delegates
	
	UFUNCTION()
		void ActOnPlayerAssignedToProfile(UPlayerProfile* InProfile, ULocalPlayer* InPlayer);

	UFUNCTION()
		void ActOnPlayerProfileRemoved(UPlayerProfile* InProfile);

protected:

public:

	// Setup

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	//virtual void Deinitialize() override;

	// Benchmark

	void RunBenchmark();

	// Setting solver

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Settings")
		bool GetIsApplyingSetting() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Settings")
		bool GetHasRegisteredAllSettings() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Settings")
		bool GetHasFinishedInitialApplySettings() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Settings")
		bool CanApplySetting(const FString& InSetting, const FString& InValue) const;

	/* Tries to apply the setting. Asserts if it can't be found. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
		bool TryApplySetting(const FString& InSetting, const FString& InValue);

	/* Applies save mode values to a setting, and additional settings which must in response. Asserts if one can't be found or can't be applied. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void ForceApplySaveModeToSettingAndAffected(const FString& InSetting);

	/* Find the setting info for a registered setting. Asserts on failure. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Settings")
		const FS_SettingSolverSettingInfo& FindSettingInfo(const FString& InSetting) const;

	/* If the value is registered on the solver, finds a value info for a registered setting. Asserts if the setting or value fails to be found. */
	const FS_SettingSolverValueInfo& FindSettingValueInfo(const FS_SettingSolverSettingInfo& InSetting, const FString& InValue) const;

	/* If the value is registered on the solver, finds a value info for a registered setting. Asserts if the setting or value fails to be found. */
	const FS_SettingSolverValueInfo& FindSettingValueInfo(const FString& InSetting, const FString& InValue) const;

	/* If the value is registered on the solver, finds a value info for a registered setting. Asserts if the setting fails to be found. */
	const FS_SettingSolverValueInfo* FindSettingValueInfoOptional(const FS_SettingSolverSettingInfo& InSetting, const FString& InValue) const;

	/* If the value is registered on the solver, finds a value info for a registered setting. Asserts if the setting fails to be found. */
	const FS_SettingSolverValueInfo* FindSettingValueInfoOptional(const FString& InSetting, const FString& InValue) const;

	/* Finds a setting object for a registered setting. Asserts if it is not valid. */
	UCustomSetting* FindSettingObject(const FS_SettingSolverSettingInfo& InSetting) const;

	/* Finds a setting object for a registered setting. Asserts if it is not valid. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Settings")
		UCustomSetting* FindSettingObject(const FString& InSetting) const;

	/* Returns a map of setting > value, not compatible with InSetting > InValue. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Settings")
		TMap<FString, FString> FindNotCompatibleSettings(const FString& InSetting, const FString& InValue) const;

};