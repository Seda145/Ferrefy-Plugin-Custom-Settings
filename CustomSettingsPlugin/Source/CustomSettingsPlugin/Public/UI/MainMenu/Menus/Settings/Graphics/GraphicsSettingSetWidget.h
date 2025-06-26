/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "SettingSetWidget.h"

#include "GraphicsSettingSetWidget.generated.h"

class USettingMultiChoiceWidget;
class USettingSliderWidget;


UCLASS(Abstract, BlueprintType, Blueprintable) 
class CUSTOMSETTINGSPLUGIN_API UGraphicsSettingSetWidget : public USettingSetWidget {
    GENERATED_BODY()

private:

protected:

    // Setup | Dependencies

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        USettingMultiChoiceWidget* Setting_RenderHardwareInterface = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        USettingMultiChoiceWidget* Setting_OverallQualityLevel = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_TextureQuality = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_AntiAliasingMode = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_AntiAliasingQuality = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_VisualEffectQuality = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_ShadowQuality = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_FoliageQuality = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_ShadingQuality = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_PostProcessingQuality = nullptr;

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_GlobalIlluminationQuality = nullptr;

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_ReflectionQuality = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_ViewDistanceQuality = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_MotionBlur = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_LensFlares = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_SSR = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_Bloom = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_AmbientOcclusion = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_RayTracedShadows = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_RayTracedReflections = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_RayTracedTranslucency = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_RayTracedAmbientOcclusion = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_RayTracedGlobalIllumination = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingSliderWidget* Setting_ResolutionScaling = nullptr;

public:

private:

protected:

    // Setup

    virtual TMap<USettingWidget*, FString> GetWidgetSettingBindingInfo_Implementation() const override;

public:

};