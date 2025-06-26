/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "AccessibilitySettingSetWidget.h"
#include "SettingWidget.h"
#include "SettingMultiChoiceWidget.h"
#include "SettingSliderWidget.h"
#include "CustomSettingsUtils.h"


// Setup

TMap<USettingWidget*, FString> UAccessibilitySettingSetWidget::GetWidgetSettingBindingInfo_Implementation() const {
    return {
        { Setting_Language, UCustomSettingsUtils::SettingLanguage },
        { Setting_SubtitleMode, UCustomSettingsUtils::SettingSubtitleMode },
        { Setting_SubtitleScale, UCustomSettingsUtils::SettingSubtitleScale },
        { Setting_UIScale, UCustomSettingsUtils::SettingUIScale },
        { Setting_VisionColorDeficiencyType, UCustomSettingsUtils::SettingVisionColorDeficiencyType },
        { Setting_VisionColorDeficiencySeverity, UCustomSettingsUtils::SettingVisionColorDeficiencySeverity }
    };
}
