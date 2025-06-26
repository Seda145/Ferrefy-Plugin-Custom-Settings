/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "SettingKeybindingWidget.h"
#include "LogCustomSettingsPlugin.h"
#include "CustomKeybindingSetting.h"
#include "SlateUtils.h"
#include "Kismet/KismetInputLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/PanelWidget.h"
#include "Internationalization/StringTable.h"
#include "UObject/SoftObjectPtr.h"
#include "SettingKeybindingInputKeySelectorsWidget.h"
#include "SettingKeybindingInputKeySelectorWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"


// Setup

USettingKeybindingWidget::USettingKeybindingWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	ConstructorHelpers::FObjectFinderOptional<UStringTable> Obj(TEXT("/UIAdditionsPlugin/Localization/Text/ST_HID.ST_HID"));
	if (Obj.Succeeded()) {
		ProceduralLocalizationStringTable = Obj.Get();
	}
	else {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Could not set a default string table for ProceduralLocalizationStringTable."));
	}
}

// Widgets 

const TMap<FName, USettingKeybindingInputKeySelectorsWidget*>& USettingKeybindingWidget::GetInputKeySelectors() const {
	return InputKeySelectors;
}

TSubclassOf<USettingKeybindingInputKeySelectorsWidget> USettingKeybindingWidget::GetInputKeySelectorsWidgetClass() const {
	return InputKeySelectorsWidgetClass;
}

UPanelWidget* USettingKeybindingWidget::GetInputKeySelectorsPanelWidget() const {
	return InputKeySelectorsPanelWidget;
}

bool USettingKeybindingWidget::GetShowKeyboardAndMouse() const {
	return bShowKeyboardAndMouse;
}

bool USettingKeybindingWidget::GetShowGamepad() const {
	return bShowGamepad;
}

bool USettingKeybindingWidget::CanShowKey(const FKey& InKey, const FName& InInputName) const {
	UCustomKeybindingSetting* CustomKeybindingSetting = Cast<UCustomKeybindingSetting>(GetCustomSetting());
	check(IsValid(CustomKeybindingSetting));
	// Well... I was really hoping for a function library to test all of EInputDevices. 
	// But there doesn't seem to be one in the engine.
	// This is a TODO to support more devices.
	// Look at implementation of blacklist on CustomKeybindingSetting as well.
	if (CustomKeybindingSetting->GetBlacklistInputNames().Contains(InInputName)) {
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Input name is blacklisted: %s"), *InInputName.ToString());
		return false;
	}
	if (CustomKeybindingSetting->GetBlacklistInputKeys().Contains(InKey)) {
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Input key is blacklisted: %s"), *InKey.ToString());
		return false;
	}

	if (UKismetInputLibrary::Key_IsKeyboardKey(InKey)
		|| UKismetInputLibrary::Key_IsMouseButton(InKey)
		) {
		if (GetShowKeyboardAndMouse()) {
			return true;
		}
		else {
			UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Input key is not shown for the desired keyboard/mouse device: %s"), *InKey.ToString());
		}
	}
	else if (UKismetInputLibrary::Key_IsGamepadKey(InKey)) {
		if (GetShowGamepad()) {
			return true;
		}
		else {
			UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Input key is not shown for the desired gamepad devices: %s"), *InKey.ToString());
		}
	}
	return false;
}

// Appearance

void USettingKeybindingWidget::BindToSetting_Internal() {
	UCustomKeybindingSetting* CustomKeybindingSetting = Cast<UCustomKeybindingSetting>(GetCustomSetting());
	check(IsValid(CustomKeybindingSetting));
	// Check we don't do this a second time.
	check(GetInputKeySelectors().Num() == 0);
	// Check a class was configured.
	check(GetInputKeySelectorsWidgetClass() != nullptr);
	check(IsValid(GetInputKeySelectorsPanelWidget()));

	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Binding input key selectors widgets."));

	const APlayerController* PC = GetOwningPlayer();
	const UPlayerInput* Pi = IsValid(PC) ? PC->PlayerInput : nullptr;
	check(IsValid(Pi));

	TSet<FName> MappingNames;

	for (const FInputActionKeyMapping& MapX : Pi->ActionMappings) {
		// Only show the input group if at least one key is on a shown input device.
		if (CanShowKey(MapX.Key, MapX.ActionName)) {
			MappingNames.Add(MapX.ActionName);
		}
	}
	for (const FInputAxisKeyMapping& MapX : Pi->AxisMappings) {
		// Only show the input group if at least one key is on a shown input device.
		if (CanShowKey(MapX.Key, MapX.AxisName)) {
			MappingNames.Add(MapX.AxisName);
		}
	}

	// Thoughts: blacklisted keys / input names could be shown readonly conditionally, but I consider that this then should not be listed under "keybinding".
	// Some likely blacklisted actions (enter for selecting, arrows for navigating) are already familiar to end users (need less to no explaining).

	for (const FName& NameX : MappingNames) {
		USettingKeybindingInputKeySelectorsWidget* InputKeySelectorsWidgetX = CreateWidget<USettingKeybindingInputKeySelectorsWidget>(this, GetInputKeySelectorsWidgetClass());
		check(IsValid(InputKeySelectorsWidgetX));
		// Visually add the new widget to the panel.
		GetInputKeySelectorsPanelWidget()->AddChild(InputKeySelectorsWidgetX);
		// Set up the new widget
		InputKeySelectorsWidgetX->BindToSetting(CustomKeybindingSetting, NameX, this);
		InputKeySelectorsWidgetX->OnRequestKeybindingChange.AddDynamic(CustomKeybindingSetting, &UCustomKeybindingSetting::ActOnInputKeySelectorsWidgetSelects);
		// Register the new widget.
		InputKeySelectors.Add(NameX, InputKeySelectorsWidgetX);
	}
}

// Localization

FText USettingKeybindingWidget::GetLocalizedSettingValue_Implementation(const FString& InValue) const {
	// This setting holds all keybindings collected as a single value. I don't localize it here.
	return FText();
}