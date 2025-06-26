/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "CustomKeybindingSetting.h"
#include "CustomSettingsUtils.h"
#include "PlayerSettingsSubsystem.h"
#include "SlateUtils.h"
#include "LogCustomSettingsPlugin.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/KismetInputLibrary.h"


FS_SettingSolverSettingRegistration UCustomKeybindingSetting::GetSolverRegistrationInfo_Implementation() const {
	return {
		UCustomSettingsUtils::SettingKeybinding,
		{},
		false
	};
}

FString UCustomKeybindingSetting::GetSaveModeValue_Implementation() const {
	const UInputSettings* InputSettings = UInputSettings::GetInputSettings();
	check(IsValid(InputSettings));
	return UCustomSettingsUtils::KeyMappingsStructToString(FS_KeyMappings(InputSettings->GetActionMappings(), InputSettings->GetAxisMappings()));
}

bool UCustomKeybindingSetting::DoesAppliedValueMatchSystemState_Implementation() const {
	// This setting does not directly apply an effect, as it is used to look up the setting value instead. 
	return true;
}

bool UCustomKeybindingSetting::IsValidValue_Implementation(const FString& InValue) const {
	if (!Super::IsValidValue_Implementation(InValue)) {
		return false;
	}

	// Thoughts: Currently any value is treated as valid (ignoring input name / key blacklists), which makes sense for initial applications / save mode.
	// Blacklist is processed through RequestInputMappingKeyRebind
	
	// When there is a mismatch (number, names) between input setting input mappings and InValue, InValue is invalid.
	// For keybindings this is common when a developer adds or removes input mappings.
	// Because the save mode value returns data from the input settings, save mode value is detected valid here.
	{
		const UInputSettings* InputSettings = UInputSettings::GetInputSettings();
		check(IsValid(InputSettings));
		const FS_KeyMappings InputSettingMappings = FS_KeyMappings(InputSettings->GetActionMappings(), InputSettings->GetAxisMappings());
		const FS_KeyMappings InMappings = UCustomSettingsUtils::StringToKeyMappingsStruct(InValue);
		if (InputSettingMappings.ActionKeyMappings.Num() != InMappings.ActionKeyMappings.Num()
			|| InputSettingMappings.AxisKeyMappings.Num() != InMappings.AxisKeyMappings.Num()
			) {
			UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Detected mismatch in number of action / axis mappings comparing InValue to input setting's mappings, so returning false."));
			return false;
		}

		TSet<FName> InputSettingActionMappingNames;
		TSet<FName> InputSettingAxisMappingNames;
		for (const FInputActionKeyMapping& MapX : InputSettingMappings.ActionKeyMappings) {
			InputSettingActionMappingNames.Add(MapX.ActionName);
		}
		for (const FInputAxisKeyMapping& MapX : InputSettingMappings.AxisKeyMappings) {
			InputSettingAxisMappingNames.Add(MapX.AxisName);
		}

		for (const FInputActionKeyMapping& MapX : InMappings.ActionKeyMappings) {
			if (!InputSettingActionMappingNames.Contains(MapX.ActionName)) {
				UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Input setting's input action mapping name: %s is not present on InValue, so returning false."), *MapX.ActionName.ToString());
				return false;
			}
		}
		for (const FInputAxisKeyMapping& MapX : InMappings.AxisKeyMappings) {
			if (!InputSettingAxisMappingNames.Contains(MapX.AxisName)) {
				UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Input setting's input axis mapping name: %s is not present on InValue, so returning false."), *MapX.AxisName.ToString());
				return false;
			}
		}
	}


	return true;
}

void UCustomKeybindingSetting::ApplyValueToSystem_Implementation() {
	// UCustomSettingsPlayerInput pulls the keybinding automatically when the setting is applied successfully.
}

// Keybinding

TSet<FName> UCustomKeybindingSetting::GetBlacklistInputNames_Implementation() const {
	// Here you can add input mapping names to the blacklist to protect them from being changed by keybinding.
	TSet<FName> BlacklistInputNames;

	/**
	* System mappings / slate navigation is backlisted here:
	* - To prevent UI control loss.
	* - Because less flexible classes register input once, not per player (SlateNavigationConfig, AnalogCursor).
	*/
	for (const FName& NameX : USlateUtils::GetStandardSlateNavInputNames()) {
		BlacklistInputNames.Add(NameX);
	}
	for (const FName& NameX : USlateUtils::GetExtendedSlateNavInputNames()) {
		BlacklistInputNames.Add(NameX);
	}

	return BlacklistInputNames;
}

