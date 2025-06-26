/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "KeyboardAndMouseSettingSetWidget.h"
#include "SettingWidget.h"
#include "SettingMultiChoiceWidget.h"
#include "SettingSliderWidget.h"
#include "CustomSettingsUtils.h"


// Setup

TMap<USettingWidget*, FString> UKeyboardAndMouseSettingSetWidget::GetWidgetSettingBindingInfo_Implementation() const {
    return {
        { Setting_InvertXAxisRotate, UCustomSettingsUtils::SettingKeyboardMouseInvertXAxisRotate },
        { Setting_InvertYAxisRotate, UCustomSettingsUtils::SettingKeyboardMouseInvertYAxisRotate },
        { Setting_XAxisRotateSensitivity, UCustomSettingsUtils::SettingKeyboardMouseXAxisRotateSensitivity },
        { Setting_YAxisRotateSensitivity, UCustomSettingsUtils::SettingKeyboardMouseYAxisRotateSensitivity }
    };
}