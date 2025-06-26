/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "PlayerSettingsSubsystem.h"
#include "Engine/GameInstance.h"
#include "PlayerIdentificationSubsystem.h"
#include "CustomSettingsUtils.h"
#include "PlayerProfile.h"
#include "Engine/LocalPlayer.h"
#include "LogCustomSettingsPlugin.h"
#include "CustomSettingsPluginSettings.h"
#include "CustomSettingsPlayerInput.h"
#include "CustomSetting.h"
#include "CustomSliderSetting.h"
#include "CustomKeybindingSetting.h"
#include "SynthBenchmark.h"
#include "SubsystemUtils.h"
#include "GenericPlatform/GenericPlatformSurvey.h"
#include "Engine/World.h"
#include "Misc/ConfigCacheIni.h"


void UPlayerSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
	Super::Initialize(Collection);

	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Initializing."));

	UWorld* World = GetWorld();
	check(World != nullptr);

	// Register settings and check if all is valid before we start using the system.
	RegisterSettings();
	RunSolverTests();

	// Set up bindings related to player identification subsystem

	UPlayerIdentificationSubsystem* PIS = USubsystemUtils::GetGameInstanceSubsystem<UPlayerIdentificationSubsystem>(this);
	if (!IsValid(PIS)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("invalid UPlayerIdentificationSubsystem"));
		return;
	}

	PIS->OnProfileRemoved.AddDynamic(this, &UPlayerSettingsSubsystem::ActOnPlayerProfileRemoved);
	PIS->OnPlayerAssignedToProfile.AddDynamic(this, &UPlayerSettingsSubsystem::ActOnPlayerAssignedToProfile);
	ULocalPlayer* LP = GetLocalPlayer();
	check(IsValid(LP));
	if (PIS->IsPlayerAssignedToAnyProfile(LP)) {
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("A profile has been assigned to this player: %d already, so we continue setting up immediately."), LP->GetLocalPlayerIndex());
		ActOnPlayerAssignedToProfile(PIS->GetProfileAssignedToPlayer(LP), LP);
	}
	else {
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("No profile has been assigned yet to player: %d. Listening for this to change."), LP->GetLocalPlayerIndex());
	}
}

// Benchmark

void UPlayerSettingsSubsystem::RunBenchmark() {
	// Don't hijack Sets.
	check(!GetIsApplyingSetting());

	UE_LOG(LogCustomSettingsPlugin, Verbose, TEXT("running benchmark."));

	FSynthBenchmarkResults SynthBenchmark;
	ISynthBenchmark::Get().Run(SynthBenchmark, true, 10);
	TArray<float> OutCPUBenchmarkSteps;
	TArray<float> OutGPUBenchmarkSteps;
	OnBenchmarkRan.Broadcast(
		// An index of 100: avg good CPU / GPU, <100: slower, >100: faster.
		SynthBenchmark.ComputeCPUPerfIndex(&OutCPUBenchmarkSteps),
		SynthBenchmark.ComputeGPUPerfIndex(&OutGPUBenchmarkSteps)
	);
}

// Setting solver

const FS_SettingSolverInfo& UPlayerSettingsSubsystem::GetSolverInfo() const {
	return SolverInfo;
}

