/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "CustomSettingsPlayerInput.h"
#include "CustomSettingsUtils.h"
#include "CustomSetting.h"
#include "CustomKeybindingSetting.h"
#include "PlayerSettingsSubsystem.h"
#include "LogCustomSettingsPlugin.h"
#include "GameFramework/PlayerController.h"
#include "Templates/SharedPointer.h"
#include "Engine/LocalPlayer.h"
#include "SlateUtils.h"



// Input

void UCustomSettingsPlayerInput::PullCustomKeybindings() {
	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Pulling keybindings"));

	const APlayerController* PC = GetOuterAPlayerController();
	const ULocalPlayer* PL = IsValid(PC) ? PC->GetLocalPlayer() : nullptr;
	const UPlayerSettingsSubsystem* PSS = IsValid(PL) ? PL->GetSubsystem<UPlayerSettingsSubsystem>() : nullptr;
	if (!IsValid(PSS)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Can't pull keybindings, because PlayerSettingsSubsystem can't be retrieved."));
		check(false);
	}

	const UCustomKeybindingSetting* KeybindSetting = Cast<UCustomKeybindingSetting>(PSS->FindSettingObject(UCustomSettingsUtils::SettingKeybinding));
	// Pointless to go on if the setting is not valid.
	check(IsValid(KeybindSetting));

	const FS_KeyMappings NewKeyMappings = UCustomSettingsUtils::StringToKeyMappingsStruct(KeybindSetting->GetAppliedValue());

	// Clear old mappings
	TArray<FInputActionKeyMapping> PlayerInputActionKeyMappingsCopy = ActionMappings;
	for (const FInputActionKeyMapping& InputActionKeyMappingX : PlayerInputActionKeyMappingsCopy) {
		RemoveActionMapping(InputActionKeyMappingX);
	}
	TArray<FInputAxisKeyMapping> PlayerInputAxisKeyMappingsCopy = AxisMappings;
	for (const FInputAxisKeyMapping& InputAxisKeyMappingX : PlayerInputAxisKeyMappingsCopy) {
		RemoveAxisMapping(InputAxisKeyMappingX);
	}
	// Add new mappings
	for (const FInputActionKeyMapping& InputActionKeyMappingX : NewKeyMappings.ActionKeyMappings) {
		AddActionMapping(InputActionKeyMappingX);
	}
	for (const FInputAxisKeyMapping& InputAxisKeyMappingX : NewKeyMappings.AxisKeyMappings) {
		AddAxisMapping(InputAxisKeyMappingX);
	}
}

// Delegates

