/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "SettingMultiChoiceWidget.h"
#include "LogCustomSettingsPlugin.h"
#include "CustomSetting.h"
#include "PlayerSettingsSubsystem.h"
#include "SlateUtils.h"
#include "ButtonWidget.h"
#include "Components/TextBlock.h"
#include "PlayerSettingsSubsystem.h"
#include "HoverFeedbackWidget.h"


// Setup

void USettingMultiChoiceWidget::NativeOnInitialized() {
	// Bind default input actions before Super call, so BP designers can alter behavior OnInitialized if required.
	BindRoutedInputAction(USlateUtils::InputActionNavLeft, true, false, this, &USettingMultiChoiceWidget::ActOnNavPrevious);
	BindRoutedInputAction(USlateUtils::InputActionNavRight, true, false, this, &USettingMultiChoiceWidget::ActOnNavNext);	

	Super::NativeOnInitialized();

	if (IsValid(GetButtonPrevious())) {
		GetButtonPrevious()->OnButtonReleased.AddDynamic(this, &USettingMultiChoiceWidget::ActOnButtonPreviousActivated);
	}
	if (IsValid(GetButtonNext())) {
		GetButtonNext()->OnButtonReleased.AddDynamic(this, &USettingMultiChoiceWidget::ActOnButtonNextActivated);
	}
}

// Widgets

UButtonWidget* USettingMultiChoiceWidget::GetButtonPrevious() const {
	return ButtonPrevious;
}

UButtonWidget* USettingMultiChoiceWidget::GetButtonNext() const {
	return ButtonNext;
}

UTextBlock* USettingMultiChoiceWidget::GetTextValueWidget() const {
	return TextValueWidget;
}

UHoverFeedbackWidget* USettingMultiChoiceWidget::GetHoverFeedbackWidget() const {
	return HoverFeedbackWidget;
}

// Appearance

void USettingMultiChoiceWidget::SyncAppearanceToValue() {
	Super::SyncAppearanceToValue();

	check(IsValid(GetCustomSetting()));

	if (IsValid(GetTextValueWidget())) {
		GetTextValueWidget()->SetText(GetLocalizedSettingValue(GetCustomSetting()->GetAppliedValue()));
	}

	bool bHasPrev = false;
	bool bHasNext = false;
	bool bDisableInner = !GetIsEnabled();
	if (!bDisableInner) {
		FString OutChoice = "";
		bHasPrev = GetCustomSetting()->FindApplicableValueInDirection(false, GetWrapAround(), OutChoice);
		bHasNext = GetCustomSetting()->FindApplicableValueInDirection(true, GetWrapAround(), OutChoice);
	}
	if (!bHasPrev && !bHasNext) {
		bDisableInner = true;
	}

	if (IsValid(GetHoverFeedbackWidget())) {
		GetHoverFeedbackWidget()->SetIsEnabled(!bDisableInner);
	}
	if (IsValid(GetButtonPrevious())) {
		GetButtonPrevious()->SetIsEnabled(!bDisableInner && bHasPrev);
	}
	if (IsValid(GetButtonNext())) {
		GetButtonNext()->SetIsEnabled(!bDisableInner && bHasNext);
	}
}

bool USettingMultiChoiceWidget::GetWrapAround() const {
	return bWrapAround;
}

// Delegates

void USettingMultiChoiceWidget::ActOnButtonPreviousActivated(UButtonWidget* InButtonWidget) {
	ActOnNavPrevious();
}

void USettingMultiChoiceWidget::ActOnButtonNextActivated(UButtonWidget* InButtonWidget) {
	ActOnNavNext();
}

// Delegates | Input

void USettingMultiChoiceWidget::ActOnNavPrevious() {
	if (GetIsEnabled() && IsValid(GetCustomSetting())) {
		GetCustomSetting()->TryApplySettingInDirection(false, GetWrapAround());
	}
}

void USettingMultiChoiceWidget::ActOnNavNext() {
	if (GetIsEnabled() && IsValid(GetCustomSetting())) {
		GetCustomSetting()->TryApplySettingInDirection(true, GetWrapAround());
	}
}