void UPlayerSettingsSubsystem::RegisterSettings() {
	// Don't register twice.
	check(GetHasRegisteredAllSettings() == false);

	const UCustomSettingsPluginSettings* CustomSettingsPluginSettings = GetDefault<UCustomSettingsPluginSettings>();
	check(IsValid(CustomSettingsPluginSettings));
	// Invalid if called another time.
	check(GetSolverInfo().Settings.Num() == 0);
	const int32 ClassNumberToRegister = CustomSettingsPluginSettings->CustomSettingClasses.Num();

	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("InitializeCustomSettings: registering all settings. Detected %d setting classes in UCustomSettingsPluginSettings."), ClassNumberToRegister);

	// Add all the settings.
	for (TSubclassOf<UCustomSetting> ClassX : CustomSettingsPluginSettings->CustomSettingClasses) {
		/**
		* Explanation of the solver structs (FS_SettingSolverSettingInfo):
		* This struct is used to register settings and their values.
		*
		* Struct explanation of compatibility modes:
		* - "NotCompatible"	: This value is not compatible with other value on other setting.
		*						Compatibility blacklist.
		*						This list is automatically updated to both settings, making extension by injection possible.
		* - "Sets"			: When this value is applied, it wants to apply another setting's value if it can.
		*						Optionally it must (bRequire). Both modes help to solve an incompatibility.
		*						A required Set will also be tested for compatibility.
		*						Sets are applied recursively.
		* - "SetBy"			: Sets are generated from this on the other setting, making extension by injection possible.
		*
		* A Save mode value may not have NotCompatible, or required Sets (or SetBy).
		*/

		// Build the info for this setting.
		check(IsValid(ClassX));
		UCustomSetting* NewSettingObj = NewObject<UCustomSetting>(this, ClassX);
		check(IsValid(NewSettingObj));
		const FS_SettingSolverSettingRegistration NewRegistrationInfo = NewSettingObj->GetSolverRegistrationInfo();
		const FString NewSettingName = NewRegistrationInfo.SettingName;

		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Starting solver setting registration, for setting name: %s"), *NewSettingName);
		check(!NewSettingName.IsEmpty());
		const TArray<FS_SettingSolverValueRegistration>& NewValueRegists = NewRegistrationInfo.ValueRegistrations;

		// Setting registration might already exist because we register NotCompatible in two ways.
		// Thoughts: So there is no way to check ahead if a NotCompatible is an actual value on an actual setting until both are registered. 
		// Assert on that later on then, comparing solver to registration info.
		FS_SettingSolverSettingInfo& NewInfo = SolverInfo.Settings.FindOrAdd(NewSettingName);
		// Ensure we are not fully registering the same setting name twice then here, by checking if SettingObject already exists.

		if (NewInfo.SettingObject != nullptr) {
			UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Setting object is already registered, for setting name: %s"), *NewSettingName);
			check(false);
		}
		NewInfo.SettingObject = NewSettingObj;
		NewInfo.SettingObject->RegisterPlayerSettingsSubsystem(this);
		NewInfo.bSettingDependsOnGlobalEffect = NewRegistrationInfo.bSettingDirectlyCausesGlobalEffect;

		for (const FS_SettingSolverValueRegistration& RegistX : NewValueRegists) {
			// Check we don't register the same value name twice in the registration info.
			check(NewValueRegists.FilterByPredicate([&RegistX](const FS_SettingSolverValueRegistration& RegistY) { return RegistX.ValueName == RegistY.ValueName; }).Num() == 1);
		}

		// Note that sliders usually don't register with value, since they can have so many combinations.
		// If you run into that question, you probably want to use a normal setting type (multi choice).

		for (const FS_SettingSolverValueInfo& ValInfoX : NewInfo.ValueInfo) {
			// If something already registered part of a value (Probably a NotCompatible two-way registration)
			// , then check if the value actually exists in the registration.
			check(NewValueRegists.ContainsByPredicate([&ValInfoX](const FS_SettingSolverValueRegistration& ValRegistY) { return ValInfoX.ValueName == ValRegistY.ValueName; }));
		}

		// Do basic registration of all values.
		// An annoyance here is that when another setting does a basic registration of a value for this setting, 
		// the order of the registered values becomes different from the order of the registration info we received just now.
		// Fix it.
		TArray<FS_SettingSolverValueInfo> ReplaceInfo;
		for (const FS_SettingSolverValueRegistration& RegistX : NewValueRegists) {
			ReplaceInfo.Add(RegistX.ValueName);
		}
		for (const FS_SettingSolverValueInfo& InfoX : NewInfo.ValueInfo) {
			// Already checked the value is in the registration, just get the index.
			const int32 Index = ReplaceInfo.IndexOfByKey(InfoX);
			ReplaceInfo[Index] = InfoX;
		}
		NewInfo.ValueInfo = ReplaceInfo;

		// Next up solver info is generated from the registration info.

		// Register incompatibilities into the solver, two-way.
		// The other setting might not have been registered yet.
		for (const FS_SettingSolverValueRegistration& RegistValueX : NewValueRegists) {
			for (const TPair<FString, FS_SettingSolverNotCompatibleValues>& RegistNotCompatX : RegistValueX.NotCompatible) {
				const FString& OtherSettingName = RegistNotCompatX.Key;
				for (const FString& OtherValueNameX : RegistNotCompatX.Value.Values) {
					// Register incompatibility of this setting with other setting and its value.
					FS_SettingSolverSettingInfo& SolverSettingX = SolverInfo.Settings.FindChecked(NewSettingName);
					FS_SettingSolverValueInfo& SolverValueX = SolverSettingX.ValueInfo[SolverSettingX.ValueInfo.AddUnique(RegistValueX.ValueName)];
					SolverValueX.NotCompatible.FindOrAdd(OtherSettingName).Values.Add(OtherValueNameX);
					// We update the incompatible setting next, so both get incompatibility with eachother.
					FS_SettingSolverSettingInfo& OtherSolverSettingX = SolverInfo.Settings.FindOrAdd(OtherSettingName);
					FS_SettingSolverValueInfo& OtherSolverValueX = OtherSolverSettingX.ValueInfo[OtherSolverSettingX.ValueInfo.AddUnique(OtherValueNameX)];
					OtherSolverValueX.NotCompatible.FindOrAdd(NewSettingName).Values.Add(RegistValueX.ValueName);
				}
			}
		}

		TMap<FString, TMap<FString, TMap<FString, FS_SettingSolverValueSetterBehavior>>> RegistGeneratedSets;
		// This is what the registered setting does
		for (const FS_SettingSolverValueRegistration& RegistValueX : NewValueRegists) {
			// Register this setting and its value.
			TMap<FString, TMap<FString, FS_SettingSolverValueSetterBehavior>>& ThisSetting = RegistGeneratedSets.FindOrAdd(NewSettingName);
			TMap<FString, FS_SettingSolverValueSetterBehavior>& ThisValue = ThisSetting.FindOrAdd(RegistValueX.ValueName);
			// This is what that makes another setting do
			for (const TPair<FString, FS_SettingSolverValueSetterBehavior>& RegistSetsX : RegistValueX.Sets) {
				// Register the other setting and its value
				FS_SettingSolverValueSetterBehavior& ThisSets = ThisValue.FindOrAdd(RegistSetsX.Key);
				ThisSets = RegistSetsX.Value;
				UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Generated Set, setting: %s, value: %s, sets other setting: %s, other value: %s, required?: %s"), *NewSettingName, *RegistValueX.ValueName, *RegistSetsX.Key, *ThisSets.ValueName, *FString(ThisSets.bRequire ? "True" : "False"));
			}

			// Entries in SetBy generate Sets for this setting's value.
			for (const TPair<FString, FS_SettingSolverValueSetterBehavior>& RegistSetByPairX : RegistValueX.SetBy) {
				// Register the other setting and its value
				TMap<FString, TMap<FString, FS_SettingSolverValueSetterBehavior>>& OtherSetting = RegistGeneratedSets.FindOrAdd(RegistSetByPairX.Key);
				TMap<FString, FS_SettingSolverValueSetterBehavior>& OtherValue = OtherSetting.FindOrAdd(RegistSetByPairX.Value.ValueName);
				// Register Sets on the other setting's values.
				FS_SettingSolverValueSetterBehavior& OtherSet = OtherValue.FindOrAdd(NewSettingName);
				// If already required, don't remove required.
				OtherSet.bRequire = OtherSet.bRequire || RegistSetByPairX.Value.bRequire;
				OtherSet.ValueName = RegistValueX.ValueName;
				UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Generated Set from SetBy. Setting %s value %s Sets setting %s value: %s. required?: %s"), *RegistSetByPairX.Key, *RegistSetByPairX.Value.ValueName, *NewSettingName, *OtherSet.ValueName, *FString(OtherSet.bRequire ? "True" : "False"));
			}
		}

		// Register generated Sets into the solver.
		// The other settings might not have been registered yet.
		// SettingX > ValueX > sets Setting Y > Value Y
		for (const TPair<FString, TMap<FString, TMap<FString, FS_SettingSolverValueSetterBehavior>>>& SettingX : RegistGeneratedSets) {
			FS_SettingSolverSettingInfo& SolverSettingX = SolverInfo.Settings.FindOrAdd(SettingX.Key);
			for (const TPair<FString, TMap<FString, FS_SettingSolverValueSetterBehavior>>& ValueX : SettingX.Value) {
				FS_SettingSolverValueInfo& SolverValueX = SolverSettingX.ValueInfo[SolverSettingX.ValueInfo.AddUnique(ValueX.Key)];
				for (const TPair<FString, FS_SettingSolverValueSetterBehavior>& SettingY : ValueX.Value) {
					SolverValueX.Sets.FindOrAdd(SettingY.Key) = SettingY.Value;
					UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Injecting Set into solver: setting: %s, value: %s, other setting: %s, other value: %s, required?: %s"), *SettingX.Key, *ValueX.Key, *SettingY.Key, *SettingY.Value.ValueName, *FString(SettingY.Value.bRequire ? "True" : "False"));
				}
			}
		}

		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Finished registration for the setting: %s"), *NewSettingName);
	}

	{
		// Now all settings are registered, I want to loop over the Sets recursively again 
		// so we can decide per setting if it applies a system wide effect through itself or a Set.
		for (TPair<FString, FS_SettingSolverSettingInfo>& SettingX : SolverInfo.Settings) {
			TSet<FString> VisitedSettings;
			auto TestSetsForAffectAllPlayers = [this, &VisitedSettings, &SettingX](auto&& TestSetsForAffectAllPlayers, const FString& InSetting) -> bool {
				TSet<FString> SettingsToCheck;
				// Guard against looping back and dupes.
				VisitedSettings.Add(InSetting);

				const FS_SettingSolverSettingInfo& SettingInfoY = FindSettingInfo(InSetting);
				if (SettingInfoY.bSettingDependsOnGlobalEffect) {
					UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Setting %s is detected as a dependency with global effect, for setting %s."), *InSetting, *SettingX.Key);
					return true;
				}

				for (const FS_SettingSolverValueInfo& ValInfoY : SettingInfoY.ValueInfo) {
					for (const TPair<FString, FS_SettingSolverValueSetterBehavior>& SetsY : ValInfoY.Sets) {
						if (!VisitedSettings.Contains(SetsY.Key)) {
							SettingsToCheck.Add(SetsY.Key);
						}
					}
				}

				for (const FString& SettingZ : SettingsToCheck) {
					if (TestSetsForAffectAllPlayers(TestSetsForAffectAllPlayers, SettingZ)) {
						return true;
					}
				}

				return false;
				};

			SettingX.Value.bSettingDependsOnGlobalEffect = TestSetsForAffectAllPlayers(TestSetsForAffectAllPlayers, SettingX.Key);
		}

		// Last we can also scan the NotCompatible references.
		// A NotCompatible setting which has / depends on a global effect, is also a global dependency.
		// This is because player 1 might set a global effect which the subsystem of player 2 is unaware of, making it impossible to run the solver for player 2.
		for (TPair<FString, FS_SettingSolverSettingInfo>& SettingX : SolverInfo.Settings) {
			bool bFoundDepend = false;
			for (FS_SettingSolverValueInfo& ValInfoX : SettingX.Value.ValueInfo) {
				if (bFoundDepend) {
					break;
				}
				for (const TPair<FString, FS_SettingSolverNotCompatibleValues>& NotCompatX : ValInfoX.NotCompatible) {
					const FS_SettingSolverSettingInfo& SettingInfoY = FindSettingInfo(NotCompatX.Key);
					if (SettingInfoY.bSettingDependsOnGlobalEffect) {
						SettingX.Value.bSettingDependsOnGlobalEffect = true;
						UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Setting %s is detected (as NotCompatible) as a dependency with global effect, for setting %s."), *NotCompatX.Key, *SettingX.Key);
						bFoundDepend = true;
						break;
					}
				}
			}
		}
	}

	if (GetSolverInfo().Settings.Num() != ClassNumberToRegister) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Registered number of settings is different from UCustomSettingsPluginSettings::CustomSettingClasses. This can happen if a setting refers to another which was not provided for registration."));
		for (const TPair<FString, FS_SettingSolverSettingInfo>& InfoX : GetSolverInfo().Settings) {
			// This indicates a partial registration.
			if (InfoX.Value.SettingObject == nullptr) {
				UE_LOG(LogCustomSettingsPlugin, Error, TEXT("This setting: %s was not registered. Only a partial registration info exists, which could be because another setting referred to it."), *InfoX.Key);
			}
		}
		check(false);
	}

	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("InitializeCustomSettings: Registered all settings."));
	bHasRegisteredAllSettings = true;
	OnRegisteredAllSettings.Broadcast();
}

