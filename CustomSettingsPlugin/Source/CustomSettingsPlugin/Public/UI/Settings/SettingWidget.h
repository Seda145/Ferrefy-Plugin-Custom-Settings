/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "KeyboundUserWidget.h"
#include "CustomSetting.h"
#include "UObject/SoftObjectPtr.h"

#include "SettingWidget.generated.h"

class USettingWidget;
class UTextBlock;
class UStringTable;
class UCustomSetting;
class UPlayerSettingsSubsystem;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDesireShowSettingDescriptionEvent, USettingWidget*, InWidget);


/**
* Abstract base class to be used for setting widgets.
* These widgets must be bound to a CustomSetting class, which controls setting related values.
* Widget interaction sends instructions to the CustomSetting, while it visually mirrors results reported back to the widget.
* Localization can be done procedurally (CustomSettingsUtils), but you will find that setting localization is likely to need various implementations.
*/
UCLASS(Abstract, BlueprintType, Blueprintable) 
class CUSTOMSETTINGSPLUGIN_API USettingWidget : public UKeyboundUserWidget {
    GENERATED_BODY()

private:

     // Settings

    /* Cached for convenience. */
    UPROPERTY(Transient)
        UPlayerSettingsSubsystem* PlayerSettingsSubsystem = nullptr;

    /* The setting this widget binds to. */
    UPROPERTY(Transient)
        UCustomSetting* CustomSetting = nullptr;

protected:

    // Localization

    UPROPERTY(EditAnywhere, Category = "Localization")
        TSoftObjectPtr<UStringTable> ProceduralLocalizationStringTable = nullptr;

    /* The key used for automated localization. During BindToSetting, this is set to the setting's name. */
    UPROPERTY(Transient)
        FString ProceduralLocalizationKey = FString();

    // Widgets

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
        UTextBlock* TextTitleWidget = nullptr;    
        
public:

    // Delegates

    UPROPERTY(BlueprintAssignable, Category = "Delegates")
        FOnDesireShowSettingDescriptionEvent OnDesiresShowSettingDescription;
    
    UPROPERTY(BlueprintAssignable, Category = "Delegates")
        FOnDesireShowSettingDescriptionEvent OnStopDesiresShowSettingDescription;

private:

    // Delegates

    UFUNCTION()
        void ActOnPlayerAssignedToProfile(UPlayerProfile* InProfile, ULocalPlayer* InPlayer);

protected:

    // Setup

    virtual void NativeOnInitialized() override;

    // Settings

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Settings")
        UPlayerSettingsSubsystem* GetPlayerSettingsSubsystem() const;

    /**
    * Override if desired. Use it if you wish to further build your widget based on data retrieved from the setting during BindToSetting. 
    * Don't call this manually. Called by BindToSetting. 
    */
    virtual void BindToSetting_Internal();

    // Navigation

    virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;

    virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;

    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    // Appearance

    /**
    * Used to visually synchronize to current setting values. 
    * A value changed, SetEnabled changed, a setting relevant to self compatibility changed, a setting became locked / unlocked etc. 
    * Visually respond to the combined situation. 
    */
    virtual void SyncAppearanceToValue();

    // Delegates

    UFUNCTION()
        void ActOnSettingValueChanged(UCustomSetting* InCustomSetting);

    UFUNCTION()
        void ActOnOtherSettingRelevantToSelfCompatibilityChanged(UCustomSetting* InOtherCustomSetting);

    UFUNCTION()
        void ActOnSettingLockedOrUnlocked(UCustomSetting* InCustomSetting);

    UFUNCTION()
        virtual void ActOnInputDeviceChanged(EInputDevices InInputDevice, int32 InSlateUserIndex);

public:

    // Setup

    USettingWidget(const FObjectInitializer& ObjectInitializer);

    // Appearance

    virtual void SetIsEnabled(bool bInIsEnabled) override;

    // Settings

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Settings")
        UCustomSetting* GetCustomSetting() const;

    UFUNCTION(BlueprintCallable, Category = "Settings")
        void BindToSetting(const FString& InSettingName);

    // Localization

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Localization")
        TSoftObjectPtr<UStringTable> GetProceduralLocalizationStringTable() const;

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Localization")
        FString GetProceduralLocalizationKey() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure = false, Category = "Appearance")
        FText GetLocalizedSettingName() const;

    virtual FText GetLocalizedSettingName_Implementation() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure = false, Category = "Appearance")
        FText GetLocalizedSettingValue(const FString& InValue) const;

    virtual FText GetLocalizedSettingValue_Implementation(const FString& InValue) const;

    // Widgets

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Widgets")
        UTextBlock* GetTextTitleWidget() const;

    // Appearance

    /* Utility to set visibility to the correct value, to ensure Slate's navigation finds this widget or an inner widget properly. */
    virtual void Show();

    /* Utility to set visibility to the correct value. */
    virtual void Hide();

};