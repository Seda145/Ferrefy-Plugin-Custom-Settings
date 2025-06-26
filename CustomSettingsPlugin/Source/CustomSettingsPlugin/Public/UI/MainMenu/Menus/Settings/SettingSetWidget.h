/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "SettingSetWidget.generated.h"


class USettingWidget;


/* Container for setting widgets, added by subclasses as widget bindings. This empty class can be used for casting. */
UCLASS(Abstract, BlueprintType, Blueprintable) 
class CUSTOMSETTINGSPLUGIN_API USettingSetWidget : public UUserWidget {
    GENERATED_BODY()

private:

    bool bHasBoundWidgetsToSettings = false;

protected:

public:

private:

    // Setup

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Settings")
        bool GetHasBoundWidgetsToSettings() const;

    void BindWidgetsToSettings();

    // Delegates

    UFUNCTION()
        void ActOnFinishedInitialApplySettings();

protected:

    // Setup

    virtual void NativeOnInitialized() override;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure = false, Category = "Settings")
        TMap<USettingWidget*, FString> GetWidgetSettingBindingInfo() const;

    virtual TMap<USettingWidget*, FString> GetWidgetSettingBindingInfo_Implementation() const;

public:

};