bool UPlayerSettingsSubsystem::GetIsApplyingSetting() const {
	return bIsApplyingSetting;
}

bool UPlayerSettingsSubsystem::GetHasRegisteredAllSettings() const {
	return bHasRegisteredAllSettings;
}

bool UPlayerSettingsSubsystem::GetHasFinishedInitialApplySettings() const {
	return bHasFinishedInitialApplySettings;
}

void UPlayerSettingsSubsystem::SetIsApplyingSetting(bool bInIsApplyingSetting) {
	bIsApplyingSetting = bInIsApplyingSetting;
}

void UPlayerSettingsSubsystem::RunSolverTests() const {
	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Running setting solver tests."));

	check(IsValid(GetLocalPlayerChecked()));
	// For a player 2+, Settings which are managed by player 1 (with a global effect) are not processed by player 2+ at all.
	TMap<FString, FS_SettingSolverSettingInfo> RelevantSettings = GetSolverInfo().Settings;
	if (GetLocalPlayerChecked()->GetLocalPlayerIndex() > 0) {
		for (const TPair<FString, FS_SettingSolverSettingInfo>& SettingX : GetSolverInfo().Settings) {
			if (SettingX.Value.bSettingDependsOnGlobalEffect) {
				RelevantSettings.Remove(SettingX.Key);
			}
		}
	}

	for (const TPair<FString, FS_SettingSolverSettingInfo>& SettingX : RelevantSettings) {
		// Test for direct misconfigurations, which is a bad copy paste or tired mind when writing the solver registrations.
		for (const FS_SettingSolverValueInfo& ValueInfoX : SettingX.Value.ValueInfo) {
			for (const TPair<FString, FS_SettingSolverNotCompatibleValues>& NotCompatX : ValueInfoX.NotCompatible) {
				// Must not be incompatible with self.
				check(NotCompatX.Key != SettingX.Key);
			}
			for (const TPair<FString, FS_SettingSolverValueSetterBehavior>& SetX : ValueInfoX.Sets) {
				// Must not set itself.
				check(SetX.Key != SettingX.Key);
			}
		}
	}

	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("OK: No direct misconfiguration spotted on settings themselves."));

	for (const TPair<FString, FS_SettingSolverSettingInfo>& SettingX : RelevantSettings) {
		// First test if a setting's save mode value itself is safe at all.
		const UCustomSetting* SettingObjX = FindSettingObject(SettingX.Value);
		const FString SaveValX = SettingObjX->GetSaveModeValue();
		const FS_SettingSolverValueInfo* ValueInfoX = FindSettingValueInfoOptional(SettingX.Value, SaveValX);
		// I would make it required, but it's possible that the save mode value is generated and not registered constant (display resolution? etc.).
		if (ValueInfoX != nullptr) {
			// Make sure a save mode value is not incompatible at any moment.
			if (ValueInfoX->NotCompatible.Num() > 0) {
				UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Save mode value has NotCompatible registered, making it unsafe: setting %s value %s."), *SettingX.Key, *SaveValX);
				check(false);
			}
			for (const TPair<FString, FS_SettingSolverValueSetterBehavior>& SetBehX : ValueInfoX->Sets) {
				// A save mode value should not depend on a required value through a Set.
				if (SetBehX.Value.bRequire) {
					// ^ However, there is one situation this is actually valid.
					// 1. When the required value is a save mode value.
					// 2. AND no other setting can ever lock it to another value
					const UCustomSetting* OtherSettingObjX = FindSettingObject(SetBehX.Key);
					check(OtherSettingObjX->GetSaveModeValue() == SetBehX.Value.ValueName);
					for (const TPair<FString, FS_SettingSolverSettingInfo>& SettingY : RelevantSettings) {
						for (const FS_SettingSolverValueInfo& ValueY : SettingY.Value.ValueInfo) {
							const FS_SettingSolverValueSetterBehavior* SetBehY = ValueY.Sets.Find(SetBehX.Key);
							if (SetBehY != nullptr) {
								if (SetBehY->ValueName != OtherSettingObjX->GetSaveModeValue()) {
									// If this hits then another setting could lock SettingObjX out of save mode, so assert.
									if (SetBehY->bRequire) {
										UE_LOG(LogCustomSettingsPlugin, Error, TEXT("A save mode value is a required value, but it can be locked to another value by another setting. Detected setting %s value %s."), *SetBehX.Key, *SetBehY->ValueName);
										check(false);
									}
								}
							}
						}
					}
				}
			}
		}
		if (!CanApplySetting(SettingX.Key, SaveValX)) {
			UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Can't apply setting: %s save mode value: %s"), *SettingX.Key, *SaveValX);
			check(false);
		}
	}

	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("OK: Setting save mode values are fine (can be applied, no Sets, no NotCompatible)."));

	{
		for (const TPair<FString, FS_SettingSolverSettingInfo>& SettingX : RelevantSettings) {
			for (const FS_SettingSolverValueInfo& ValueInfoX : SettingX.Value.ValueInfo) {
				TMap<FString, TSet<FString>> RefNewChain = TMap<FString, TSet<FString>>();

				auto TestForInvalid = [this, &RefNewChain](auto&& TestForInvalid, const FString& InSetting, const FString& InValue) -> void {
					// If the requested setting + value is already in the chain, we got a branch causing infinite loop back into itself.
					TSet<FString>& ValuesInChain = RefNewChain.FindOrAdd(InSetting);
					if (ValuesInChain.Contains(InValue)) {
						UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Infinite loop detected at setting %s value %s."), *InSetting, *InValue);
						check(false);
					}
					ValuesInChain.Add(InValue);

					const FS_SettingSolverValueInfo* ValueInfoY = FindSettingValueInfoOptional(InSetting, InValue);
					if (ValueInfoY == nullptr) {
						// I want every value to be registered, but for a slider that is impossible (too many options).
						// However, consider that a slider already reports if a value is within bounds, and can still be locked.
						// If that is not enough, you want a normal setting (MultiChoice) instead of a slider.
						const UCustomSetting* const SettingObj = FindSettingObject(InSetting);
						if (Cast<UCustomSliderSetting>(SettingObj) == nullptr
							&& Cast<UCustomKeybindingSetting>(SettingObj) == nullptr
							) {
							UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Detected an unregistered value at setting %s value %s on the common setting type."), *InSetting, *InValue);
							check(false);
						}
					}
					else {
						if (ValueInfoY->Sets.Num() > 0) {
							for (const TPair<FString, FS_SettingSolverValueSetterBehavior>& SetsY : ValueInfoY->Sets) {
								TestForInvalid(TestForInvalid, SetsY.Key, SetsY.Value.ValueName);
								// With these loops we are reconstructing Set branches
								// , and for every loop the next part can be different.
								// For that reason remove the previous last added bit of the chain so that the new one can be tracked.
								if (ValuesInChain.Num() > 1) {
									RefNewChain.FindChecked(SetsY.Key).Remove(SetsY.Value.ValueName);
								}
								else {
									RefNewChain.Remove(SetsY.Key);
								}
							}
						}
					}
					};

				// Start testing for infinite loops on Sets, one by one.
				TestForInvalid(TestForInvalid, SettingX.Key, ValueInfoX.ValueName);
			}
		}

		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("OK: Setting 'Sets' do not result in an infinite loop."));
	}

	{
		for (const TPair<FString, FS_SettingSolverSettingInfo>& SettingX : RelevantSettings) {
			for (const FS_SettingSolverValueInfo& ValueInfoX : SettingX.Value.ValueInfo) {

				auto TestForInvalid = [this](auto&& TestForInvalid, const FString& InSetting, const FString& InValue) -> void {
					const FS_SettingSolverValueInfo* ValueInfoY = FindSettingValueInfoOptional(InSetting, InValue);
					if (ValueInfoY == nullptr) {
						// I want every value to be registered, but for a slider that is impossible (too many options).
						// However, consider that a slider already reports if a value is within bounds, and can still be locked.
						// If that is not enough, you want a normal setting (MultiChoice) instead of a slider.

						const UCustomSetting* const SettingObj = FindSettingObject(InSetting);
						if (Cast<UCustomSliderSetting>(SettingObj) == nullptr
							&& Cast<UCustomKeybindingSetting>(SettingObj) == nullptr
							) {
							UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Detected an unregistered value at setting %s value %s on the common setting type."), *InSetting, *InValue);
							check(false);
						}
					}
					else {
						for (const TPair<FString, FS_SettingSolverValueSetterBehavior>& SetsY : ValueInfoY->Sets) {
							TestForInvalid(TestForInvalid, SetsY.Key, SetsY.Value.ValueName);
						}
					}
					};

				TestForInvalid(TestForInvalid, SettingX.Key, ValueInfoX.ValueName);
			}
		}

		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("OK: If any Set value is not registered on the solver, it's on a slider and not on a common setting."));
	}

	{
		for (const TPair<FString, FS_SettingSolverSettingInfo>& SettingX : RelevantSettings) {
			for (const FS_SettingSolverValueInfo& ValueInfoX : SettingX.Value.ValueInfo) {
				TSet<FString> RefNewChain = TSet<FString>();

				const FString StartingSetting = SettingX.Key;
				const FString StartingValue = ValueInfoX.ValueName;

				auto TestForInvalid = [this, &RefNewChain, &StartingSetting, &StartingValue](auto&& TestForInvalid, const FString& InSetting, const FString& InValue) -> void {
					RefNewChain.Add(InSetting);

					const FS_SettingSolverValueInfo* ValueInfoY = FindSettingValueInfoOptional(InSetting, InValue);
					if (ValueInfoY != nullptr) {
						for (const TPair<FString, FS_SettingSolverValueSetterBehavior>& SetsY : ValueInfoY->Sets) {
							const FString* IsInChain = RefNewChain.Find(SetsY.Key);
							if (IsInChain != nullptr) {
								// The setting is configured as reentering into to another value (valid, but loop got to be cut when detected).
								// Example loop, a two way interaction: 
								// - (Overall Quality: High > Some Setting: High)
								// - (Some Setting: High > Overall Quality: Custom)
								// Because the loop is cut before reentry, the setting currently at the end may not require what will be cut off (misconfiguration).
								UE_LOG(LogCustomSettingsPlugin, Verbose, TEXT("Detected a (not infinite) loop through a setting's 'Set', starting from setting %s value %s and detected in setting %s value %s. A reentry must be aborted."), *StartingSetting, *StartingValue, *InSetting, *InValue);

								if (SetsY.Value.bRequire) {
									UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Cut off required value detected at two-way interaction loop: setting %s value %s."), *SetsY.Key, *SetsY.Value.ValueName);
									check(false);
								}

								// No need to look further. Tested for infinite loops in a previous test. Loops as these should be cut when encountered.
								RefNewChain.Remove(SetsY.Key);
								continue;
							}
							TestForInvalid(TestForInvalid, SetsY.Key, SetsY.Value.ValueName);
							// With these loops we are reconstructing Set branches
							// , and for every loop the next part can be different.
							// For that reason remove the previous last added bit of the chain so that the new one can be tracked.
							RefNewChain.Remove(SetsY.Key);
						}
					}
					};

				// Start testing for invalid reentry loops.
				// Check out every possible Set branch one by one.
				TestForInvalid(TestForInvalid, SettingX.Key, ValueInfoX.ValueName);
			}
		}

		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("OK: Reentering settings (two way interaction) do not cut off a requirement when cutting the loop."));
	}

	// All done?
	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Finished setting solver tests."));
}

