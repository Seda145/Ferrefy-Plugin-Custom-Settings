/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "SettingKeybindingInputKeySelectorWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/Image.h"
#include "Engine/DataTable.h"
#include "Engine/AssetManager.h"
#include "Components/InputKeySelector.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "LogCustomSettingsPlugin.h"
#include "SettingKeybindingImageWidget.h"
#include "HoverFeedbackWidget.h"
#include "Blueprint/WidgetTree.h"
#include "UObject/SoftObjectPtr.h"


// Setup

USettingKeybindingInputKeySelectorWidget::USettingKeybindingInputKeySelectorWidget(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer) {

	ConstructorHelpers::FObjectFinderOptional<UDataTable> Obj(TEXT("/UIAdditionsPlugin/Data/DataTables/UI/Input/DT_InputActionButtonKeyImages.DT_InputActionButtonKeyImages"));
	if (Obj.Succeeded()) {
		KeybindingImagesDT.DataTable = Obj.Get();
		KeybindingImagesDT.RowName = TEXT("Default");
	}
	else {
		UE_LOG(LogCustomSettingsPlugin, Warning, TEXT("Could not set a default datatable for this widget."));
	}
}


void USettingKeybindingInputKeySelectorWidget::NativeOnInitialized() {
	Super::OnInitialized();

	check(IsValid(GetInputKeySelectorWidget()));

	// Just update the inner input key selector to be as flexible as possible. 
	// The setting should handle conditions, not the widget.
	GetInputKeySelectorWidget()->SetAllowGamepadKeys(true);
	GetInputKeySelectorWidget()->SetAllowModifierKeys(true);

	GetInputKeySelectorWidget()->OnIsSelectingKeyChanged.AddDynamic(this, &USettingKeybindingInputKeySelectorWidget::ActOnIsSelectingKeyChanged);
	GetInputKeySelectorWidget()->OnKeySelected.AddDynamic(this, &USettingKeybindingInputKeySelectorWidget::ActOnKeySelected);
}

// Widgets

UInputKeySelector* USettingKeybindingInputKeySelectorWidget::GetInputKeySelectorWidget() const {
	return InputKeySelectorWidget;
}

UHoverFeedbackWidget* USettingKeybindingInputKeySelectorWidget::GetHoverFeedbackWidget() const {
	return HoverFeedbackWidget;
}

// Appearance

//void USettingKeybindingInputKeySelectorWidget::SetIsEnabled(bool bInIsEnabled) {
//	Super::SetIsEnabled(bInIsEnabled);
//
//  Currently ignoring this situation, as right now keybinding widgets are just expected to be enabled.
//  Something disabled for read-only purposes? Should probably not be a keybinding widget.
// 
//	SyncAppearanceToValue(GetSyncedInputChord());
//}

void USettingKeybindingInputKeySelectorWidget::SyncAppearanceToValue(const FInputChord& InValue) {
	bIsSyncingToValue = true;

	check(IsValid(SettingKeybindingImageWidgetClass));
	check(IsValid(GetInputKeySelectorWidget()));
	check(InValue.Key.IsValid());
	// The input chord synchronized to should be reported by the setting.
	// This is the original value we change from to any new value.
	SyncedInputChord = InValue;

	{
		// Update key textures. Multiple key textures can be added in a row to the panel, to show key + modifier keys.
		check(IsValid(KeybindingImagesPanelWidget));
		KeybindingImagesPanelWidget->ClearChildren();

		auto AddKeyImage = [this](const FKey& InKey) {
			const FS_InputKeyImageData* DataPtr = KeybindingImagesDT.GetRow<FS_InputKeyImageData>(CUR_LOG_CONTEXT);
			const TSoftObjectPtr<UTexture2D>* KeyTexturePtr = DataPtr != nullptr ? DataPtr->KeyTextures.Find(InKey) : nullptr;
			const TSoftObjectPtr<UTexture2D> KeyTexture = KeyTexturePtr != nullptr ? *KeyTexturePtr : nullptr;

			// I'm not loading these async currently, because that randomizes panel addition order.
			UTexture2D* HardKeyTexture = KeyTexture.LoadSynchronous();
			if (HardKeyTexture == nullptr) {
				UE_LOG(LogCustomSettingsPlugin, Warning, TEXT("Missing texture for keybinding image, key %s"), *InKey.ToString());
				return;
			}

			USettingKeybindingImageWidget* KeyImageWidgetX = Cast<USettingKeybindingImageWidget>(CreateWidget(this, SettingKeybindingImageWidgetClass));
			check(IsValid(KeyImageWidgetX) && IsValid(KeyImageWidgetX->GetKeyImageWidget()));
			KeybindingImagesPanelWidget->AddChild(KeyImageWidgetX);

			KeyImageWidgetX->SetVisibility(ESlateVisibility::HitTestInvisible);
			if (KeyTexture.IsValid()) {
				KeyImageWidgetX->GetKeyImageWidget()->SetBrushFromTexture(KeyTexture.Get());
			}
			else {
				KeyImageWidgetX->GetKeyImageWidget()->SetBrushFromTexture(nullptr);
			}
		};

		if (InValue.NeedsControl()) {
			AddKeyImage(EKeys::LeftControl);
		}
		if (InValue.NeedsShift()) {
			AddKeyImage(EKeys::LeftShift);
		}
		if (InValue.NeedsAlt()) {
			AddKeyImage(EKeys::LeftAlt);
		}
		if (InValue.NeedsCommand()) {
			AddKeyImage(EKeys::LeftCommand);
		}
		AddKeyImage(InValue.Key);
	}

	{
		// Update the input key selector
		GetInputKeySelectorWidget()->SetSelectedKey(InValue);
	}

	bIsSyncingToValue = false;
}

void USettingKeybindingInputKeySelectorWidget::AppearStartSelectingKey_Implementation() {
	if (IsValid(KeybindingImagesPanelWidget)) {
		KeybindingImagesPanelWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void USettingKeybindingInputKeySelectorWidget::AppearStopSelectingKey_Implementation() {
	if (IsValid(KeybindingImagesPanelWidget)) {
		KeybindingImagesPanelWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

bool USettingKeybindingInputKeySelectorWidget::GetIsSyncingToValue() const {
	return bIsSyncingToValue;
}

FInputChord USettingKeybindingInputKeySelectorWidget::GetSyncedInputChord() const {
	return SyncedInputChord;
}

// Delegates

void USettingKeybindingInputKeySelectorWidget::ActOnIsSelectingKeyChanged() {
	if (IsValid(GetInputKeySelectorWidget()) && GetInputKeySelectorWidget()->GetIsSelectingKey()) {
		// If selecting a key, visualize this. 
		AppearStartSelectingKey();
	}
	else {
		// If not selecting a key (anymore), the widget will likely be synchronizing to the applied setting value. 
		// Still calling this method for BP designers in case.
		AppearStopSelectingKey();
	}
}

void USettingKeybindingInputKeySelectorWidget::ActOnKeySelected(FInputChord InInputChord) {
	// This setting widget works a little different from other settings, because it does not call "apply" directly on the setting / subsystem.
	// This is because a single setting stores all keybindings.
	if (GetIsSyncingToValue()) {
		// We are synchronizing to a setting, so we abort a request before it becomes a loop back.
		return;
	}
	// Note: UE 5.2 seems to be a bug in SInputKeySelector::OnKeyUp to SInputKeySelector::SelectKey 
	// which refuses to pass on a combo of multiple modifier keys at once.
	// Seems you can do CTRL + I but not CTRL + ALT + I.
	OnInputKeySelected.Broadcast(this);
}