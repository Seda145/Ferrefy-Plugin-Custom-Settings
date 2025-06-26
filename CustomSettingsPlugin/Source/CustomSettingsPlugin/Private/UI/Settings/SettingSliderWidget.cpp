/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "SettingSliderWidget.h"
#include "SlateUtils.h"
#include "Components/ProgressBar.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "LogCustomSettingsPlugin.h"
#include "PlayerSettingsSubsystem.h"
#include "CustomSetting.h"
#include "CustomSliderSetting.h"
#include "CustomSettingsUtils.h"
#include "HoverFeedbackWidget.h"
#include "Kismet/KismetMathLibrary.h"


// Setup

void USettingSliderWidget::NativeOnInitialized() {
	// Bind default input actions before Super call, so BP designers can alter behavior OnInitialized if required.
	BindRoutedInputAction(USlateUtils::InputActionNavLeft, true, false, this, &USettingSliderWidget::ActOnNavLeft);
	BindRoutedInputAction(USlateUtils::InputActionNavRight, true, false, this, &USettingSliderWidget::ActOnNavRight);

	Super::NativeOnInitialized();

	// Widget bindings.
	if (IsValid(GetSliderHandleWidget())) {
		GetSliderHandleWidget()->OnValueChanged.AddDynamic(this, &USettingSliderWidget::ActOnSliderValueChanged);
		// I'm not using GetSliderHandleWidget()->MouseUsesStep because it has bad mouse movement handling (mouse vs handle is not 1:1), so we must implement stepsize here.
		GetSliderHandleWidget()->MouseUsesStep = false;
	}
	else {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("GetSliderHandleWidget invalid."));
	}
}

// Widgets 

USlider* USettingSliderWidget::GetSliderHandleWidget() const {
	return SliderHandleWidget;
}

UProgressBar* USettingSliderWidget::GetProgressBarWidget() const {
	return ProgressBarWidget;
}

UTextBlock* USettingSliderWidget::GetTextValueWidget() const {
	return TextValueWidget;
}

UHoverFeedbackWidget* USettingSliderWidget::GetHoverFeedbackWidget() const {
	return HoverFeedbackWidget;
}

// Appearance

void USettingSliderWidget::SyncAppearanceToValue() {
	Super::SyncAppearanceToValue();

	const UCustomSliderSetting* CustomSliderSetting = Cast<UCustomSliderSetting>(GetCustomSetting());
	check(IsValid(CustomSliderSetting));

	if (IsValid(GetSliderHandleWidget())) {
		GetSliderHandleWidget()->SetStepSize(CustomSliderSetting->GetStepSize());
		GetSliderHandleWidget()->SetMinValue(CustomSliderSetting->GetMinValue());
		GetSliderHandleWidget()->SetMaxValue(CustomSliderSetting->GetMaxValue());
		GetSliderHandleWidget()->SetValue(FMath::Clamp(UCustomSettingsUtils::StringToFloat(CustomSliderSetting->GetAppliedValue()), CustomSliderSetting->GetMinValue(), CustomSliderSetting->GetMaxValue()));

		if (IsValid(GetProgressBarWidget())) {
			GetProgressBarWidget()->SetPercent(GetSliderHandleWidget()->GetNormalizedValue());
		}
	}

	if (IsValid(GetTextValueWidget())) {
		GetTextValueWidget()->SetText(GetLocalizedSettingValue(GetCustomSetting()->GetAppliedValue()));
	}

	bool bHasPrev = false;
	bool bHasNext = false;
	bool bDisableInner = !GetIsEnabled();
	if (!bDisableInner) {
		FString OutChoice = "";
		bHasPrev = CustomSliderSetting->FindApplicableValueInDirection(false, OutChoice);
		bHasNext = CustomSliderSetting->FindApplicableValueInDirection(true, OutChoice);
	}
	if (!bHasPrev && !bHasNext) {
		bDisableInner = true;
	}

	if (IsValid(GetHoverFeedbackWidget())) {
		GetHoverFeedbackWidget()->SetIsEnabled(!bDisableInner);
	}
	if (IsValid(GetSliderHandleWidget())) {
		GetSliderHandleWidget()->SetIsEnabled(!bDisableInner);
	}
	if (IsValid(GetProgressBarWidget())) {
		GetProgressBarWidget()->SetIsEnabled(!bDisableInner);
	}
}

