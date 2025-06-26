/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "SettingKeybindingInputKeySelectorsWidget.h"
#include "SettingKeybindingInputKeySelectorWidget.h"
#include "LogCustomSettingsPlugin.h"
#include "SlateUtils.h"
#include "LocalizationUtils.h"
#include "SettingKeybindingWidget.h"
#include "Components/InputKeySelector.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Internationalization/StringTable.h"
#include "CustomKeybindingSetting.h"
#include "UObject/ConstructorHelpers.h"


// Setup

USettingKeybindingInputKeySelectorsWidget::USettingKeybindingInputKeySelectorsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	ConstructorHelpers::FObjectFinderOptional<UStringTable> Obj(TEXT("/UIAdditionsPlugin/Localization/Text/ST_HID.ST_HID"));
	if (Obj.Succeeded()) {
		ProceduralLocalizationStringTable = Obj.Get();
	}
	else {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Could not set a default string table for ProceduralLocalizationStringTable."));
	}
}

UCustomKeybindingSetting* USettingKeybindingInputKeySelectorsWidget::GetCustomSetting() const {
	return Setting;
}

USettingKeybindingWidget* USettingKeybindingInputKeySelectorsWidget::GetSettingKeybindingWidget() const {
	return SettingKeybindingWidget;
}

void USettingKeybindingInputKeySelectorsWidget::BindToSetting(UCustomKeybindingSetting* InSetting, const FName& InInputMappingName, USettingKeybindingWidget* InSettingKeybindingWidget) {
	// Test if all is ok, and we are not binding again.
	check(GetCustomSetting() == nullptr);
	SettingKeybindingWidget = InSettingKeybindingWidget;
	check(IsValid(GetSettingKeybindingWidget()));
	check(IsValid(InSetting));
	check(InInputMappingName != "");
	check(IsValid(GetSettingKeybindingInputKeySelectorWidgetClass()));
	check(IsValid(GetInputNameTitleWidget()));

	Setting = InSetting;
	InputMappingName = InInputMappingName;
	const UStringTable* StrTab = GetProceduralLocalizationStringTable().LoadSynchronous();
	check(IsValid(StrTab));
	GetInputNameTitleWidget()->SetText(ULocalizationUtils::LocalizeInputMappingName(StrTab->GetStringTableId(), GetInputMappingName()));

	// Set up delegates.
	GetCustomSetting()->OnValueAppliedSuccess.AddDynamic(this, &USettingKeybindingInputKeySelectorsWidget::ActOnSettingValueChanged);
	if (GetCustomSetting()->HasAppliedValue()) {
		ActOnSettingValueChanged(GetCustomSetting());
	}
}

// Widgets

TSubclassOf<USettingKeybindingInputKeySelectorWidget> USettingKeybindingInputKeySelectorsWidget::GetSettingKeybindingInputKeySelectorWidgetClass() const {
	return SettingKeybindingInputKeySelectorWidgetClass;
}


UTextBlock* USettingKeybindingInputKeySelectorsWidget::GetInputNameTitleWidget() const {
	return InputNameTitleWidget;
}

UPanelWidget* USettingKeybindingInputKeySelectorsWidget::GetInputKeySelectorPanelWidget() const {
	return InputKeySelectorPanelWidget;
}

// Localization

const FS_KeyMappings& USettingKeybindingInputKeySelectorsWidget::GetKeyMappings() const {
	return KeyMappings;
}

TSoftObjectPtr<UStringTable> USettingKeybindingInputKeySelectorsWidget::GetProceduralLocalizationStringTable() const {
	return ProceduralLocalizationStringTable;
}

FName USettingKeybindingInputKeySelectorsWidget::GetInputMappingName() const {
	return InputMappingName;
}

// Appearance

