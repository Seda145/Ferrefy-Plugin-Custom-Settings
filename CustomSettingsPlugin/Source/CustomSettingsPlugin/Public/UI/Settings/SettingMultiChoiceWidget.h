/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "SettingWidget.h"

#include "SettingMultiChoiceWidget.generated.h"

class UTextBlock;
class UButtonWidget;
class UHoverFeedbackWidget;


/**
* A Widget which allows navigating through a setting's values in a previous / next style.
* The widget uses direct input to navigate back and forward, so the inner buttons are not used to focus on.
*/
UCLASS(Abstract, BlueprintType, Blueprintable, meta = (DontUseGenericSpawnObject = "True"))
class CUSTOMSETTINGSPLUGIN_API USettingMultiChoiceWidget : public USettingWidget {
    GENERATED_BODY()

private:

protected:

    // Appearance

    UPROPERTY(EditAnywhere, Category = "Appearance")
        bool bWrapAround = false;

    // Widgets

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
		UButtonWidget* ButtonPrevious = nullptr;

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
		UButtonWidget* ButtonNext = nullptr;

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
		UTextBlock* TextValueWidget = nullptr;

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        UHoverFeedbackWidget* HoverFeedbackWidget = nullptr;

public:

private:

protected:

    // Setup

    virtual void NativeOnInitialized() override;

    // Widgets

    /* Get the (U) Slate widget nested within this UserWidget. */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Widgets")
        UButtonWidget* GetButtonPrevious() const;

    /* Get the (U) Slate widget nested within this UserWidget. */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Widgets")
        UButtonWidget* GetButtonNext() const;

    /* Get the (U) Slate widget nested within this UserWidget. */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Widgets")
        UTextBlock* GetTextValueWidget() const;

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Widgets")
        UHoverFeedbackWidget* GetHoverFeedbackWidget() const;

    // Appearance

    /* Synchronize appearance to match the applied value, and to visualize choice (in)compatibilities. */
    virtual void SyncAppearanceToValue() override;

    /* Return if getting a next / previous value should wrap around to the other end of the choices array if bounds are reached. */
    bool GetWrapAround() const;

    // Delegates

    UFUNCTION()
        void ActOnButtonPreviousActivated(UButtonWidget* InButtonWidget);

    UFUNCTION()
        void ActOnButtonNextActivated(UButtonWidget* InButtonWidget);

    // Delegates | Input

    UFUNCTION()
        void ActOnNavPrevious();

    UFUNCTION()
        void ActOnNavNext();

public:

};