void UPlayerSettingsSubsystem::InitialApplySettings() {
	bHasFinishedInitialApplySettings = false;
	/**
	* This method is expected to run when settings must be attempted to force to saved values.
	* This happens when a player profile is assigned, so that we have an INI section to load from.
	*
	* Thoughts: Reconstructing an application order (through Sets, as the UI would trigger application) is challenging if not impossible, given just the desired end value of settings.
	* More efficient should be to loop through all the settings and apply those that can be applied once and individually (no Set chains).
	*		The idea is to avoid recursive applications and reentries, to avoid multiple applications, overcomplexity and unsolvable order.
	*
	* If something remains unapplied, in conflict, or otherwise invalid, just restore it to save mode.
	* This is a valid situation, because it is expected that at some point system compatibility changes, a user messes with INI, or that an update simply changes the settings etc.
	*/

	// If anything fails to proceed properly, try save mode, or crash. We are removing locks from settings and doing a full (re)application.
	// If I'm right, we don't have to revert the settings to save mode first, and we can just apply each setting once individually.

	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("InitialApplySettings: Starts"));

	for (TPair<FString, FS_SettingSolverSettingInfo>& ThisSettingInfoX : SolverInfo.Settings) {
		for (const FString& LockX : ThisSettingInfoX.Value.LockedBySettings) {
			UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Setting %s lock will be unlocked. It was placed by setting %s"), *ThisSettingInfoX.Key, *LockX);
		}
		ThisSettingInfoX.Value.LockedBySettings.Empty();
	}

	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Applying initial setting values."));

	TArray<FString> ApplyCue;
	GetSolverInfo().Settings.GenerateKeyArray(ApplyCue);

	/**
	* With multiple players, there are multiple of this subsystem. This is not an issue for per player settings with no global requirements.
	* However, settings with system global effect (RHI, display resolution, VSync etc.) one solver will not communicate with another.
	* Only player 1 should be able to manage system global effects. This makes sense because:
	* - Players want to keep their own personalized settings save.
	* - Setting requirements can only be solved within a single subsystem.
	* - Global settings changing each time a second / third player is added is not practical.
	*
	* What is important is that:
	* - Settings for player 2+ do not have requirements they can not solve (a global effect setting).
	* - Settings for player 2+ are not overwritten by player 1.
	* - Settings for player 2+ must be entirely ignored and hidden away if they are a global effect (they don't even get to apply save mode).
	* - Solvers do not depend on another solver. Not designed that way. Player 2 might log into a profile before player 1 and is not aware of any updates on player 1's solver.
	*/

	// Keep track of old values before possible reapplication.
	TMap<FString, FString> OldValues;
	for (const FString& SettingX : ApplyCue) {
		UCustomSetting* SettingObjX = FindSettingObject(SettingX);
		OldValues.Add(SettingX, SettingObjX->GetAppliedValue());
		SettingObjX->CleanValue();
	}

	// If not player 1, must ignore some settings entirely (Nothing should be applied).
	const ULocalPlayer* PL = GetLocalPlayerChecked();
	if (PL->GetLocalPlayerIndex() > 0) {
		for (TPair<FString, FString> SettingX : OldValues) {
			const FS_SettingSolverSettingInfo& InfX = FindSettingInfo(SettingX.Key);
			if (InfX.bSettingDependsOnGlobalEffect) {
				ApplyCue.Remove(SettingX.Key);
			}
		}
	}

	int32 OldCueSize = ApplyCue.Num();
	bool bKeepLooping = true;
	while (bKeepLooping) {
		bKeepLooping = false;
		for (int32 i = ApplyCue.Num() - 1; i >= 0; i--) {
			const FString SettingName = ApplyCue[i];
			UCustomSetting* SettingObj = FindSettingObject(SettingName);
			const FString& SavedValX = GetSavedSettingValue(SettingName);
			if (CanApplySetting(SettingName, SavedValX)) {
				SetIsApplyingSetting(true);
				SettingObj->ApplyValue(SavedValX);
				SetIsApplyingSetting(false);
				ApplyCue.RemoveAt(i);
			}
		}
		if (OldCueSize == ApplyCue.Num()) {
			// 1. INI corrupt / misconfiguration / change in system compatibility / value just can't be applied etc.
			// 2. Nothing left to apply, we are done.
			// Either way we can stop looping here.
		}
		else {
			// There are settings left to apply, but one required another to be applied. Loop again.
			OldCueSize = ApplyCue.Num();
			bKeepLooping = true;
		}
	}

	// Anything still not applied gets a save mode value.
	for (int32 i = ApplyCue.Num() - 1; i >= 0; i--) {
		const FString SettingName = ApplyCue[i];

		const FString& SavedValX = GetSavedSettingValue(SettingName);
		if (SavedValX.IsEmpty()) {
			// Just nothing was saved. Very Verbose.
			UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Setting %s had an empty saved value, so it will be given its save mode value."), *SettingName);
		}
		else {
			UE_LOG(LogCustomSettingsPlugin, Warning, TEXT("Setting %s could not be applied with saved value %s, and will be given its save mode value."), *SettingName, *SavedValX);
		}

		UCustomSetting* SettingObj = FindSettingObject(SettingName);
		const FString& SaveModeValX = SettingObj->GetSaveModeValue();
		// Save mode may not fail.
		check(CanApplySetting(SettingName, SaveModeValX));
		SetIsApplyingSetting(true);
		SettingObj->ApplyValue(SaveModeValX);
		SetIsApplyingSetting(false);
		ApplyCue.RemoveAt(i);
	}

	// Test if locks should be applied, then do so.
	// Test for lock mismatches and prepare recovery.
	TSet<FString> MustRecoverSettings;
	for (const TPair<FString, FS_SettingSolverSettingInfo>& ThisSettingInfoX : SolverInfo.Settings) {
		const FString ThisAppliedValueX = FindSettingObject(ThisSettingInfoX.Value)->GetAppliedValue();
		const FS_SettingSolverValueInfo* ThisValInfoX = FindSettingValueInfoOptional(ThisSettingInfoX.Value, ThisAppliedValueX);
		// ValInfoX could be nullptr on a slider, where not every value on the slider is registered.
		if (ThisValInfoX != nullptr) {
			for (const TPair<FString, FS_SettingSolverValueSetterBehavior>& ThisSetBeh : ThisValInfoX->Sets) {
				// Check if a Set has a lock, and if the value that should be locked matches the applied value.
				if (ThisSetBeh.Value.bRequire) {
					FS_SettingSolverSettingInfo& OtherSettingX = SolverInfo.Settings.FindChecked(ThisSetBeh.Key);
					const FString& OtherAppliedValueX = FindSettingObject(OtherSettingX)->GetAppliedValue();
					if (OtherAppliedValueX != ThisSetBeh.Value.ValueName) {
						// The other setting has applied something else than this setting requires. 
						// Recover this setting to save mode.
						UE_LOG(LogCustomSettingsPlugin, Warning, TEXT("Setting %s value %s expected other setting %s value %s to be set, but it was set to %s. Instead of successfully setting a lock, setting %s will be cued for save mode recovery."), *ThisSettingInfoX.Key, *ThisAppliedValueX, *ThisSetBeh.Key, *ThisSetBeh.Value.ValueName, *OtherAppliedValueX, *ThisSettingInfoX.Key);
						MustRecoverSettings.Add(ThisSettingInfoX.Key);
					}
					else {
						// The other setting value is as expected, but it is not locked yet by this setting.
						OtherSettingX.LockedBySettings.Add(ThisSettingInfoX.Key);
					}
				}
			}
		}
	}

	for (const FString& MustRecoverX : MustRecoverSettings) {
		UE_LOG(LogCustomSettingsPlugin, Warning, TEXT("Setting %s is being recovered to save mode, in response to a mismatch in its required Set."), *MustRecoverX);
		UCustomSetting* SettingObjX = FindSettingObject(MustRecoverX);
		const FString& SaveModeValX = SettingObjX->GetSaveModeValue();
		// Save mode may not fail.
		check(CanApplySetting(MustRecoverX, SaveModeValX));
		SetIsApplyingSetting(true);
		SettingObjX->ApplyValue(SaveModeValX);
		SetIsApplyingSetting(false);
	}

	// At this point all settings applied with succes and finalized. Let the world know.
	for (TPair<FString, FS_SettingSolverSettingInfo>& ThisSettingInfoX : SolverInfo.Settings) {
		UCustomSetting* SettingObj = FindSettingObject(ThisSettingInfoX.Value);
		SettingObj->OnValueAppliedSuccess.Broadcast(SettingObj);
		NotifySettingObjectsOfNotCompatibleChange(ThisSettingInfoX.Key, OldValues.FindChecked(ThisSettingInfoX.Key), SettingObj->GetAppliedValue());
		// Now also broadcast if a setting was locked or not in the end.
		if (ThisSettingInfoX.Value.LockedBySettings.Num() > 0) {
			UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Setting %s entering locked state, computed after being initialized."), *ThisSettingInfoX.Key);
			SettingObj->OnSettingLocked.Broadcast(SettingObj);
		}
		else {
			UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Setting %s entering unlocked state, computed after being initialized."), *ThisSettingInfoX.Key);
			SettingObj->OnSettingUnlocked.Broadcast(SettingObj);
		}
	}

	bHasFinishedInitialApplySettings = true;

	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("InitialApplySettings: Finished initial apply settings."));
	OnFinishedInitialApplySettings.Broadcast();
}