void UCustomSettingsPlayerInput::ActOnWorldBeginPlay() {
	Super::ActOnWorldBeginPlay();

	const ULocalPlayer* PL = IsValid(GetOuterAPlayerController()) ? GetOuterAPlayerController()->GetLocalPlayer() : nullptr;
	UPlayerSettingsSubsystem* PSS = IsValid(PL) ? PL->GetSubsystem<UPlayerSettingsSubsystem>() : nullptr;
	check(IsValid(PSS));		

	// Bind to settings

	// Keybindings
	{
		UCustomSetting* KeybindSetting = PSS->FindSettingObject(UCustomSettingsUtils::SettingKeybinding);
		KeybindSetting->OnValueAppliedSuccess.AddDynamic(this, &UCustomSettingsPlayerInput::ActOnKeybindSettingValueAppliedSuccess);
		if (KeybindSetting->HasAppliedValue()) {
			ActOnKeybindSettingValueAppliedSuccess(KeybindSetting);
		}
	}
	// Gamepad
	{
		UCustomSetting* Setting = PSS->FindSettingObject(UCustomSettingsUtils::SettingGamepadInvertXAxisRotate);
		Setting->OnValueAppliedSuccess.AddDynamic(this, &UCustomSettingsPlayerInput::ActOnGamepadInvertXAxisRotateSettingValueAppliedSuccess);
		if (Setting->HasAppliedValue()) {
			ActOnGamepadInvertXAxisRotateSettingValueAppliedSuccess(Setting);
		}
	}
	{
		UCustomSetting* Setting = PSS->FindSettingObject(UCustomSettingsUtils::SettingGamepadInvertYAxisRotate);
		Setting->OnValueAppliedSuccess.AddDynamic(this, &UCustomSettingsPlayerInput::ActOnGamepadInvertYAxisRotateSettingValueAppliedSuccess);
		if (Setting->HasAppliedValue()) {
			ActOnGamepadInvertYAxisRotateSettingValueAppliedSuccess(Setting);
		}
	}
	{
		UCustomSetting* Setting = PSS->FindSettingObject(UCustomSettingsUtils::SettingGamepadXAxisRotateSensitivity);
		Setting->OnValueAppliedSuccess.AddDynamic(this, &UCustomSettingsPlayerInput::ActOnGamepadXAxisRotateSensitivitySettingValueAppliedSuccess);
		if (Setting->HasAppliedValue()) {
			ActOnGamepadXAxisRotateSensitivitySettingValueAppliedSuccess(Setting);
		}
	}
	{
		UCustomSetting* Setting = PSS->FindSettingObject(UCustomSettingsUtils::SettingGamepadYAxisRotateSensitivity);
		Setting->OnValueAppliedSuccess.AddDynamic(this, &UCustomSettingsPlayerInput::ActOnGamepadYAxisRotateSensitivitySettingValueAppliedSuccess);
		if (Setting->HasAppliedValue()) {
			ActOnGamepadYAxisRotateSensitivitySettingValueAppliedSuccess(Setting);
		}
	}
	// Keyboard and mouse
	{
		UCustomSetting* Setting = PSS->FindSettingObject(UCustomSettingsUtils::SettingKeyboardMouseInvertXAxisRotate);
		Setting->OnValueAppliedSuccess.AddDynamic(this, &UCustomSettingsPlayerInput::ActOnKeyboardMouseInvertXAxisRotateSettingValueAppliedSuccess);
		if (Setting->HasAppliedValue()) {
			ActOnKeyboardMouseInvertXAxisRotateSettingValueAppliedSuccess(Setting);
		}
	}
	{
		UCustomSetting* Setting = PSS->FindSettingObject(UCustomSettingsUtils::SettingKeyboardMouseInvertYAxisRotate);
		Setting->OnValueAppliedSuccess.AddDynamic(this, &UCustomSettingsPlayerInput::ActOnKeyboardMouseInvertYAxisRotateSettingValueAppliedSuccess);
		if (Setting->HasAppliedValue()) {
			ActOnKeyboardMouseInvertYAxisRotateSettingValueAppliedSuccess(Setting);
		}
	}
	{
		UCustomSetting* Setting = PSS->FindSettingObject(UCustomSettingsUtils::SettingKeyboardMouseXAxisRotateSensitivity);
		Setting->OnValueAppliedSuccess.AddDynamic(this, &UCustomSettingsPlayerInput::ActOnKeyboardMouseXAxisRotateSensitivitySettingValueAppliedSuccess);
		if (Setting->HasAppliedValue()) {
			ActOnKeyboardMouseXAxisRotateSensitivitySettingValueAppliedSuccess(Setting);
		}
	}
	{
		UCustomSetting* Setting = PSS->FindSettingObject(UCustomSettingsUtils::SettingKeyboardMouseYAxisRotateSensitivity);
		Setting->OnValueAppliedSuccess.AddDynamic(this, &UCustomSettingsPlayerInput::ActOnKeyboardMouseYAxisRotateSensitivitySettingValueAppliedSuccess);
		if (Setting->HasAppliedValue()) {
			ActOnKeyboardMouseYAxisRotateSensitivitySettingValueAppliedSuccess(Setting);
		}
	}
}

void UCustomSettingsPlayerInput::ActOnKeybindSettingValueAppliedSuccess(UCustomSetting* InSetting) {
	PullCustomKeybindings();
}

