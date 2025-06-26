/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "CustomSettingsPluginSettings.h"
#include "SlateUtils.h"
// Audio
#include "AmbienceVolumeSetting.h"
#include "MasterVolumeSetting.h"
#include "MusicVolumeSetting.h"
#include "SFXVolumeSetting.h"
#include "UIVolumeSetting.h"
#include "VoiceVolumeSetting.h"
#include "WeatherVolumeSetting.h"
// Display
#include "FOVSetting.h"
#include "GammaSetting.h"
#include "LockFPSSetting.h"
#include "DisplayResolutionSetting.h"
#include "VSyncSetting.h"
#include "WindowModeSetting.h"
// Game
#include "DifficultySetting.h"
// Graphics
#include "AmbientOcclusionSetting.h"
#include "AntiAliasingModeSetting.h"
#include "AntiAliasingQualitySetting.h"
#include "BloomSetting.h"
#include "FoliageQualitySetting.h"
#include "GlobalIlluminationQualitySetting.h"
#include "LensFlaresSetting.h"
#include "MotionBlurSetting.h"
#include "OverallQualityLevelSetting.h"
#include "PostProcessingQualitySetting.h"
#include "RayTracedAmbientOcclusionSetting.h"
#include "RayTracedGlobalIlluminationSetting.h"
#include "RayTracedReflectionsSetting.h"
#include "RayTracedShadowsSetting.h"
#include "RayTracedTranslucencySetting.h"
#include "ReflectionQualitySetting.h"
#include "RenderHardwareInterfaceSetting.h"
#include "ResolutionScalingSetting.h"
#include "ShadingQualitySetting.h"
#include "ShadowQualitySetting.h"
#include "SSRSetting.h"
#include "TextureQualitySetting.h"
#include "ViewDistanceQualitySetting.h"
#include "VisualEffectQualitySetting.h"
// Controls Keyboard Mouse
#include "KeyboardMouseInvertXAxisRotateSetting.h"
#include "KeyboardMouseInvertYAxisRotateSetting.h"
#include "KeyboardMouseXAxisRotateSensitivitySetting.h"
#include "KeyboardMouseYAxisRotateSensitivitySetting.h"
// Controls Gamepad
#include "GamepadInvertXAxisRotateSetting.h"
#include "GamepadInvertYAxisRotateSetting.h"
#include "GamepadXAxisRotateSensitivitySetting.h"
#include "GamepadYAxisRotateSensitivitySetting.h"
#include "GamepadRumbleSetting.h"
// Controls | Keybinding
#include "CustomKeybindingSetting.h"
// Accessibility
#include "LanguageSetting.h"
#include "SubtitleScaleSetting.h"
#include "SubtitleScaleLegacySetting.h"
#include "SubtitleModeSetting.h"
#include "UIScaleSetting.h"
#include "VisionColorDeficiencySeveritySetting.h"
#include "VisionColorDeficiencyTypeSetting.h"


UCustomSettingsPluginSettings::UCustomSettingsPluginSettings() {
	// Set up custom settings
	InstallCustomSettings();
}

