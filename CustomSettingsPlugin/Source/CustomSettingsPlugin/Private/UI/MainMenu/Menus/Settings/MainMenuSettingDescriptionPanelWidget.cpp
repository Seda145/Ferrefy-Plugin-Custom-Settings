/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "MainMenuSettingDescriptionPanelWidget.h"
#include "LogCustomSettingsPlugin.h"
#include "LocalizationUtils.h"
#include "SettingWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Components/TextBlock.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Internationalization/StringTable.h"


// Setup

void UMainMenuSettingDescriptionPanelWidget::NativePreConstruct() {
	Super::NativePreConstruct();

#if WITH_EDITOR
	if (IsDesignTime()) {
		// This widget is not supposed to display data by default which is rare but requires us to clear the panel from any initial / preview data when playing.
		// If we are in IsDesignTime then we should not clear it to preserve any preview state set in blueprints, so we should return here.
		return;
	}
#endif // WITH_EDITOR

	ClearPanel();
}

// Data

void UMainMenuSettingDescriptionPanelWidget::ClearPanel() {
	if (IsValid(TextTitleWidget)) {
		TextTitleWidget->SetText(FText());
	}
	if (IsValid(TextDescriptionWidget)) {
		TextDescriptionWidget->SetText(FText());
	}
}

void UMainMenuSettingDescriptionPanelWidget::FillPanel(const USettingWidget* InSettingWidget) {
	ClearPanel();

	if (!IsValid(InSettingWidget)) {
		return;
	}

	UStringTable* StringTable = InSettingWidget->GetProceduralLocalizationStringTable().LoadSynchronous();
	if (!IsValid(StringTable)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Can't procedurally a setting description, because its ProceduralLocalizationStringTable is invalid."));
		return;
	}

	if (IsValid(TextTitleWidget)) {
		TextTitleWidget->SetText(IsValid(InSettingWidget->GetTextTitleWidget()) ? InSettingWidget->GetTextTitleWidget()->GetText() : FText());
	}
	if (IsValid(TextDescriptionWidget)) {
		TextDescriptionWidget->SetText(ULocalizationUtils::LocalizeSettingDescription(StringTable->GetStringTableId(), InSettingWidget->GetProceduralLocalizationKey()));
	}
}