void UCustomSettingsPlayerInput::ActOnGamepadInvertXAxisRotateSettingValueAppliedSuccess(UCustomSetting* InSetting) {
	check(IsValid(InSetting));
	bool bInvert = UCustomSettingsUtils::StringToBool(InSetting->GetAppliedValue());
	const float MultValue = bInvert ? -1 : 1;
	AddInputAxisMultiplier(EInputDevices::Gamepad, USlateUtils::InputAxisYawPos, FName(UCustomSettingsUtils::SettingGamepadInvertXAxisRotate), MultValue);
	AddInputAxisMultiplier(EInputDevices::Gamepad, USlateUtils::InputAxisYawNeg, FName(UCustomSettingsUtils::SettingGamepadInvertXAxisRotate), MultValue);
}

void UCustomSettingsPlayerInput::ActOnGamepadInvertYAxisRotateSettingValueAppliedSuccess(UCustomSetting* InSetting) {
	check(IsValid(InSetting));
	bool bInvert = UCustomSettingsUtils::StringToBool(InSetting->GetAppliedValue());
	const float MultValue = bInvert ? -1 : 1;
	AddInputAxisMultiplier(EInputDevices::Gamepad, USlateUtils::InputAxisPitchPos, FName(UCustomSettingsUtils::SettingGamepadInvertYAxisRotate), MultValue);
	AddInputAxisMultiplier(EInputDevices::Gamepad, USlateUtils::InputAxisPitchNeg, FName(UCustomSettingsUtils::SettingGamepadInvertYAxisRotate), MultValue);
}

void UCustomSettingsPlayerInput::ActOnGamepadXAxisRotateSensitivitySettingValueAppliedSuccess(UCustomSetting* InSetting) {
	check(IsValid(InSetting));
	const float MultValue = UCustomSettingsUtils::StringToFloat(InSetting->GetAppliedValue());
	AddInputAxisMultiplier(EInputDevices::Gamepad, USlateUtils::InputAxisYawPos, FName(UCustomSettingsUtils::SettingGamepadXAxisRotateSensitivity), MultValue);
	AddInputAxisMultiplier(EInputDevices::Gamepad, USlateUtils::InputAxisYawNeg, FName(UCustomSettingsUtils::SettingGamepadXAxisRotateSensitivity), MultValue);
}

void UCustomSettingsPlayerInput::ActOnGamepadYAxisRotateSensitivitySettingValueAppliedSuccess(UCustomSetting* InSetting) {
	check(IsValid(InSetting));
	const float MultValue = UCustomSettingsUtils::StringToFloat(InSetting->GetAppliedValue());
	AddInputAxisMultiplier(EInputDevices::Gamepad, USlateUtils::InputAxisPitchPos, FName(UCustomSettingsUtils::SettingGamepadYAxisRotateSensitivity), MultValue);
	AddInputAxisMultiplier(EInputDevices::Gamepad, USlateUtils::InputAxisPitchNeg, FName(UCustomSettingsUtils::SettingGamepadYAxisRotateSensitivity), MultValue);
}

void UCustomSettingsPlayerInput::ActOnKeyboardMouseInvertXAxisRotateSettingValueAppliedSuccess(UCustomSetting* InSetting) {
	check(IsValid(InSetting));
	bool bInvert = UCustomSettingsUtils::StringToBool(InSetting->GetAppliedValue());
	const float MultValue = bInvert ? -1 : 1;
	AddInputAxisMultiplier(EInputDevices::Keyboard, USlateUtils::InputAxisYawPos, FName(UCustomSettingsUtils::SettingKeyboardMouseInvertXAxisRotate), MultValue);
	AddInputAxisMultiplier(EInputDevices::Keyboard, USlateUtils::InputAxisYawNeg, FName(UCustomSettingsUtils::SettingKeyboardMouseInvertXAxisRotate), MultValue);
	AddInputAxisMultiplier(EInputDevices::Mouse, USlateUtils::InputAxisYawPos, FName(UCustomSettingsUtils::SettingKeyboardMouseInvertXAxisRotate), MultValue);
	AddInputAxisMultiplier(EInputDevices::Mouse, USlateUtils::InputAxisYawNeg, FName(UCustomSettingsUtils::SettingKeyboardMouseInvertXAxisRotate), MultValue);
}

