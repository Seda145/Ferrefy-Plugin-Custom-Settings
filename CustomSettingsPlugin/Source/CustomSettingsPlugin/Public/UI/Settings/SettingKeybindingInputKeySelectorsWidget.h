/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "CustomSettingsUtils.h"
#include "Blueprint/UserWidget.h"
#include "Templates/SubclassOf.h"
#include "UObject/SoftObjectPtr.h"

#include "SettingKeybindingInputKeySelectorsWidget.generated.h"


class USettingKeybindingInputKeySelectorWidget;
class UPanelWidget;
class UTextBlock;
class UStringTable;
class USettingKeybindingWidget;
class UCustomSetting;
class UCustomKeybindingSetting;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRequestKeybindingChange, const FName&, InInputName, const FInputChord&, InOldValue, const FInputChord&, InNewValue);


/**
* Widget which groups input keys relevant to a specific input mapping, providing keybinding functionality.
*/
UCLASS(Abstract, BlueprintType, Blueprintable, meta = (DontUseGenericSpawnObject = "True"))
class CUSTOMSETTINGSPLUGIN_API USettingKeybindingInputKeySelectorsWidget : public UUserWidget {
    GENERATED_BODY()

private:

    // Settings

    UPROPERTY(Transient)
        UCustomKeybindingSetting* Setting = nullptr;

     UPROPERTY(Transient)
        USettingKeybindingWidget* SettingKeybindingWidget = nullptr;

    // Localization

    UPROPERTY(Transient)
        FName InputMappingName = "";

    UPROPERTY(Transient)
        FS_KeyMappings KeyMappings = FS_KeyMappings();

protected:

    // Widgets

    UPROPERTY(EditAnywhere, Category = "Widgets")
        TSubclassOf<USettingKeybindingInputKeySelectorWidget> SettingKeybindingInputKeySelectorWidgetClass = nullptr;

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UTextBlock* InputNameTitleWidget = nullptr;

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UPanelWidget* InputKeySelectorPanelWidget = nullptr;

    // Localization

    UPROPERTY(EditAnywhere, Category = "Localization")
        TSoftObjectPtr<UStringTable> ProceduralLocalizationStringTable = nullptr;
    
public:

    // Delegates

    UPROPERTY()
        FOnRequestKeybindingChange OnRequestKeybindingChange;

private:

    // Widgets

    TSubclassOf<USettingKeybindingInputKeySelectorWidget> GetSettingKeybindingInputKeySelectorWidgetClass() const;

    // Delegates

    UFUNCTION()
        void ActOnSettingValueChanged(UCustomSetting* InSetting);

    UFUNCTION()
        void ActOnInputKeySelected(USettingKeybindingInputKeySelectorWidget* InInputKeySelectorWidget);

protected:

    // Widgets
    
    UTextBlock* GetInputNameTitleWidget() const;

    // Localization

    const FS_KeyMappings& GetKeyMappings() const;

    FName GetInputMappingName() const;

public:

    // Setup

    USettingKeybindingInputKeySelectorsWidget(const FObjectInitializer& ObjectInitializer);

    UCustomKeybindingSetting* GetCustomSetting() const;

    USettingKeybindingWidget* GetSettingKeybindingWidget() const;

    void BindToSetting(UCustomKeybindingSetting* InSetting, const FName& InInputMappingName, USettingKeybindingWidget* InSettingKeybindingWidget);

    // Widgets

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Widgets")
        UPanelWidget* GetInputKeySelectorPanelWidget() const;

    // Appearance

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Localization")
        TSoftObjectPtr<UStringTable> GetProceduralLocalizationStringTable() const;

    void SyncAppearanceToValue();

};