void USettingKeybindingInputKeySelectorsWidget::SyncAppearanceToValue() {
	// InputKeySelectorPanelWidget is used to display just selector widgets.
	// Important is to not generate a selector for a key that is blacklisted on the setting.

	check(IsValid(GetCustomSetting()))
	check(IsValid(GetSettingKeybindingWidget()))
	if (!IsValid(GetInputKeySelectorPanelWidget())) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid GetInputKeySelectorPanelWidget(). Can't sync appearance to value."));
		return;
	}

	const FS_KeyMappings AllKeyMappings = UCustomSettingsUtils::StringToKeyMappingsStruct(GetCustomSetting()->GetAppliedValue());
	KeyMappings.ActionKeyMappings = AllKeyMappings.ActionKeyMappings.FilterByPredicate([this](const FInputActionKeyMapping& MapX) { return MapX.ActionName == GetInputMappingName(); });
	KeyMappings.AxisKeyMappings = AllKeyMappings.AxisKeyMappings.FilterByPredicate([this](const FInputAxisKeyMapping& MapX) { return MapX.AxisName == GetInputMappingName(); });

	if (GetInputKeySelectorPanelWidget()->GetChildrenCount() == 0) {
		// Then they have to be created initially.
		auto MakeWidget = [this](const FInputChord& InInputChord) {
			if (!GetSettingKeybindingWidget()->CanShowKey(InInputChord.Key, GetInputMappingName())) {
				return;
			}

			USettingKeybindingInputKeySelectorWidget* SelectorWidgetX = CreateWidget<USettingKeybindingInputKeySelectorWidget>(this, GetSettingKeybindingInputKeySelectorWidgetClass());
			check(IsValid(SelectorWidgetX));
			GetInputKeySelectorPanelWidget()->AddChild(SelectorWidgetX);
			SelectorWidgetX->OnInputKeySelected.AddDynamic(this, &USettingKeybindingInputKeySelectorsWidget::ActOnInputKeySelected);
		};

		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Starting initial generation of input key selector widgets."));
		for (const FInputActionKeyMapping& MapX : GetKeyMappings().ActionKeyMappings) {
			MakeWidget(USlateUtils::GetInputChordFromInputActionKeyMapping(MapX));
		}
		for (const FInputAxisKeyMapping& MapX : GetKeyMappings().AxisKeyMappings) {
			MakeWidget(USlateUtils::GetInputChordFromInputAxisKeyMapping(MapX));
		}
	}

	// Sync all Selector widgets.

	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Starting appearance syncs of selector widgets."));
	int32 SyncIndex = 0;
	auto SyncWidget = [this, &SyncIndex](const FInputChord& InChord){
		if (!GetSettingKeybindingWidget()->CanShowKey(InChord.Key, GetInputMappingName())) {
			// The idea is that input selector widgets were not created if their key was blacklisted
			// , so when looping over the current input mappings we still got to skip over the blacklisted ones to find the index of the next not blacklisted one.
			return;
		}

		TArray<UWidget*> Ch = GetInputKeySelectorPanelWidget()->GetAllChildren();
		// It would not be right if we had created a different number of selector widgets before.
		check(Ch.IsValidIndex(SyncIndex));
		USettingKeybindingInputKeySelectorWidget* SelectorWidgetX = Cast<USettingKeybindingInputKeySelectorWidget>(Ch[SyncIndex]);
		check(SelectorWidgetX != nullptr);
		SelectorWidgetX->SyncAppearanceToValue(InChord);
		SyncIndex++;
	};

	for (const FInputActionKeyMapping& MapX : GetKeyMappings().ActionKeyMappings) {
		SyncWidget(USlateUtils::GetInputChordFromInputActionKeyMapping(MapX));
	}
	for (const FInputAxisKeyMapping& MapX : GetKeyMappings().AxisKeyMappings) {
		SyncWidget(USlateUtils::GetInputChordFromInputAxisKeyMapping(MapX));
	}
}

// Delegates

void USettingKeybindingInputKeySelectorsWidget::ActOnSettingValueChanged(UCustomSetting* InSetting) {
	SyncAppearanceToValue();
}

void USettingKeybindingInputKeySelectorsWidget::ActOnInputKeySelected(USettingKeybindingInputKeySelectorWidget* InInputKeySelectorWidget) {
	check(IsValid(InInputKeySelectorWidget));
	check(IsValid(InInputKeySelectorWidget->GetInputKeySelectorWidget()));
	OnRequestKeybindingChange.Broadcast(GetInputMappingName(), InInputKeySelectorWidget->GetSyncedInputChord(), InInputKeySelectorWidget->GetInputKeySelectorWidget()->GetSelectedKey());
}