TSet<FKey> UCustomKeybindingSetting::GetBlacklistInputKeys_Implementation() const {
	// Here you can add input keys to the blacklist to prevent keybinding from mapping to the key.
	TSet<FKey> BlacklistInputKeys;

	// Anything should be in here by default? Engine mappings (console)?
	// Most likely not a requirement but just up to preference.

	// Just... blacklist anything that seems unsupported.
	// Look at implementation of blacklist on USettingKeybindingWidget CanShowKey as well.
	TArray<FKey> AllKeys;
	EKeys::GetAllKeys(AllKeys);
	for (FKey KeyX : AllKeys) {
		if (!UKismetInputLibrary::Key_IsMouseButton(KeyX)
			&& !UKismetInputLibrary::Key_IsGamepadKey(KeyX)
			&& !UKismetInputLibrary::Key_IsKeyboardKey(KeyX)
			) {
			BlacklistInputKeys.Add(KeyX);
		}
		// Blacklisting axis keys which are impossible to select properly 
		// (left thumbstick / 2D / up, mouse axis / 2D / up? etc.)
		// Bit of a pain currently.
		// TODO when engine implementation improves look back at this.
		if (
			KeyX.IsAxis2D()
			|| KeyX.IsAxis3D()
			) {
			UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Blacklisting certain axis key from keybinding %s."), *KeyX.ToString());
			BlacklistInputKeys.Add(KeyX);
		}
	}

	// Add anything if still required.
	BlacklistInputKeys.Add(EKeys::MouseX);
	BlacklistInputKeys.Add(EKeys::MouseY);
	BlacklistInputKeys.Add(EKeys::Gamepad_LeftX);
	BlacklistInputKeys.Add(EKeys::Gamepad_LeftY);
	BlacklistInputKeys.Add(EKeys::Gamepad_RightX);
	BlacklistInputKeys.Add(EKeys::Gamepad_RightY);

	return BlacklistInputKeys;
}

void UCustomKeybindingSetting::RequestInputMappingKeyRebind(const FName& InInputName, const FInputChord& InOldValue, const FInputChord& InNewValue) {
	if (!IsValid(GetPlayerSettingsSubsystem())) {
		return;
	}
	if (GetBlacklistInputNames().Contains(InInputName)) {
		return;
	}
	// Thoughts: Should not allow a key to rebind to a different input device, since we are showing keybinding widgets per input device. 
	// Allowing would lead to further complexity.
	auto IsRebindOnSameInputDevice = [](const FKey& InOldKey, const FKey& InNewKey) -> bool {
		// Again afaik no library provided by engine to retrieve an EInputDevices per key, so I got to do this with limited support.
		// Seems it also doesn't detect axis "buttons" (joystick movements, mouse movements etc.)
		if (
			(UKismetInputLibrary::Key_IsKeyboardKey(InOldKey) && UKismetInputLibrary::Key_IsKeyboardKey(InNewKey))
			|| (UKismetInputLibrary::Key_IsMouseButton(InOldKey) && UKismetInputLibrary::Key_IsMouseButton(InNewKey))
			|| (UKismetInputLibrary::Key_IsGamepadKey(InOldKey) && UKismetInputLibrary::Key_IsGamepadKey(InNewKey))
			) {
			return true;
		}
		return false;
	};
	if (!IsRebindOnSameInputDevice(InOldValue.Key, InNewValue.Key)) {
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Ignoring rebind of key %s to %s because they are not the same input device."), *InOldValue.Key.ToString(), *InNewValue.Key.ToString());
		return;
	}

	// Thoughts: Treat the old value as blacklisted too? Since we wouldn't be able to change back.
	const TSet<FKey> BlacklistKeys = GetBlacklistInputKeys();
	if (BlacklistKeys.Contains(InOldValue.Key) || BlacklistKeys.Contains(InNewValue.Key)) {
		return;
	}

	FS_KeyMappings KeyMappings = UCustomSettingsUtils::StringToKeyMappingsStruct(GetAppliedValue());
	bool bFoundIt = false;
	for (FInputActionKeyMapping& MapX : KeyMappings.ActionKeyMappings) {
		if (MapX.ActionName == InInputName && InOldValue == USlateUtils::GetInputChordFromInputActionKeyMapping(MapX)) {
			MapX.bAlt = InNewValue.bAlt;
			MapX.bCmd = InNewValue.bCmd;
			MapX.bCtrl = InNewValue.bCtrl;
			MapX.bShift = InNewValue.bShift;
			MapX.Key = InNewValue.Key;
			bFoundIt = true;
			break;		
		}
	}
	if (!bFoundIt) {
		for (FInputAxisKeyMapping& MapX : KeyMappings.AxisKeyMappings) {
			if (MapX.AxisName == InInputName && InOldValue == USlateUtils::GetInputChordFromInputAxisKeyMapping(MapX)) {
				// Axis... sadly doesn't register modifier keys. And ignore the scale, I don't know any keybinding UI which uses that.
				// I find the scale registration nonsense because in game systems scale and inversion is usually not a hardcoded float but part of more flexible / complex systems.
				// In case of inversion, you should just add another binding instead.
				MapX.Key = InNewValue.Key;
				bFoundIt = true;
				break;			
			}
		}
	}
	check(bFoundIt);

	GetPlayerSettingsSubsystem()->TryApplySetting(GetSolverRegistrationInfo().SettingName, UCustomSettingsUtils::KeyMappingsStructToString(KeyMappings));
}

// Delegates

void UCustomKeybindingSetting::ActOnInputKeySelectorsWidgetSelects(const FName& InInputName, const FInputChord& InOldValue, const FInputChord& InNewValue) {
	RequestInputMappingKeyRebind(InInputName, InOldValue, InNewValue);
}