/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "SettingWidget.h"
#include "LogCustomSettingsPlugin.h"
#include "CustomSettingsUtils.h"
#include "PlayerSettingsSubsystem.h"
#include "LocalizationUtils.h"
#include "PlayerIdentificationSubsystem.h"
#include "SubsystemUtils.h"
#include "Components/TextBlock.h"
#include "Internationalization/StringTable.h"
#include "SlateUtils.h"
#include "Modules/ModuleManager.h"
#include "DetectCurrentInputDevicePreProcessor.h"
#include "UIAdditionsPlugin.h"
#include "UObject/ConstructorHelpers.h"
#include "HUDCore.h"


// Setup

USettingWidget::USettingWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	ConstructorHelpers::FObjectFinderOptional<UStringTable> Obj(TEXT("/CustomSettingsPlugin/Localization/Text/ST_MainMenu.ST_MainMenu"));
	if (Obj.Succeeded()) {
		ProceduralLocalizationStringTable = Obj.Get();
	}
	else {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Could not set a default string table for ProceduralLocalizationStringTable."));
	}
}

void USettingWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	const FUIAdditionsPluginModule& UIAdditionsPluginModule = FModuleManager::GetModuleChecked<FUIAdditionsPluginModule>(TEXT("UIAdditionsPlugin"));
	const TSharedPtr<FDetectCurrentInputDevicePreProcessor>& Processor = UIAdditionsPluginModule.GetDetectCurrentInputDevicePreProcessor();
	check(Processor.IsValid());
	Processor->OnInputDeviceChanged.AddUObject(this, &USettingWidget::ActOnInputDeviceChanged);
}

// Appearance

void USettingWidget::SetIsEnabled(bool bInIsEnabled) {
	Super::SetIsEnabled(bInIsEnabled);

	SyncAppearanceToValue();
}

// Settings

UPlayerSettingsSubsystem* USettingWidget::GetPlayerSettingsSubsystem() const {
	return PlayerSettingsSubsystem;
}

UCustomSetting* USettingWidget::GetCustomSetting() const {
	return CustomSetting;
}

void USettingWidget::BindToSetting_Internal() {
	// Override if desired.
}

void USettingWidget::BindToSetting(const FString& InSettingName) {
	// Don't bind twice, and bind to a name.
	check(GetCustomSetting() == nullptr);
	check(!InSettingName.IsEmpty());

	// For convenience, cache pointers so we don't need to constantly search for the data.
	PlayerSettingsSubsystem = IsValid(GetOwningLocalPlayer()) ? GetOwningLocalPlayer()->GetSubsystem<UPlayerSettingsSubsystem>() : nullptr;
	check(IsValid(GetPlayerSettingsSubsystem()));

	// Without a setting initializing this widget is be useless (just assert).
	// You should not be using a setting widget if the setting is not registered.
	CustomSetting = GetPlayerSettingsSubsystem()->FindSettingObject(InSettingName);
	// Prepare automated localization.
	ProceduralLocalizationKey = GetCustomSetting()->GetSolverRegistrationInfo().SettingName;
	// Allow widgets to further customize or build themselves to setting data.
	BindToSetting_Internal();
	// Now everything has been built, do basic localization.
	if (IsValid(GetTextTitleWidget())) {
		GetTextTitleWidget()->SetText(GetLocalizedSettingName());
	}
	// Set up delegates.
	GetCustomSetting()->OnValueAppliedSuccess.AddDynamic(this, &USettingWidget::ActOnSettingValueChanged);
	GetCustomSetting()->OnOtherSettingRelevantToSelfCompatibilityChanged.AddDynamic(this, &USettingWidget::ActOnOtherSettingRelevantToSelfCompatibilityChanged);
	GetCustomSetting()->OnSettingLocked.AddDynamic(this, &USettingWidget::ActOnSettingLockedOrUnlocked);
	GetCustomSetting()->OnSettingUnlocked.AddDynamic(this, &USettingWidget::ActOnSettingLockedOrUnlocked);
	if (GetCustomSetting()->HasAppliedValue()) {
		// Instead of calling above delegate methods (they both just call SyncAppearanceToValue):
		SyncAppearanceToValue();
	}

	// Respond when a player is assigned to a profile, because we want to hide some settings which have a global effect for players other than player 1.
	UPlayerIdentificationSubsystem* PIS = USubsystemUtils::GetGameInstanceSubsystem<UPlayerIdentificationSubsystem>(this);
	check(IsValid(PIS));
	const FS_SettingSolverSettingInfo& SettingInfo = PlayerSettingsSubsystem->FindSettingInfo(GetCustomSetting()->GetSolverRegistrationInfo().SettingName);
	if (SettingInfo.bSettingDependsOnGlobalEffect) {
		// Relevant only if this setting affects all players.
		PIS->OnPlayerAssignedToProfile.AddDynamic(this, &USettingWidget::ActOnPlayerAssignedToProfile);
		if (PIS->IsPlayerAssignedToAnyProfile(GetOwningLocalPlayer())) {
			ActOnPlayerAssignedToProfile(PIS->GetProfileAssignedToPlayer(GetOwningLocalPlayer()), GetOwningLocalPlayer());
		}	
	}

	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Bound setting widget to setting: %s"), *InSettingName);
}

// Navigation

void USettingWidget::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) {
	Super::NativeOnAddedToFocusPath(InFocusEvent);
	OnDesiresShowSettingDescription.Broadcast(this);
}

