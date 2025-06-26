/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "SettingSetWidget.h"

#include "AccessibilitySettingSetWidget.generated.h"


class USettingMultiChoiceWidget;
class USettingSliderWidget;
class USettingWidget;


UCLASS(Abstract, BlueprintType, Blueprintable) 
class CUSTOMSETTINGSPLUGIN_API UAccessibilitySettingSetWidget : public USettingSetWidget {
    GENERATED_BODY()

private:

protected:

    // Setup | Dependencies

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_Language = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_SubtitleMode = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_SubtitleScale = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_UIScale = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_VisionColorDeficiencyType = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingSliderWidget* Setting_VisionColorDeficiencySeverity = nullptr;

public:

private:

protected:

    // Setup

    virtual TMap<USettingWidget*, FString> GetWidgetSettingBindingInfo_Implementation() const override;

public:

};