//void UPlayerSettingsSubsystem::Deinitialize() {
//#if WITH_EDITOR
	// Possible TODO Thoughts:
	// Some editor settings like gamma affect both game and editor.
	// Here you could recover to save mode if WITH_EDITOR, so that ingame altered settings no longer affect the editor.
	// However, this would overcomplicate with possible nonsense (practical or opposite?) as it:
	// - Applies and saves savemode value.
	// - Makes you unable to apply settings which require an application reboot.
	// - ...
	// When you require a full recovery after shutdown, you could just follow normal operation by deleting the CustomSetting INI entries and rebooting the app.

	//UE_LOG(LogCustomSettingsPlugin, Verbose, TEXT("Deinitializing, disabling autosave on settings and restoring settings to save mode."));
	//for (const TPair<FString, UCustomSetting*>& SettingPairX : GetCustomSettings()) {
	//	if (!IsValid(SettingPairX.Value)) {
	//		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("The CustomSetting container contains an invalid object."));
	//		continue;
	//	}

	//	SettingPairX.Value->RecoverToSaveMode();
	//}
//#endif
//}

// Setting Solver

bool UPlayerSettingsSubsystem::FindSetBranches(const FString& InSetting, const FString& InValue, FS_SettingSolverSimSettingBranch& OutRequiredBranch, TArray<FS_SettingSolverSimSettingBranch>& OutOptionalBranches, bool bInGenerateOptionalBranches) const {
	// InSetting forms the required / optional base branch of the tree.
	// InSetting can have multiple "Sets" (a setting applying another setting).
	// A Set can be recursive, forming branches of Sets on the tree.
	// A Set can be required or optional. When walking the branches starting from the base of the tree, any optional Set encountered snaps off to become its own optional branch (recursively as well).
	// All branches we end up with have cues of settings + value that it requires to be applied for the branch to be applicable. 
	//
	//				__						 __
	//		/	   /			/		    /
	//	 __		__			    ^ opt	    ^ opt
	//		\ /				__		   __  
	//		 |				   \ /	   ^ opt
	//		 ^					|
	//							^ req
	// 
	//   
	// If the branch is optional, it can be skipped. Only the main (required) branch must be applicable for a setting to be applicable.
	// 
	// If a branch loops back into itself, it must be cut before reentry. This loop is a two-way interaction between settings. 
	// A branch dies if it is invalid, causing no side branches to be generated. It is invalid if a setting reentry is detected on a required value.

	OutRequiredBranch = FS_SettingSolverSimSettingBranch();
	OutOptionalBranches = TArray<FS_SettingSolverSimSettingBranch>();

	check(!InSetting.IsEmpty());
	check(!InValue.IsEmpty());

	// Just a set for reentry tests, doesn't mean the set is complete or that entries will change.
	TSet<FString> SettingsWantToChange;
	int32 DeadBranchCount = 0;
	auto BuildBranches = [this, &SettingsWantToChange, &DeadBranchCount, &OutRequiredBranch, &OutOptionalBranches, &bInGenerateOptionalBranches](auto&& BuildBranches, const FS_SettingSolverSimBranchCue& InCue) -> bool {
		// If this setting was already visited, it should have been detected reentry.
		check(!SettingsWantToChange.Contains(InCue.Setting));
		SettingsWantToChange.Add(InCue.Setting);

		// Collect copy of setting info
		const FS_SettingSolverSettingInfo& SettingInfo = GetSolverInfo().Settings.FindChecked(InCue.Setting);
		const FS_SettingSolverValueInfo* const ValueInfo = FindSettingValueInfoOptional(SettingInfo, InCue.Value);

		// Build the new branch
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("BuildBranches: Trying to build new branch setting %s value %s."), *InCue.Setting, *InCue.Value);
		FS_SettingSolverSimSettingBranch NewBranch = FS_SettingSolverSimSettingBranch();
		NewBranch.Setting = InCue.Setting;
		NewBranch.DesiredValue = InCue.Value;
		// The first setting in a branch (self) is always required to the branch.
		NewBranch.RequiredSetCue.Add({ NewBranch.Setting, NewBranch.DesiredValue });

		// First setting on the branch desires to change
		SettingsWantToChange.Add(NewBranch.Setting);

		// Cue to optionally build side branches if all goes well on this branch.
		TSet<FS_SettingSolverSimBranchCue> NewBranchCue;

		bool bBranchDied = false;
		if (ValueInfo != nullptr) {
			for (TPair<FString, FS_SettingSolverValueSetterBehavior> SetBeh : ValueInfo->Sets) {
				if (SettingsWantToChange.Contains(SetBeh.Key)) {
					if (SetBeh.Value.bRequire) {
						// NewBranch is dead.
						UE_LOG(LogCustomSettingsPlugin, Error, TEXT("BuildBranches: setting %s value %s detected required reentry coming in from setting %s value %s. Killing of this full branch (and side branches)."), *InCue.Setting, *InCue.Value, *SetBeh.Key, *SetBeh.Value.ValueName);
						bBranchDied = true;
						DeadBranchCount++;
						break;
					}
					// Ignore reentry
					UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("BuildBranches: setting %s value %s ignoring optional reentry coming in from setting %s value %s."), *InCue.Setting, *InCue.Value, *SetBeh.Key, *SetBeh.Value.ValueName);
					continue;
				}
				else {
					if (SetBeh.Value.bRequire) {
						UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("BuildBranches: Setting %s value %s adding Set setting %s value %s to RequiredSetCue."), *InCue.Setting, *InCue.Value, *SetBeh.Key, *SetBeh.Value.ValueName);
						SettingsWantToChange.Add(SetBeh.Key);
						NewBranch.RequiredSetCue.Add(SetBeh.Key, SetBeh.Value.ValueName);
					}
					else {
						if (bInGenerateOptionalBranches) {
							NewBranchCue.Add({ SetBeh.Key, SetBeh.Value.ValueName, false });
						}
					}
				}
			}
		}

		if (bBranchDied) {
			if (OutRequiredBranch.Setting.IsEmpty()) {
				// The first branch is the main branch, so nothing can be made. 
				UE_LOG(LogCustomSettingsPlugin, Error, TEXT("The main branch died, so nothing can be done."));
				return false;
			}
			else {
				// No side branches should be generated from this branch.
				NewBranchCue.Empty();
			}
		}
		else {
			if (OutRequiredBranch.Setting.IsEmpty()) {
				OutRequiredBranch = NewBranch;
			}
			else {
				// A next attempt is another branch, optional.
				OutOptionalBranches.Add(NewBranch);
			}

			for (const FS_SettingSolverSimBranchCue& CueX : NewBranchCue) {
				BuildBranches(BuildBranches, CueX);
			}
		}

		return true;
		};

	// Build the base of the tree and its branches, do the main branch first.
	bool bSuccess = BuildBranches(BuildBranches, { InSetting, InValue, true });
	if (!bSuccess) {
		// Nothing may have been generated.
		check(OutRequiredBranch.Setting.IsEmpty());
		check(OutOptionalBranches.Num() == 0);
	}
	else {
		check(!OutRequiredBranch.Setting.IsEmpty());
		// The setting itself + Sets forms the cue, so there must be a minimum of 1 in it.
		check(OutRequiredBranch.RequiredSetCue.Num() > 0);
	}

	// Done building. Optional branches are allowed to be cut at reentry or skipped entirely.
	if (!bSuccess) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Built branches: 0. Dead branches: %d"), DeadBranchCount);
	}
	else {
		if (DeadBranchCount > 0) {
			UE_LOG(LogCustomSettingsPlugin, Warning, TEXT("Built branches: %d. Dead branches: %d"), OutOptionalBranches.Num() + 1, DeadBranchCount);
		}
		else {
			UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Built branches: %d. Dead branches: 0"), OutOptionalBranches.Num() + 1);
		}
	}

	return bSuccess;
}

