/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "AudioSettingSetWidget.h"
#include "SettingWidget.h"
#include "SettingMultiChoiceWidget.h"
#include "SettingSliderWidget.h"
#include "CustomSettingsUtils.h"


// Setup

TMap<USettingWidget*, FString> UAudioSettingSetWidget::GetWidgetSettingBindingInfo_Implementation() const {
	return {
		{ Setting_MasterVolume, UCustomSettingsUtils::SettingMasterVolume },
		{ Setting_MusicVolume, UCustomSettingsUtils::SettingMusicVolume },
		{ Setting_SFXVolume, UCustomSettingsUtils::SettingSFXVolume },
		{ Setting_VoiceVolume, UCustomSettingsUtils::SettingVoiceVolume },
		{ Setting_UIVolume, UCustomSettingsUtils::SettingUIVolume },
		{ Setting_WeatherVolume, UCustomSettingsUtils::SettingWeatherVolume },
		{ Setting_AmbienceVolume, UCustomSettingsUtils::SettingAmbienceVolume }
	};
}