void UCustomSettingsPluginSettings::InstallCustomSettings() {
	// Audio
	CustomSettingClasses.Add(UAmbienceVolumeSetting::StaticClass());
	CustomSettingClasses.Add(UMasterVolumeSetting::StaticClass());
	CustomSettingClasses.Add(UMusicVolumeSetting::StaticClass());
	CustomSettingClasses.Add(USFXVolumeSetting::StaticClass());
	CustomSettingClasses.Add(UUIVolumeSetting::StaticClass());
	CustomSettingClasses.Add(UVoiceVolumeSetting::StaticClass());
	CustomSettingClasses.Add(UWeatherVolumeSetting::StaticClass());
	// Display
	CustomSettingClasses.Add(UFOVSetting::StaticClass());
	CustomSettingClasses.Add(UGammaSetting::StaticClass());
	CustomSettingClasses.Add(ULockFPSSetting::StaticClass());
	CustomSettingClasses.Add(UDisplayResolutionSetting::StaticClass());
	CustomSettingClasses.Add(UVSyncSetting::StaticClass());
	CustomSettingClasses.Add(UWindowModeSetting::StaticClass());
	// Game
	CustomSettingClasses.Add(UDifficultySetting::StaticClass());
	// Graphics
	CustomSettingClasses.Add(UAmbientOcclusionSetting::StaticClass());
	CustomSettingClasses.Add(UAntiAliasingModeSetting::StaticClass());
	CustomSettingClasses.Add(UAntiAliasingQualitySetting::StaticClass());
	CustomSettingClasses.Add(UBloomSetting::StaticClass());
	CustomSettingClasses.Add(UFoliageQualitySetting::StaticClass());
	CustomSettingClasses.Add(UGlobalIlluminationQualitySetting::StaticClass());
	CustomSettingClasses.Add(ULensFlaresSetting::StaticClass());
	CustomSettingClasses.Add(UMotionBlurSetting::StaticClass());
	CustomSettingClasses.Add(UOverallQualityLevelSetting::StaticClass());
	CustomSettingClasses.Add(UPostProcessingQualitySetting::StaticClass());
	CustomSettingClasses.Add(URayTracedAmbientOcclusionSetting::StaticClass());
	CustomSettingClasses.Add(URayTracedGlobalIlluminationSetting::StaticClass());
	CustomSettingClasses.Add(URayTracedReflectionsSetting::StaticClass());
	CustomSettingClasses.Add(URayTracedShadowsSetting::StaticClass());
	CustomSettingClasses.Add(URayTracedTranslucencySetting::StaticClass());
	CustomSettingClasses.Add(UReflectionQualitySetting::StaticClass());
	CustomSettingClasses.Add(URenderHardwareInterfaceSetting::StaticClass());
	CustomSettingClasses.Add(UResolutionScalingSetting::StaticClass());
	CustomSettingClasses.Add(UShadingQualitySetting::StaticClass());
	CustomSettingClasses.Add(UShadowQualitySetting::StaticClass());
	CustomSettingClasses.Add(USSRSetting::StaticClass());
	CustomSettingClasses.Add(UTextureQualitySetting::StaticClass());
	CustomSettingClasses.Add(UViewDistanceQualitySetting::StaticClass());
	CustomSettingClasses.Add(UVisualEffectQualitySetting::StaticClass());
	// Controls Keyboard Mouse
	CustomSettingClasses.Add(UKeyboardMouseInvertXAxisRotateSetting::StaticClass());
	CustomSettingClasses.Add(UKeyboardMouseInvertYAxisRotateSetting::StaticClass());
	CustomSettingClasses.Add(UKeyboardMouseXAxisRotateSensitivitySetting::StaticClass());
	CustomSettingClasses.Add(UKeyboardMouseYAxisRotateSensitivitySetting::StaticClass());
	// Controls Gamepad
	CustomSettingClasses.Add(UGamepadInvertXAxisRotateSetting::StaticClass());
	CustomSettingClasses.Add(UGamepadInvertYAxisRotateSetting::StaticClass());
	CustomSettingClasses.Add(UGamepadXAxisRotateSensitivitySetting::StaticClass());
	CustomSettingClasses.Add(UGamepadYAxisRotateSensitivitySetting::StaticClass());
	CustomSettingClasses.Add(UGamepadRumbleSetting::StaticClass());
	// Controls | Keybinding
	CustomSettingClasses.Add(UCustomKeybindingSetting::StaticClass());
	// Accessibility
	CustomSettingClasses.Add(ULanguageSetting::StaticClass());
	//.. Pick normal or legacy version, not both.
	//CustomSettingClasses.Add(USubtitleScaleSetting::StaticClass());
	CustomSettingClasses.Add(USubtitleScaleLegacySetting::StaticClass());
	CustomSettingClasses.Add(USubtitleModeSetting::StaticClass());
	CustomSettingClasses.Add(UUIScaleSetting::StaticClass());
	CustomSettingClasses.Add(UVisionColorDeficiencySeveritySetting::StaticClass());
	CustomSettingClasses.Add(UVisionColorDeficiencyTypeSetting::StaticClass());
}