// Localization

FText USettingSliderWidget::GetLocalizedSettingValue_Implementation(const FString& InValue) const {
	// Build a string to display the value with on the slider:
	FString ValueString = FString::SanitizeFloat(FCString::Atof(*InValue), 0);
	// The fraction character, which seems to always be '.'.
	const int32 FracPos = ValueString.Find(".");
	if (FracPos != INDEX_NONE) {
		// Get the value string with the fraction length based on the step size, to display a practical precision automatically.
		// By retrieving the first leading fractional 0 we can turn a float like 0.37200002 into 0.372 just for simple display purposes.
		int32 RemoveAtPosition = ValueString.Find("0", ESearchCase::IgnoreCase, ESearchDir::FromStart, FracPos);
		if (RemoveAtPosition != INDEX_NONE) {
			if (RemoveAtPosition == FracPos + 1) {
				// Remove the fraction character too.
				RemoveAtPosition = FracPos;
			}
			else if (RemoveAtPosition > FracPos + 2) {
				// Clamp to a max length of 2, assuming we use stepsize 0.01 as a minimum.
				RemoveAtPosition = FracPos + 2;
			}
		}
		else {
			// No leading zero was found in the fraction. Clamp to a max length of 2, assuming we use stepsize 0.01 as a minimum.
			RemoveAtPosition = FracPos + 2;
		}

		ValueString.RemoveAt(RemoveAtPosition, ValueString.Len());
	}

	return FText::FromString(ValueString);
}

// Delegates

void USettingSliderWidget::ActOnSliderValueChanged(float InValue) {
	UCustomSliderSetting* CustomSliderSetting = Cast<UCustomSliderSetting>(GetCustomSetting());
	if (!GetIsEnabled() || !IsValid(CustomSliderSetting) || !CustomSliderSetting->HasAppliedValue()) {
		// Ignore if disabled, or if the CustomSetting has not applied any value yet.
		return;
	}

	if (CustomSliderSetting->GetAppliedValue() == UCustomSettingsUtils::FloatToString(GetSliderHandleWidget()->GetValue())) {
		// Return if nothing changed compared to the setting to prevent any reapplication.
		return;
	}

	// Not using GetSliderHandleWidget()->MouseUsesStep because it has bad mouse movement handling (mouse vs handle is not 1:1), so we must implement stepsize here a different way.
	double OutRemainder = 0.f;
	// Floors down to full step count.
	int32 Result = UKismetMathLibrary::FMod(InValue, CustomSliderSetting->GetStepSize(), OutRemainder);
    if (OutRemainder > CustomSliderSetting->GetStepSize() / 2) {
        // Round up to next step if remainder is more than half a step.
		Result++;
	}
	// Get new value of steps * step size.
	float NewValue = Result * CustomSliderSetting->GetStepSize();

	// We want to clamp here to not error on UCustomSetting::CanApply, where unclamped / invalid values are guarded against.
	NewValue = FMath::Clamp(NewValue, CustomSliderSetting->GetMinValue(), CustomSliderSetting->GetMaxValue());

	CustomSliderSetting->ApplyValue(UCustomSettingsUtils::FloatToString(NewValue));
}

// Delegates | Input

void USettingSliderWidget::ActOnNavLeft() {
	if (GetIsEnabled()) {
		UCustomSliderSetting* CustomSliderSetting = Cast<UCustomSliderSetting>(GetCustomSetting());
		if (IsValid(CustomSliderSetting)) {
			CustomSliderSetting->TryApplySettingInDirection(false);
		}
	}
}

void USettingSliderWidget::ActOnNavRight() {
	if (GetIsEnabled()) {
		UCustomSliderSetting* CustomSliderSetting = Cast<UCustomSliderSetting>(GetCustomSetting());
		if (IsValid(CustomSliderSetting)) {
			CustomSliderSetting->TryApplySettingInDirection(true);
		}
	}
}
