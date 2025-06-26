/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "DisplaySettingSetWidget.h"
#include "SettingWidget.h"
#include "SettingMultiChoiceWidget.h"
#include "SettingSliderWidget.h"
#include "CustomSettingsUtils.h"


// Setup

TMap<USettingWidget*, FString> UDisplaySettingSetWidget::GetWidgetSettingBindingInfo_Implementation() const {
    return {
        { Setting_WindowMode, UCustomSettingsUtils::SettingWindowMode },
        { Setting_DisplayResolution, UCustomSettingsUtils::SettingDisplayResolution },
        { Setting_LockFPS, UCustomSettingsUtils::SettingLockFPS },
        { Setting_VSync, UCustomSettingsUtils::SettingVSync },
        { Setting_Gamma, UCustomSettingsUtils::SettingGamma },
        { Setting_FOV, UCustomSettingsUtils::SettingFOV }
    };
}