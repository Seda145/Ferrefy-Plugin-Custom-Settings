/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "SettingWidget.h"

#include "SettingSliderWidget.generated.h"

class UTextBlock;
class USlider;
class UProgressBar;
class UCustomSetting;
class UCustomSliderSetting;
class UHoverFeedbackWidget;


/**
* Widget which presents a 1D slider to control the value of a single setting.
* The widget uses direct input to navigate back and forward, so the inner slider handle is not used to focus on.
*/
UCLASS(Abstract, BlueprintType, Blueprintable, meta = (DontUseGenericSpawnObject = "True"))
class CUSTOMSETTINGSPLUGIN_API USettingSliderWidget : public USettingWidget {
    GENERATED_BODY()

private:

protected:	

    // Setup | Dependencies

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        USlider* SliderHandleWidget = nullptr;

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UProgressBar* ProgressBarWidget = nullptr;

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        UTextBlock* TextValueWidget = nullptr;

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        UHoverFeedbackWidget* HoverFeedbackWidget = nullptr;

public:

private:

    // Delegates | Widgets

    UFUNCTION()
        void ActOnSliderValueChanged(float InValue);

    // Delegates | Input

    UFUNCTION()
        void ActOnNavLeft();

    UFUNCTION()
        void ActOnNavRight();

protected:

    // Setup

    virtual void NativeOnInitialized() override;

    // Widgets 

    /* Get the Slate widget nested within this UserWidget. */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Widgets")
        USlider* GetSliderHandleWidget() const;

    /* Get the Slate widget nested within this UserWidget. */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Widgets")
        UProgressBar* GetProgressBarWidget() const;

    /* Get the Slate widget nested within this UserWidget. */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Widgets")
        UTextBlock* GetTextValueWidget() const;


    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Widgets")
        UHoverFeedbackWidget* GetHoverFeedbackWidget() const;

    // Appearance

    virtual void SyncAppearanceToValue() override;

public:

    // Localization

    virtual FText GetLocalizedSettingValue_Implementation(const FString& InValue) const override;

};