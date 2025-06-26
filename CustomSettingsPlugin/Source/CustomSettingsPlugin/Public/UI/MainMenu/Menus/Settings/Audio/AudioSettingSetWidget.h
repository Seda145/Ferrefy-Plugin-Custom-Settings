/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "SettingSetWidget.h"

#include "AudioSettingSetWidget.generated.h"

class USettingSliderWidget;


UCLASS(Abstract, BlueprintType, Blueprintable) 
class CUSTOMSETTINGSPLUGIN_API UAudioSettingSetWidget : public USettingSetWidget {
    GENERATED_BODY()

private:

protected:

    // Setup | Dependencies

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingSliderWidget* Setting_MasterVolume = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingSliderWidget* Setting_MusicVolume = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingSliderWidget* Setting_SFXVolume = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingSliderWidget* Setting_VoiceVolume = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingSliderWidget* Setting_UIVolume = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingSliderWidget* Setting_WeatherVolume = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingSliderWidget* Setting_AmbienceVolume = nullptr;

public:

private:

protected:

    // Setup

    virtual TMap<USettingWidget*, FString> GetWidgetSettingBindingInfo_Implementation() const override;

public:

};