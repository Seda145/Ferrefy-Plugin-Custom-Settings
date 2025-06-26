/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "SettingWidget.h"
#include "Templates/SubclassOf.h"

#include "SettingKeybindingWidget.generated.h"


class USettingKeybindingInputKeySelectorsWidget;
class UPanelWidget;


/**
* Widget which generates a list of grouped input mappings providing keybinding functionality.
*/
UCLASS(Abstract, BlueprintType, Blueprintable, meta = (DontUseGenericSpawnObject = "True"))
class CUSTOMSETTINGSPLUGIN_API USettingKeybindingWidget : public USettingWidget {
    GENERATED_BODY()

private:

    // Widgets

    UPROPERTY(Transient)
        TMap<FName, USettingKeybindingInputKeySelectorsWidget*> InputKeySelectors;

protected:

    // Widgets

    UPROPERTY(EditAnywhere, Category = "Widgets")
        TSubclassOf<USettingKeybindingInputKeySelectorsWidget> InputKeySelectorsWidgetClass = nullptr;

    UPROPERTY(EditAnywhere, Category = "Widgets")
        bool bShowKeyboardAndMouse = false;

    UPROPERTY(EditAnywhere, Category = "Widgets")
        bool bShowGamepad = false;

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UPanelWidget* InputKeySelectorsPanelWidget = nullptr;

public:

private:

protected:

    // Widgets 

    const TMap<FName, USettingKeybindingInputKeySelectorsWidget*>& GetInputKeySelectors() const;

    TSubclassOf<USettingKeybindingInputKeySelectorsWidget> GetInputKeySelectorsWidgetClass() const;

    bool GetShowKeyboardAndMouse() const;

    bool GetShowGamepad() const;

    // Appearance

    virtual void BindToSetting_Internal() override;

public:

    // Setup

    USettingKeybindingWidget(const FObjectInitializer& ObjectInitializer);

    // Widgets 

    bool CanShowKey(const FKey& InKey, const FName& InInputName) const;

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Widgets")
        UPanelWidget* GetInputKeySelectorsPanelWidget() const;

    // Localization

    virtual FText GetLocalizedSettingValue_Implementation(const FString& InValue) const override;


};