bool UPlayerSettingsSubsystem::CanApplySetting(const FString& InSetting, const FString& InValue) const {
	// To really know if something can be applied, it has to be done.
	// Reason: Solver registration is the guide, but settings can depend on applied system state to be applicable (any order), and post application checks to ensure an entire branch can be applied.
	// 
	// What we can do is:
	// - Test for NotCompatible on the desired situation.
	// - Test if locks on settings can be solved. 
	// - Test if at least the main branch is solved. Because optional branches are optional and would not influence CanApplySetting's result.
	//
	// The reasons for going through this extra trouble: 
	// - To avoid apply spamming and avoidable failures, by doing a check beforehand.
	// - To collect feedback for UI (disable settings / values which can not be applied up front).
	// - To get debugging information when application fails after the solver test. For example when system application misbehaves or when you might want to mark an optional Set required instead.

	// Thoughts: I have some thoughts on looping through the optional branches back and forth to see if they fill in requirements or unlock settings for another branch,
	// but it could be nonsense. At least for CanApplySetting. If a branch depends on another, it should probably have been on the required branch (solver configuration, required Set), or it wasn't by choice.

	check(!InSetting.IsEmpty());
	if (InValue.IsEmpty()) {
		// Just returning false here, because FindSetBranches asserts on it, 
		// and because a missing INI value is expected to be empty.
		// This works well with InitialApplySettings.
		return false;
	}

	const FS_SettingSolverSettingInfo& SettingInfo = FindSettingInfo(InSetting);
	if (SettingInfo.bSettingDependsOnGlobalEffect) {
		const ULocalPlayer* PL = GetLocalPlayerChecked();
		if (PL->GetLocalPlayerIndex() != 0) {
			// Not the first player, return false as we can't manage this setting for other players (not even save mode value!).
			return false;
		}
	}

	const UCustomSetting* ThisSetting = FindSettingObject(InSetting);
	if (!ThisSetting->IsValidValue(InValue)) {
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("CanApplySetting: Can not apply setting because the UCustomSetting reports the value as invalid."));
		return false;
	}

	FS_SettingSolverSimSettingBranch RefRequiredBranch = FS_SettingSolverSimSettingBranch();
	TArray<FS_SettingSolverSimSettingBranch> RefOptionalBranches = TArray<FS_SettingSolverSimSettingBranch>();
	// Optional branches are not generated, because we are only interested if a main branch can apply.
	if (!FindSetBranches(InSetting, InValue, RefRequiredBranch, RefOptionalBranches, false)) {
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("CanApplySetting: Can not apply setting because the Set branch failed to generate."));
		return false;
	}

	if (FindSettingInfo(InSetting).LockedBySettings.Num() > 0) {
		// The first setting in the required tree is locked by some other setting.
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("CanApplySetting: Can not apply setting because the first setting (%s) is locked by another."), *InSetting);
		return false;
	}

	// Check for any incompatibility in the new situation, if all values were applied.
	for (TPair<FString, FString> SetX : RefRequiredBranch.RequiredSetCue) {
		// See if a desired value is incompatible with another.
		const FS_SettingSolverSettingInfo& ThisSettingInfo = GetSolverInfo().Settings.FindChecked(SetX.Key);
		// So get the desired value's incompatibilities, not the currently applied one.
		const FS_SettingSolverValueInfo* ThisValueInfo = FindSettingValueInfoOptional(ThisSettingInfo, SetX.Value);
		if (ThisValueInfo == nullptr) {
			// Some settings (likely slider) don't register all values.
			continue;
		}
		for (const TPair<FString, FS_SettingSolverNotCompatibleValues>& NotCompatX : ThisValueInfo->NotCompatible) {
			// Incompatibility is registered two-way for all settings, so we just pull information for the few we need from ThisValueInfo->NotCompatible.
			const FS_SettingSolverSettingInfo& OtherSettingInfo = FindSettingInfo(NotCompatX.Key);
			// Get the desired new value for the other setting, or the currently applied one if we have none.
			FString* PossOtherNewValuePtr = RefRequiredBranch.RequiredSetCue.Find(NotCompatX.Key);
			const FString OtherNewValue = PossOtherNewValuePtr != nullptr ? *PossOtherNewValuePtr : FindSettingObject(OtherSettingInfo)->GetAppliedValue();
			// check if the values in the new situation are not compatible.
			if (NotCompatX.Value.Values.Contains(OtherNewValue)) {
				return false;
			}
		}
	}

	// Check for remaining locks on any setting in the Set branch.
	// Basically all settings changing in the branch can also unlock something.
	// Not assuming any order, we could simulate those locks lifted magically and check if any locks remain.
	// Order could be a TODO, look at it depending on application implementation.
	for (TPair<FString, FString> SetX : RefRequiredBranch.RequiredSetCue) {
		const FS_SettingSolverSettingInfo& ThisSettingInfo = GetSolverInfo().Settings.FindChecked(SetX.Key);
		if (ThisSettingInfo.LockedBySettings.Num() > 0) {
			TArray<FString> Unlocks;
			TSet<FString> RemainingLocks = ThisSettingInfo.LockedBySettings;
			RefRequiredBranch.RequiredSetCue.GenerateKeyArray(Unlocks);
			for (const FString& UnlockX : Unlocks) {
				RemainingLocks.Remove(UnlockX);
			}
			if (RemainingLocks.Num() > 0) {
				for (const FString& LockX : RemainingLocks) {
					UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("CanApplySetting: Can not apply setting %s because a lock remains: %s"), *InSetting, *LockX);
				}
				return false;
			}
		}
	}

	// Seems fine.
	return true;
}

