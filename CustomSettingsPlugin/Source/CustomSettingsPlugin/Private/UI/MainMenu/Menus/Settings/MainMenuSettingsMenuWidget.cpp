/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "MainMenuSettingsMenuWidget.h"
#include "SlateUtils.h"
#include "SettingWidget.h"
#include "SettingSetWidget.h"
#include "LogCustomSettingsPlugin.h"
#include "MainMenuSettingDescriptionPanelWidget.h"
#include "PlayerSettingsSubsystem.h"
#include "CustomSetting.h"
#include "Blueprint/WidgetTree.h"


// Setup

void UMainMenuSettingsMenuWidget::NativeOnInitialized() {
	// Bind default input actions before Super call, so BP designers can alter behavior OnInitialized if required.
	BindRoutedInputAction(USlateUtils::InputActionNavReset, false, true, this, &UMainMenuSettingsMenuWidget::ActOnNavReset);
	
    Super::NativeOnInitialized();

	// Delegates
	if (IsValid(DecisionDialog_ResetSettings)) {
		DecisionDialog_ResetSettings->OnDecided.AddDynamic(this, &UMainMenuSettingsMenuWidget::ActOnDecisionDialog_ResetSettingsDecided);
	}

	for (USettingWidget* SettingWidgetX : FindSettingWidgets()) {
		if (!IsValid(SettingWidgetX)) {
			UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid SettingWidgetX"));
			continue;
		}

		SettingWidgetX->OnDesiresShowSettingDescription.AddDynamic(this, &UMainMenuSettingsMenuWidget::ActOnDesiresShowSettingDescription);
		SettingWidgetX->OnStopDesiresShowSettingDescription.AddDynamic(this, &UMainMenuSettingsMenuWidget::ActOnStopDesiresShowSettingDescription);
	}
}

// Settings

TArray<USettingWidget*> UMainMenuSettingsMenuWidget::FindSettingWidgets() const {
	TArray<USettingWidget*> SettingWidgets = TArray<USettingWidget*>();

	// Loop over all inner widgets but don't dive into other widget trees.
	WidgetTree->ForEachWidget([&SettingWidgets](UWidget* WidgetX) {
		USettingWidget* SettingWidgetX = Cast<USettingWidget>(WidgetX);
		if (IsValid(SettingWidgetX)) {
			// Found a setting directly placed on this menu.
			SettingWidgets.Add(SettingWidgetX);
		}
		else {
			// Try if the widget is a setting set, and if so loop through that widget tree.
			USettingSetWidget* SettingSetWidgetX = Cast<USettingSetWidget>(WidgetX);
			if (IsValid(SettingSetWidgetX)) {
				SettingSetWidgetX->WidgetTree->ForEachWidget([&SettingWidgets](UWidget* WidgetY) {
					USettingWidget* SettingWidgetY = Cast<USettingWidget>(WidgetY);
					if (IsValid(SettingWidgetY)) {
						// Found a setting nested within a setting set. Not looking any deeper.
						SettingWidgets.Add(SettingWidgetY);
					}
				});
			}
		}
	});

	return SettingWidgets;
}

void UMainMenuSettingsMenuWidget::ResetSettings() {
	for (USettingWidget* SettingWidgetX : FindSettingWidgets()) {
		if (!IsValid(SettingWidgetX) || !SettingWidgetX->GetCustomSetting() || !IsValid(SettingWidgetX->GetCustomSetting()->GetPlayerSettingsSubsystem())) {
			UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid SettingWidgetX or GetCustomSetting()"));
			continue;
		}

		SettingWidgetX->GetCustomSetting()->GetPlayerSettingsSubsystem()->ForceApplySaveModeToSettingAndAffected(SettingWidgetX->GetCustomSetting()->GetSolverRegistrationInfo().SettingName);
	}
}

void UMainMenuSettingsMenuWidget::RequestFullSettingsReset() {
	NavigateTo(DecisionDialog_ResetSettings);
}

// Delegates | DecisionDialog

void UMainMenuSettingsMenuWidget::ActOnDecisionDialog_ResetSettingsDecided(E_DecisionDialogDecisions InDecision) {
	if (InDecision == E_DecisionDialogDecisions::Yes) {
		ResetSettings();
	}
}

// Delegates | Setting

void UMainMenuSettingsMenuWidget::ActOnDesiresShowSettingDescription(USettingWidget* InSettingWidget) {
	if (IsValid(SettingDescriptionPanel)) {
		SettingDescriptionPanel->FillPanel(InSettingWidget);
	}
}

void UMainMenuSettingsMenuWidget::ActOnStopDesiresShowSettingDescription(USettingWidget* InSettingWidget) {
	if (IsValid(SettingDescriptionPanel)) {
		SettingDescriptionPanel->ClearPanel();
	}
}

// Delegates | Input

void UMainMenuSettingsMenuWidget::ActOnNavReset() {
	RequestFullSettingsReset();
}