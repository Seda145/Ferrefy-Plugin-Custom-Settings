/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "GamepadSettingSetWidget.h"
#include "SettingWidget.h"
#include "SettingMultiChoiceWidget.h"
#include "SettingSliderWidget.h"
#include "CustomSettingsUtils.h"


// Setup

TMap<USettingWidget*, FString> UGamepadSettingSetWidget::GetWidgetSettingBindingInfo_Implementation() const {
    return {
        { Setting_InvertXAxisRotate, UCustomSettingsUtils::SettingGamepadInvertXAxisRotate },
        { Setting_InvertYAxisRotate, UCustomSettingsUtils::SettingGamepadInvertYAxisRotate },
        { Setting_XAxisRotateSensitivity, UCustomSettingsUtils::SettingGamepadXAxisRotateSensitivity },
        { Setting_YAxisRotateSensitivity, UCustomSettingsUtils::SettingGamepadYAxisRotateSensitivity },
        { Setting_Rumble, UCustomSettingsUtils::SettingGamepadRumble }
    };
}