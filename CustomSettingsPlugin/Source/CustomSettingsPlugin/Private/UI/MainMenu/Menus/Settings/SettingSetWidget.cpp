/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "SettingSetWidget.h"
#include "SettingWidget.h"
#include "PlayerSettingsSubsystem.h"
#include "LogCustomSettingsPlugin.h"


// Setup

void USettingSetWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	const ULocalPlayer* PL = GetOwningLocalPlayer();
	UPlayerSettingsSubsystem* PSS = IsValid(PL) ? PL->GetSubsystem<UPlayerSettingsSubsystem>() : nullptr;
	if (!IsValid(PSS)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("UPlayerSettingsSubsystem or player used to retrieve it invalid."));
		// Can't set up the setting widgets without it.
		check(false);
	}

	PSS->OnFinishedInitialApplySettings.AddDynamic(this, &USettingSetWidget::ActOnFinishedInitialApplySettings);
	if (PSS->GetHasFinishedInitialApplySettings()) {
		ActOnFinishedInitialApplySettings();
	}
}

TMap<USettingWidget*, FString> USettingSetWidget::GetWidgetSettingBindingInfo_Implementation() const {
	// Override to provide registration information.
	return TMap<USettingWidget*, FString>();
}

bool USettingSetWidget::GetHasBoundWidgetsToSettings() const {
	return bHasBoundWidgetsToSettings;
}

void USettingSetWidget::BindWidgetsToSettings() {
	for (TPair<USettingWidget*, FString>& BindInfoX : GetWidgetSettingBindingInfo()) {
		if (IsValid(BindInfoX.Key)) {
			BindInfoX.Key->BindToSetting(BindInfoX.Value);
		}
	}
	bHasBoundWidgetsToSettings = true;
}

// Delegates

void USettingSetWidget::ActOnFinishedInitialApplySettings() {
	if (!GetHasBoundWidgetsToSettings()) {
		BindWidgetsToSettings();
	}
}