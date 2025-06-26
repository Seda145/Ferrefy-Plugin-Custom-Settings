/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "GameplaySettingSetWidget.h"
#include "SettingWidget.h"
#include "SettingMultiChoiceWidget.h"
#include "SettingSliderWidget.h"
#include "CustomSettingsUtils.h"


// Setup

TMap<USettingWidget*, FString> UGameplaySettingSetWidget::GetWidgetSettingBindingInfo_Implementation() const {
    return {
        { Setting_Difficulty, UCustomSettingsUtils::SettingDifficulty },
    };
}