void USettingWidget::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) {
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
	OnStopDesiresShowSettingDescription.Broadcast(this);
}

void USettingWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (IsFocusable()) {
		const int32 UserIndex = USlateUtils::GetSlateUserIndexForPlayerController(GetOwningPlayer());
		const EInputDevices CurDevice = USlateUtils::GetCurrentInputDevice(UserIndex);
		// Checking if currently using a cursor prevents cursor hijacking focus when mouse enters a widget because a keyboard key is scrolling a list.
		const AHUDCore* HUD = IsValid(GetOwningPlayer()) ? Cast<AHUDCore>(GetOwningPlayer()->GetHUD()) : nullptr;
		const bool bIsUsingCursor = CurDevice == EInputDevices::Mouse || (IsValid(HUD) && HUD->IsAnalogCursorValid());
		// Checking if currently using a cursor prevents cursor hijacking focus when mouse enters a widget because a keyboard key is scrolling a list.
		if (bIsUsingCursor) {
			// If this is not done, engine behavior is that a setting widget other than the one hovered on can remain focused.
			// Setting focus on hover solves the following situations:
			// - A UI key navigation navigates from a widget other than the one hovered.
			// - NativeOnMouseEnter not triggering OnDesiresShowSettingDescription when steps: 1. hovering A, 2. navigating with keys to widget B. 3. moving the mouse while still hovering A.
			//
			// Also solved is that the setting description panel must update at all times when logical.
			// Currently there is no setting where inner slate widgets are focusable (they are clickable but that's all), so that is not a concern here.
			SetFocus();
		}
	}
}

// Localization

TSoftObjectPtr<UStringTable> USettingWidget::GetProceduralLocalizationStringTable() const {
	return ProceduralLocalizationStringTable;
}

FString USettingWidget::GetProceduralLocalizationKey() const {
	return ProceduralLocalizationKey;
}

FText USettingWidget::GetLocalizedSettingName_Implementation() const {
	const UStringTable* StringTable = GetProceduralLocalizationStringTable().LoadSynchronous();
	return IsValid(StringTable) ? ULocalizationUtils::LocalizeSettingTitle(StringTable->GetStringTableId(), GetProceduralLocalizationKey()) : FText();
}

FText USettingWidget::GetLocalizedSettingValue_Implementation(const FString& InValue) const {
	if (InValue.IsEmpty()) {
		return FText();
	}
	const UStringTable* StringTable = GetProceduralLocalizationStringTable().LoadSynchronous();
	return IsValid(StringTable) ? ULocalizationUtils::LocalizeSettingValue(StringTable->GetStringTableId(), InValue) : FText();
}

// Widgets

UTextBlock* USettingWidget::GetTextTitleWidget() const {
	return TextTitleWidget;
}

// Appearance

void USettingWidget::SyncAppearanceToValue() {
	// Override if required
}

void USettingWidget::Show() {
	// All setting widgets are visible, so that Slate can navigate back / forth with arrow keys.
	//
	// Not all setting widgets take input directly, at times only inner widgets are focusable (button, key selector).
	// At times it's the other way around (slider, multichoice), where the inner slate widgets are too hardcoded to configure input behavior nicely (action response and handling), and the wrapping setting widget takes over.
	// Changing visibility or focus related behavior must be done with care!
	SetVisibility(ESlateVisibility::Visible);
}

void USettingWidget::Hide(){
	SetVisibility(ESlateVisibility::Collapsed);
}

// Delegates

void USettingWidget::ActOnPlayerAssignedToProfile(UPlayerProfile* InProfile, ULocalPlayer* InPlayer) {
	if (!IsValid(InProfile) || !IsValid(InPlayer)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("InProfile or InPlayer invalid."));
		return;
	}
	if (InPlayer != GetOwningLocalPlayer()) {
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Player profile changed but for a different player. Ignoring."));
		return;
	}

	// This method was bound only if this setting affects all players (it does), so we don't have to check that here.
	// This is done so that Player 2 doesn't see widgets for settings he should not be able to modify, since the setting affects multiple players.
	// Only player 1 is allowed to modify those settings. Other players load those global values and don't manage them.
	bool bIsFirstPlayer = GetOwningLocalPlayer()->GetLocalPlayerIndex() == 0;
	if (bIsFirstPlayer) {
		Show();
	}
	else {
		Hide();
	}
}

void USettingWidget::ActOnSettingValueChanged(UCustomSetting* InCustomSetting) {
	SyncAppearanceToValue();
}

void USettingWidget::ActOnOtherSettingRelevantToSelfCompatibilityChanged(UCustomSetting* InOtherCustomSetting) {
	SyncAppearanceToValue();
}

void USettingWidget::ActOnSettingLockedOrUnlocked(UCustomSetting* InCustomSetting) {
	SyncAppearanceToValue();
}

void USettingWidget::ActOnInputDeviceChanged(EInputDevices InInputDevice, int32 InSlateUserIndex) {
	const int32 UserIndex = USlateUtils::GetSlateUserIndexForPlayerController(GetOwningPlayer());
	if (UserIndex == INDEX_NONE || UserIndex != InSlateUserIndex) {
		// Invalid or another player.
		return;
	}

	if (IsFocusable()) {
		if (InInputDevice == EInputDevices::Mouse && IsHovered()) {
			// See why SetFocus is done on NativeOnMouseEnter.
			// Reason I also call this on ActOnInputDeviceChanged is that it is possible to switch to a currently hovering cursor device without triggering MouseEnter.
			SetFocus();
		}
	}
}