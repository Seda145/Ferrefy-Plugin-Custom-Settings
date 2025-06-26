/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "KeyBindingSettingSetWidget.h"
#include "SettingKeyBindingWidget.h"
#include "CustomSettingsUtils.h"


// Setup

TMap<USettingWidget*, FString> UKeyBindingSettingSetWidget::GetWidgetSettingBindingInfo_Implementation() const {
    return {
        { Setting_Keybinding, UCustomSettingsUtils::SettingKeybinding }
    };
}