bool UPlayerSettingsSubsystem::TryApplySetting(const FString& InSetting, const FString& InValue) {
	// Don't hijack Sets, it's unsafe.
	check(!GetIsApplyingSetting());

	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("TryApplySetting: setting %s value %s"), *InSetting, *InValue);
	if (!CanApplySetting(InSetting, InValue)) {
		// Could still happen that halfway apply something breaks but inspect that when it happens.
		return false;
	}

	SetIsApplyingSetting(true);

	const FString StartSetting = InSetting;
	const FString StartValue = InValue;

	TSet<FString> VisitedSettings;
	auto ApplySetting = [this, &VisitedSettings, &StartSetting, &StartValue](auto&& ApplySetting, const FString& InSettingX, const FString& InValueX, FString InLockBy) -> bool {
		if (VisitedSettings.Contains(InSettingX)) {
			UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Setting reentry loop detected. This is a valid two-way interaction with reentry aborted. Detected in setting %s value %s"), *InSettingX, *InValueX);
			// Valid to cut the chain.
			return true;
		}
		VisitedSettings.Add(InSettingX);

		FS_SettingSolverSettingInfo& ThisSettingInfo = SolverInfo.Settings.FindChecked(InSettingX);
		const FS_SettingSolverValueInfo* ThisNewValueInfo = FindSettingValueInfoOptional(ThisSettingInfo, InValueX);
		if (ThisNewValueInfo != nullptr) {
			for (const TPair<FString, FS_SettingSolverValueSetterBehavior>& ThisValueSet : ThisNewValueInfo->Sets) {
				UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("TryApplySetting: Testing of setting can still be applied: setting %s value %s"), *InSettingX, *InValueX);
				//if (!CanApplySetting(ThisValueSet.Key, ThisValueSet.Value.ValueName)) {
				// Still test on the first set on the branch? Else you will run into a lock on a Set.
				if (!CanApplySetting(StartSetting, StartValue)) {
					UE_LOG(LogCustomSettingsPlugin, Error, TEXT("It was not possible to apply required Set %s value %s for setting %s value %s. This is unexpected because we checked before."), *ThisValueSet.Key, *ThisValueSet.Value.ValueName, *InSettingX, *InValueX);
					return false;
				}
			}
		}

		// Sets can be Set, so we still seem fine.

		// Remove current locks on current Sets.
		UCustomSetting* SettingObj = FindSettingObject(ThisSettingInfo);
		const FS_SettingSolverValueInfo* ThisCurrentValueInfo = FindSettingValueInfoOptional(ThisSettingInfo, SettingObj->GetAppliedValue());
		if (ThisCurrentValueInfo != nullptr) {
			for (const TPair<FString, FS_SettingSolverValueSetterBehavior>& ThisValueSet : ThisCurrentValueInfo->Sets) {
				if (ThisValueSet.Value.bRequire) {
					FS_SettingSolverSettingInfo& OtherSettingInfo = SolverInfo.Settings.FindChecked(ThisValueSet.Key);
					UCustomSetting* OtherSettingObj = FindSettingObject(OtherSettingInfo);
					if (OtherSettingInfo.LockedBySettings.Contains(InSettingX)) {
						OtherSettingInfo.LockedBySettings.Remove(InSettingX);
						if (OtherSettingInfo.LockedBySettings.Num() == 0) {
							UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Setting %s entering unlocked state, with the last lock removed: %s"), *ThisValueSet.Key, *InSettingX);
							OtherSettingObj->OnSettingUnlocked.Broadcast(SettingObj);
						}
						UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("setting %s current value %s is changing, removing the 'required lock' %s from setting %s"), *InSettingX, *ThisCurrentValueInfo->ValueName, *InSettingX, *ThisValueSet.Key);
					}
				}
			}
		}

		const FString OldValue = SettingObj->GetAppliedValue();

		// Apply self
		SettingObj->ApplyValue(InValueX);

		// Lock if needed.
		if (!InLockBy.IsEmpty()) {
			ThisSettingInfo.LockedBySettings.Add(InLockBy);
			if (ThisSettingInfo.LockedBySettings.Num() == 1) {
				UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Setting %s entering locked state, the first lock being: %s"), *InSettingX, *InLockBy);
				SettingObj->OnSettingLocked.Broadcast(SettingObj);
			}
		}

		if (ThisNewValueInfo != nullptr) {
			for (const TPair<FString, FS_SettingSolverValueSetterBehavior>& ValueSetX : ThisNewValueInfo->Sets) {
				const FString NewLock = ValueSetX.Value.bRequire ? InSettingX : "";
				if (!ApplySetting(ApplySetting, ValueSetX.Key, ValueSetX.Value.ValueName, NewLock)) {
					// Something broke halfway regardless of prior checks.
					UE_LOG(LogCustomSettingsPlugin, Error, TEXT("A Set failed to apply. setting %s value %s"), *ValueSetX.Key, *ValueSetX.Value.ValueName);
					return false;
				}
			}
		}

		// Save to INI
		GConfig->SetString(*GetSettingINISection(), *InSettingX, *InValueX, GetSettingINI());
		GConfig->Flush(true, GetSettingINI());
		// Let the others know of success
		SettingObj->OnValueAppliedSuccess.Broadcast(SettingObj);
		NotifySettingObjectsOfNotCompatibleChange(InSettingX, OldValue, SettingObj->GetAppliedValue());

		return true;
		};

	const bool bSuccess = ApplySetting(ApplySetting, InSetting, InValue, "");

	SetIsApplyingSetting(false);

	if (!bSuccess) {
		for (const FString& SettingX : VisitedSettings) {
			UE_LOG(LogCustomSettingsPlugin, Warning, TEXT("Because the setting chain of setting %s value %s failed to apply after a prior CanApply check, affected setting %s will be restored to save mode."), *InSetting, *InValue, *SettingX);
			ForceApplySaveModeToSettingAndAffected(InSetting);
		}
	}
	else {
		UCustomSetting* SettingObj = FindSettingObject(StartSetting);
		// There will be cases where a Set desires to do one thing or another (appearing dynamic / generated).
		// Examples: 
		// -- A starts benchmark, settings B C D E calculate their values.
		// -- "Auto" mode which Sets value A B or C.
		// 
		// Such applications are not done by hijacking TryApply halfway, as it would unsafely stray from solver registrations.
		// What can be done, is to respond to OnSetChainAppliedSuccess. triggered once for a chain after TryApply finishes applying
		// , to TryApply again.
		// 
		// Benefit:
		// -- Follows configured and tested solver registration safely. All goes through TryApply. 
		//  Sets are known beforehand (as configured), tested, and not generated / dynamic.
		// Downside: 
		// -- Set Chains are attempted one after another. That does not detect two-way interaction:
		//  overall quality: auto > benchmark runs > A setting responds: high > overall quality responds: custom (it doesn't stick with auto.)
		// -- checks run per Set chain, not on the combined Set chains.
		//
		// Use with care, understanding the pros / cons.
		SettingObj->OnSetChainAppliedSuccess.Broadcast(SettingObj);
	}

	return bSuccess;
}

void UPlayerSettingsSubsystem::ForceApplySaveModeToSettingAndAffected(const FString& InSetting) {
	FS_SettingSolverSettingInfo& SettingInfo = SolverInfo.Settings.FindChecked(InSetting);
	for (const FString& LockX : SettingInfo.LockedBySettings) {
		ForceApplySaveModeToSettingAndAffected(LockX);
	}
	SettingInfo.LockedBySettings.Empty();
	UCustomSetting* SettingObj = FindSettingObject(SettingInfo);
	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Setting %s entering unlocked state, because save mode value is being applied (force)."), *InSetting);
	SettingObj->OnSettingUnlocked.Broadcast(SettingObj);
	check(TryApplySetting(InSetting, FindSettingObject(SettingInfo)->GetSaveModeValue()));
}

