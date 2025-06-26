/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomKeybindingSetting.h"
#include "InputCoreTypes.h"
#include "Engine/DataTable.h"
#include "InputKeyImageData.h"
#include "Engine/StreamableManager.h"

#include "SettingKeybindingInputKeySelectorWidget.generated.h"

class UPanelWidget;
class UTextBlock;
class UImage;
class UInputKeySelector;
class USettingKeybindingImageWidget;
class USettingKeybindingInputKeySelectorWidget;
class UHoverFeedbackWidget;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputKeySelected, USettingKeybindingInputKeySelectorWidget*, InInputKeySelectorWidget);


/**
* Widget which provides keybinding functionality for a single key, relevant to an input mapping.
* The inner UInputKeySelector is used for to focus on, as it provides all input handling as default.
*/
UCLASS(Abstract, BlueprintType, Blueprintable, meta = (DontUseGenericSpawnObject = "True"))
class CUSTOMSETTINGSPLUGIN_API USettingKeybindingInputKeySelectorWidget : public UUserWidget {
    GENERATED_BODY()

private:

protected:

    // Widgets

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UPanelWidget* KeybindingImagesPanelWidget = nullptr;

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
		UInputKeySelector* InputKeySelectorWidget = nullptr;

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        UHoverFeedbackWidget* HoverFeedbackWidget = nullptr;

    UPROPERTY(EditAnywhere, Category = "Widgets")
        TSubclassOf<USettingKeybindingImageWidget> SettingKeybindingImageWidgetClass = nullptr;

    // Appearance
    
    UPROPERTY(EditAnywhere, Category = "Appearance", meta = (RowType = "/Script/UIAdditionsPlugin.S_InputKeyImageData"))
		FDataTableRowHandle KeybindingImagesDT = FDataTableRowHandle();

    bool bIsSyncingToValue = false;

    FInputChord SyncedInputChord = FInputChord(EKeys::Invalid);

public:

    UPROPERTY()
        FOnInputKeySelected OnInputKeySelected;

private:

    // Delegates
    
    UFUNCTION()
        void ActOnIsSelectingKeyChanged();    
        
    UFUNCTION()
        void ActOnKeySelected(FInputChord InInputChord);

protected:

    // Setup

    virtual void NativeOnInitialized() override;

    // Appearance

    UFUNCTION(BlueprintNativeEvent, Category = "Appearance")
        void AppearStartSelectingKey();

    virtual void AppearStartSelectingKey_Implementation();

    UFUNCTION(BlueprintNativeEvent, Category = "Appearance")
        void AppearStopSelectingKey();

    virtual void AppearStopSelectingKey_Implementation();

public:

    // Setup

    USettingKeybindingInputKeySelectorWidget(const FObjectInitializer& InObjectInitializer);

    // Widgets

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Widgets")
        UInputKeySelector* GetInputKeySelectorWidget() const;

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Widgets")
        UHoverFeedbackWidget* GetHoverFeedbackWidget() const;

    // Appearance

    //virtual void SetIsEnabled(bool bInIsEnabled) override;

    void SyncAppearanceToValue(const FInputChord& InValue);

    bool GetIsSyncingToValue() const;

    FInputChord GetSyncedInputChord() const;

};