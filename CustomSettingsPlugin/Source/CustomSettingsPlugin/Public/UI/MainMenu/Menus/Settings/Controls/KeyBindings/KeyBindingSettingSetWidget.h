/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "SettingSetWidget.h"

#include "KeybindingSettingSetWidget.generated.h"


class USettingKeybindingWidget;
class USettingWidget;


UCLASS(Abstract, BlueprintType, Blueprintable) 
class CUSTOMSETTINGSPLUGIN_API UKeyBindingSettingSetWidget : public USettingSetWidget {
    GENERATED_BODY()

private:

protected:

    // Setup | Dependencies

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        USettingKeybindingWidget* Setting_Keybinding = nullptr;

public:

private:

protected:

	// Setup

    virtual TMap<USettingWidget*, FString> GetWidgetSettingBindingInfo_Implementation() const override;

public:

};