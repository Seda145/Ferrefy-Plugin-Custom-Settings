/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "CustomSetting.h"
#include "CustomSettingsUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "PlayerSettingsSubsystem.h"


UPlayerSettingsSubsystem* UCustomSetting::GetPlayerSettingsSubsystem() const {
	return PlayerSettingsSubsystem;
}

void UCustomSetting::RegisterPlayerSettingsSubsystem(UPlayerSettingsSubsystem* InPlayerSettingsSubsystem) {
	// Don't do it twice. It's pointless post initialization by the UPlayerSettingsSubsystem.
	// This is the place to set up bindings to PlayerSettingsSubsystem once: (InPlayerSettingsSubsystem->OnBenchmarkRan etc.)
	check(PlayerSettingsSubsystem == nullptr);
	PlayerSettingsSubsystem = InPlayerSettingsSubsystem;
}

FS_SettingSolverSettingRegistration UCustomSetting::GetSolverRegistrationInfo_Implementation() const {
	// Template for copy paste into your settings:
	
	//return {
	//	// Setting Name
	//	"",
	//	// Value Registrations
	//	{ 
	//		{
	//			"FirstValue",
	//			{	// Not Compatible
	//				{"OtherSettingName", {{"OtherValue1", "OtherValue2"}}}
	//			},
	//			{	// Sets
	//				{"OtherSettingName", {"OtherValue", false}}
	//			},
	//			{	// SetBy
	//				{"OtherSettingName", {"OtherValue", false}}
	//			}
	//		}
	//	},
	//	// Causes global effect?
	//	false
	//};

	// Make sure to override.
	unimplemented();
	return FS_SettingSolverSettingRegistration();
}

FString UCustomSetting::GetSaveModeValue_Implementation() const {
	// Make sure to override.
	unimplemented();
	return FString();
}

FString UCustomSetting::GetAppliedValue() const {
	return AppliedValue;
}

bool UCustomSetting::HasAppliedValue() const {
	return !GetAppliedValue().IsEmpty();
}

bool UCustomSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	// Override if required (if applying a system effect). 
	return false;
}

bool UCustomSetting::IsValidValue_Implementation(const FString& InValue) const {
	// Override if required. 
	return !InValue.IsEmpty();
}

void UCustomSetting::ApplyValueToSystem_Implementation() {
	// Override if required. 
}

void UCustomSetting::CleanValue_Internal_Implementation() {
	// Override if needed.
}

void UCustomSetting::CleanValue() {
	check(IsValid(GetPlayerSettingsSubsystem()));
	check(!GetPlayerSettingsSubsystem()->GetHasFinishedInitialApplySettings());
	CleanValue_Internal();
	AppliedValue = "";
}

void UCustomSetting::ApplyValue(const FString& InValue) {
	check(IsValid(GetPlayerSettingsSubsystem()));
	if (!GetPlayerSettingsSubsystem()->GetIsApplyingSetting()) {
		// The only valid way to get to this method is through UPlayerSettingsSubsystem::TryApplySetting.
		// Any other way would bypass setting solver checks.
		// For convenience we can reroute a direct call to the solver instead of asserting here.
		GetPlayerSettingsSubsystem()->TryApplySetting(GetSolverRegistrationInfo().SettingName, InValue);
		return;
	}
	// You should have tested this already from the solver.
	if (!IsValidValue(InValue)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Setting: %s value %s is not a valid value."), *GetSolverRegistrationInfo().SettingName, *InValue);
		check(false);
	}

	AppliedValue = InValue;
	ApplyValueToSystem();
	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Setting: %s value %s has been applied."), *GetSolverRegistrationInfo().SettingName, *InValue);
	if (!DoesAppliedValueMatchSystemState()) {
		UE_LOG(LogCustomSettingsPlugin, Warning, TEXT("setting: %s Directly after application, DoesAppliedValueMatchSystemState() returns false. Inspect this if unexpected."), *GetSolverRegistrationInfo().SettingName); 
		// Can't return false here, for some engine / plugin settings do not offer tests. 
		// Other reason could be valid, like an application restart being required to finish application.
		// A warning is something to inspect if unexpected.
		// Thoughts: A failure in applying a setting Set (a chain of one setting applying another) should be detected solver side
		// , when CanApply changes to false halfway a chain. That would indicate a problem that possibly needs recovery to previous or save mode value.
	}
}

bool UCustomSetting::FindApplicableValueInDirection(bool bInGetNextValue, bool bInWrapAround, FString& OutValue) const {
	// This could happen in a valid situation (setting not applied yet, or player 2 doesn't apply global settings).
	if (!HasAppliedValue()) {
		return false;
	}

	const FString SettingName = GetSolverRegistrationInfo().SettingName;
	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Looking for applicable value for setting %s in direction %s, wrap around: %s"), *SettingName, *FString(bInGetNextValue ? "Next" : "Previous"), *FString(bInWrapAround ? "true" : "false"));
	OutValue = "";

	check(IsValid(GetPlayerSettingsSubsystem()));

	const FS_SettingSolverSettingInfo& SettingInfo = GetPlayerSettingsSubsystem()->FindSettingInfo(SettingName);
	if (SettingInfo.LockedBySettings.Num() > 0) {
		// To avoid looping this pointlessly, test if the setting is locked before we get started.
		for (const FString& LockX : SettingInfo.LockedBySettings) {
			UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("No applicable value, because the setting %s is locked by: %s"), *SettingName, *LockX);
		}
		return false;
	}

	// Get the keys to iterate and get the original index.
	int32 TestIndex = SettingInfo.ValueInfo.IndexOfByPredicate([this](const FS_SettingSolverValueInfo& ValInfoX) { return GetAppliedValue() == ValInfoX.ValueName; });
	check(TestIndex != INDEX_NONE);
	const int32 OriginalIndex = TestIndex;
	// Immediately get the previous or next index.
	TestIndex += (bInGetNextValue ? 1 : -1);

	// If we hit the original index, we made a complete wrap around the array (excluding original value) and nothing was applicable.
	while (TestIndex != OriginalIndex) {
		if (!SettingInfo.ValueInfo.IsValidIndex(TestIndex)) {
			if (!bInWrapAround) {
				// We hit array bounds but don't desire to wrap.
				UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("No applicable value found there for setting %s"), *SettingName);
				return false;
			}
			// Wrap around to the first or the last index on the value array, if the new TestIndex is not valid (we hit array bounds).
			TestIndex = bInGetNextValue ? 0 : SettingInfo.ValueInfo.Num() - 1;
		}

		const FString& TestKey = SettingInfo.ValueInfo[TestIndex].ValueName;
		if (GetPlayerSettingsSubsystem()->CanApplySetting(SettingName, TestKey)) {
			// All we need, we're done
			OutValue = TestKey;
			UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Applicable value found for setting %s, value %s"), *SettingName, *OutValue);
			return true;
		}
		else {
			// Value index was valid but the value could not be applied. Try a new index.
			TestIndex += (bInGetNextValue ? 1 : - 1);
		}
	}

	// Nothing in that direction is applicable.
	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("No applicable value found there for setting %s"), *SettingName);
	return false;
}

bool UCustomSetting::TryApplySettingInDirection(bool bInGetNextValue, bool bInWrapAround) {
	check(IsValid(GetPlayerSettingsSubsystem()));
	const FString SettingName = GetSolverRegistrationInfo().SettingName;
	FString OutValue = "";
	return FindApplicableValueInDirection(bInGetNextValue, bInWrapAround, OutValue) ? GetPlayerSettingsSubsystem()->TryApplySetting(SettingName, OutValue) : false;
}