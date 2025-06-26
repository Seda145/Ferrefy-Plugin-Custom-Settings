/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "GraphicsSettingSetWidget.h"
#include "SettingWidget.h"
#include "SettingMultiChoiceWidget.h"
#include "SettingSliderWidget.h"
#include "CustomSettingsUtils.h"


// Setup

TMap<USettingWidget*, FString> UGraphicsSettingSetWidget::GetWidgetSettingBindingInfo_Implementation() const {
	return {
		{ Setting_AmbientOcclusion, UCustomSettingsUtils::SettingAmbientOcclusion },
		{ Setting_AntiAliasingMode, UCustomSettingsUtils::SettingAntiAliasingMode },
		{ Setting_AntiAliasingQuality, UCustomSettingsUtils::SettingAntiAliasingQuality },
		{ Setting_Bloom, UCustomSettingsUtils::SettingBloom },
		{ Setting_FoliageQuality, UCustomSettingsUtils::SettingFoliageQuality },
		{ Setting_GlobalIlluminationQuality, UCustomSettingsUtils::SettingGlobalIlluminationQuality },
		{ Setting_LensFlares, UCustomSettingsUtils::SettingLensFlares },
		{ Setting_MotionBlur, UCustomSettingsUtils::SettingMotionBlur },
		{ Setting_OverallQualityLevel, UCustomSettingsUtils::SettingOverallQualityLevel },
		{ Setting_PostProcessingQuality, UCustomSettingsUtils::SettingPostProcessingQuality },
		{ Setting_RayTracedAmbientOcclusion, UCustomSettingsUtils::SettingRaytracedAmbientOcclusion },
		{ Setting_RayTracedGlobalIllumination, UCustomSettingsUtils::SettingRaytracedGlobalIllumination },
		{ Setting_RayTracedReflections, UCustomSettingsUtils::SettingRaytracedReflections },
		{ Setting_RayTracedShadows, UCustomSettingsUtils::SettingRaytracedShadows },
		{ Setting_RayTracedTranslucency, UCustomSettingsUtils::SettingRaytracedTranslucency },
		{ Setting_ReflectionQuality, UCustomSettingsUtils::SettingReflectionQuality },
		{ Setting_RenderHardwareInterface, UCustomSettingsUtils::SettingRenderHardwareInterface },
		{ Setting_ResolutionScaling, UCustomSettingsUtils::SettingResolutionScale },
		{ Setting_ShadingQuality, UCustomSettingsUtils::SettingShadingQuality },
		{ Setting_ShadowQuality, UCustomSettingsUtils::SettingShadowQuality },
		{ Setting_SSR, UCustomSettingsUtils::SettingSSR },
		{ Setting_TextureQuality, UCustomSettingsUtils::SettingTextureQuality },
		{ Setting_ViewDistanceQuality, UCustomSettingsUtils::SettingViewDistanceQuality },
		{ Setting_VisualEffectQuality, UCustomSettingsUtils::SettingVisualEffectQuality }
	};
}