// INI

FString UPlayerSettingsSubsystem::GetSettingINI() const {
	// Just using this existing ini. The GameUserSettings class uses this as well, but has its own section, and will not conflict.
	return GGameUserSettingsIni;
}

FString UPlayerSettingsSubsystem::GetSettingINIProfile() const {
	// If the ini profile is empty, no profile was assigned yet.
	check(!SettingINIProfile.IsEmpty());
	return SettingINIProfile;
}

FString UPlayerSettingsSubsystem::GetSettingINISection() const {
	return FString(UCustomSettingsUtils::SectionCustomSettingsProfile + "/" + GetSettingINIProfile());
}

FString UPlayerSettingsSubsystem::GetSavedSettingValue(const FString& InSetting) const {
	if (!GConfig) {
		UE_LOG(LogCustomSettingsPlugin, Warning, TEXT("GConfig is Invalid, can't get setting value. Returning empty."));
		return FString();
	}

	// Get the value from ini, or empty.
	FString OutValue = "";
	GConfig->GetString(*GetSettingINISection(), *InSetting, OutValue, GetSettingINI());

	return OutValue;
}

void UPlayerSettingsSubsystem::RemoveSavedSettingValuesByProfileName(const FString& InProfileName) {
	if (!GConfig) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GConfig is Invalid."));
		return;
	}

	// A setting is expected to have one constant INI file to read and write its CustomSetting value to on its profile. 
	// The profile is expected to change between default state and optionally to a different one when a player is assigned a personal profile.
	// It makes the most sense that "resetting saved profile values" would loop through settings, ask them for their INI to remove from, then wipe sections by InProfileName (not the current setting profile).

	for (const TPair<FString, FS_SettingSolverSettingInfo> SettingInfoX : GetSolverInfo().Settings) {
		const UCustomSetting* SettingObj = SettingInfoX.Value.SettingObject;
		check(IsValid(SettingObj));
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Removing saved value from setting specified INI %s by profile %s for setting %s"), *GetSettingINI(), *InProfileName, *SettingInfoX.Key);

		// Try to empty all (if any) saved setting sections for a player profile.
		const FString Section = FString(UCustomSettingsUtils::SectionCustomSettingsProfile + "/" + InProfileName);
		if (GConfig->DoesSectionExist(*Section, GetSettingINI())) {
			if (GConfig->EmptySection(*Section, GetSettingINI())) {
				UE_LOG(LogCustomSettingsPlugin, Verbose, TEXT("Emptied setting section: %s"), *Section);
			}
		}
	}
}

const FS_SettingSolverSettingInfo& UPlayerSettingsSubsystem::FindSettingInfo(const FString& InSetting) const {
	return GetSolverInfo().Settings.FindChecked(InSetting);
}

const FS_SettingSolverValueInfo& UPlayerSettingsSubsystem::FindSettingValueInfo(const FS_SettingSolverSettingInfo& InSetting, const FString& InValue) const {
	const FS_SettingSolverValueInfo* ValueX = InSetting.ValueInfo.FindByPredicate([&InValue](const FS_SettingSolverValueInfo& ValInfoX) { return InValue == ValInfoX.ValueName; });
	check(ValueX != nullptr);
	return *ValueX;
}

const FS_SettingSolverValueInfo& UPlayerSettingsSubsystem::FindSettingValueInfo(const FString& InSetting, const FString& InValue) const {
	return FindSettingValueInfo(FindSettingInfo(InSetting), InValue);
}

const FS_SettingSolverValueInfo* UPlayerSettingsSubsystem::FindSettingValueInfoOptional(const FS_SettingSolverSettingInfo& InSetting, const FString& InValue) const {
	return InSetting.ValueInfo.FindByPredicate([&InValue](const FS_SettingSolverValueInfo& ValInfoX) { return InValue == ValInfoX.ValueName; });
}

const FS_SettingSolverValueInfo* UPlayerSettingsSubsystem::FindSettingValueInfoOptional(const FString& InSetting, const FString& InValue) const {
	return FindSettingValueInfoOptional(FindSettingInfo(InSetting), InValue);
}

UCustomSetting* UPlayerSettingsSubsystem::FindSettingObject(const FS_SettingSolverSettingInfo& InSetting) const {
	check(IsValid(InSetting.SettingObject));
	return InSetting.SettingObject;
}

UCustomSetting* UPlayerSettingsSubsystem::FindSettingObject(const FString& InSetting) const {
	const FS_SettingSolverSettingInfo& SettingInfo = FindSettingInfo(InSetting);
	check(IsValid(SettingInfo.SettingObject));
	return SettingInfo.SettingObject;
}

TMap<FString, FString> UPlayerSettingsSubsystem::FindNotCompatibleSettings(const FString& InSetting, const FString& InValue) const {
	TMap<FString, FString> NotCompatibleSettings;
	for (const TPair<FString, FS_SettingSolverSettingInfo> OtherSettingInfoX : GetSolverInfo().Settings) {
		const UCustomSetting* const OtherSettingObj = FindSettingObject(OtherSettingInfoX.Value);
		for (const FS_SettingSolverValueInfo& OtherValueInfoX : OtherSettingInfoX.Value.ValueInfo) {
			const FS_SettingSolverNotCompatibleValues* const OtherNotCompats = OtherValueInfoX.NotCompatible.Find(InSetting);
			if (OtherNotCompats != nullptr && OtherNotCompats->Values.Contains(InValue)) {
				NotCompatibleSettings.Add({ OtherSettingInfoX.Key, OtherValueInfoX.ValueName });
			}
		}
	}
	return NotCompatibleSettings;
}

void UPlayerSettingsSubsystem::NotifySettingObjectsOfNotCompatibleChange(const FString& InSetting, const FString& InOldValue, const FString& InNewValue) {
	const TMap<FString, FString> OldNotCompats = FindNotCompatibleSettings(InSetting, InOldValue);
	const TMap<FString, FString> NewNotCompats = FindNotCompatibleSettings(InSetting, InNewValue);
	TMap<FString, FString> DifCompats = NewNotCompats.FilterByPredicate([&OldNotCompats](const TPair<FString, FString>& NotCompatX) {
		const FString* ValPtr = OldNotCompats.Find(NotCompatX.Key);
		return ValPtr == nullptr || NotCompatX.Value != *ValPtr;
		});
	for (const TPair<FString, FString>& DifX : OldNotCompats.FilterByPredicate([&NewNotCompats](const TPair<FString, FString>& NotCompatX) {
		const FString* ValPtr = NewNotCompats.Find(NotCompatX.Key);
		return ValPtr == nullptr || NotCompatX.Value != *ValPtr;

		})) {
		DifCompats.Add(DifX);
	}

	for (const TPair<FString, FString>& DifX : DifCompats) {
		// NotCompatible relations changed for these settings > values, and they should be notified of it.
		// This is useful to update UI to a state where old / new incompatibilities must be reflected as disabled or re enabled widget options.
		UCustomSetting* SettingObj = FindSettingObject(DifX.Key);
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("A change in NotCompat (setting %s value %s) was detected and will be broadcasted to setting %s"), *DifX.Key, *DifX.Value, *DifX.Key);
		SettingObj->OnOtherSettingRelevantToSelfCompatibilityChanged.Broadcast(SettingObj);
	}
}

// Delegates

void UPlayerSettingsSubsystem::ActOnPlayerProfileRemoved(UPlayerProfile* InProfile) {
	if (!IsValid(InProfile)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("InProfile invalid"));
		return;
	}

	RemoveSavedSettingValuesByProfileName(InProfile->GetProfileName());
}

void UPlayerSettingsSubsystem::ActOnPlayerAssignedToProfile(UPlayerProfile* InProfile, ULocalPlayer* InPlayer) {
	if (!IsValid(InProfile) || !IsValid(InPlayer)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("InProfile or InPlayer invalid."));
		return;
	}
	if (InPlayer != GetLocalPlayer()) {
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Player profile changed but for a different player. Ignoring."));
		// Profile can't be loaded or the player is not the player using this subsystem.
		return;
	}

	SettingINIProfile = InProfile->GetProfileName();

	// A player profile is required before applying initial settings, because a default profile is not in use. 
	// A default profile would conflict when multiple players are present at once. 
	// It would also load different initial settings than the personalized ones.
	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Player profile %s was assigned, triggering InitialApplySettings."), *GetSettingINIProfile());
	InitialApplySettings();
}