void UCustomSettingsPlayerInput::ActOnKeyboardMouseInvertYAxisRotateSettingValueAppliedSuccess(UCustomSetting* InSetting) {
	check(IsValid(InSetting));
	bool bInvert = UCustomSettingsUtils::StringToBool(InSetting->GetAppliedValue());
	const float MultValue = bInvert ? -1 : 1;
	AddInputAxisMultiplier(EInputDevices::Keyboard, USlateUtils::InputAxisPitchPos, FName(UCustomSettingsUtils::SettingKeyboardMouseInvertYAxisRotate), MultValue);
	AddInputAxisMultiplier(EInputDevices::Keyboard, USlateUtils::InputAxisPitchNeg, FName(UCustomSettingsUtils::SettingKeyboardMouseInvertYAxisRotate), MultValue);
	AddInputAxisMultiplier(EInputDevices::Mouse, USlateUtils::InputAxisPitchPos, FName(UCustomSettingsUtils::SettingKeyboardMouseInvertYAxisRotate), MultValue);
	AddInputAxisMultiplier(EInputDevices::Mouse, USlateUtils::InputAxisPitchNeg, FName(UCustomSettingsUtils::SettingKeyboardMouseInvertYAxisRotate), MultValue);
}

void UCustomSettingsPlayerInput::ActOnKeyboardMouseXAxisRotateSensitivitySettingValueAppliedSuccess(UCustomSetting* InSetting) {
	check(IsValid(InSetting));
	const float MultValue = UCustomSettingsUtils::StringToFloat(InSetting->GetAppliedValue());
	AddInputAxisMultiplier(EInputDevices::Keyboard, USlateUtils::InputAxisYawPos, FName(UCustomSettingsUtils::SettingKeyboardMouseXAxisRotateSensitivity), MultValue);
	AddInputAxisMultiplier(EInputDevices::Keyboard, USlateUtils::InputAxisYawNeg, FName(UCustomSettingsUtils::SettingKeyboardMouseXAxisRotateSensitivity), MultValue);
	AddInputAxisMultiplier(EInputDevices::Mouse, USlateUtils::InputAxisYawPos, FName(UCustomSettingsUtils::SettingKeyboardMouseXAxisRotateSensitivity), MultValue);
	AddInputAxisMultiplier(EInputDevices::Mouse, USlateUtils::InputAxisYawNeg, FName(UCustomSettingsUtils::SettingKeyboardMouseXAxisRotateSensitivity), MultValue);
}

void UCustomSettingsPlayerInput::ActOnKeyboardMouseYAxisRotateSensitivitySettingValueAppliedSuccess(UCustomSetting* InSetting) {
	check(IsValid(InSetting));
	const float MultValue = UCustomSettingsUtils::StringToFloat(InSetting->GetAppliedValue());
	AddInputAxisMultiplier(EInputDevices::Keyboard, USlateUtils::InputAxisPitchPos, FName(UCustomSettingsUtils::SettingKeyboardMouseYAxisRotateSensitivity), MultValue);
	AddInputAxisMultiplier(EInputDevices::Keyboard, USlateUtils::InputAxisPitchNeg, FName(UCustomSettingsUtils::SettingKeyboardMouseYAxisRotateSensitivity), MultValue);
	AddInputAxisMultiplier(EInputDevices::Mouse, USlateUtils::InputAxisPitchPos, FName(UCustomSettingsUtils::SettingKeyboardMouseYAxisRotateSensitivity), MultValue);
	AddInputAxisMultiplier(EInputDevices::Mouse, USlateUtils::InputAxisPitchNeg, FName(UCustomSettingsUtils::SettingKeyboardMouseYAxisRotateSensitivity), MultValue);
}
