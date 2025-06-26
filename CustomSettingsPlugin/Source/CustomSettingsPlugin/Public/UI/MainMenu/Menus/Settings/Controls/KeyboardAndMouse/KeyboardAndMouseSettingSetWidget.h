/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "SettingSetWidget.h"

#include "KeyboardAndMouseSettingSetWidget.generated.h"


class USettingMultiChoiceWidget;
class USettingSliderWidget;
class USettingWidget;


UCLASS(Abstract, BlueprintType, Blueprintable) 
class CUSTOMSETTINGSPLUGIN_API UKeyboardAndMouseSettingSetWidget : public USettingSetWidget {
    GENERATED_BODY()

private:

protected:

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_InvertXAxisRotate = nullptr;

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingMultiChoiceWidget* Setting_InvertYAxisRotate = nullptr;

     /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingSliderWidget* Setting_XAxisRotateSensitivity = nullptr;

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingSliderWidget* Setting_YAxisRotateSensitivity = nullptr;

public:

private:

protected:

    // Setup

    virtual TMap<USettingWidget*, FString> GetWidgetSettingBindingInfo_Implementation() const override;

public:

};