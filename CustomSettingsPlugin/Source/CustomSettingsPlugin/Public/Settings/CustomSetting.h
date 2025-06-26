/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "CustomSettingSolverInfo.h"

#include "CustomSetting.generated.h"


class UCustomSetting;
class UPlayerSettingsSubsystem;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCustomSettingEvent, UCustomSetting*, InCustomSetting);


/** 
* A CustomSetting can load / apply / save and test settings. 
* Before use, a CustomSetting must be initialized by the PlayerSettingsSubsystem.
*/
UCLASS(BlueprintType, Blueprintable)
class CUSTOMSETTINGSPLUGIN_API UCustomSetting : public UObject {
    GENERATED_BODY()

private:

    UPROPERTY(Transient)
        FString AppliedValue = "";

    UPROPERTY(Transient)
        UPlayerSettingsSubsystem* PlayerSettingsSubsystem = nullptr;

protected:

public:

    // Delegates

    /* Broadcasts the setting which started the Set chain. */
    UPROPERTY(BlueprintAssignable, Category = "Delegates")
        FOnCustomSettingEvent OnSetChainAppliedSuccess;

    /* Broadcasts this setting on which the value changed. */
    UPROPERTY(BlueprintAssignable, Category = "Delegates")
        FOnCustomSettingEvent OnValueAppliedSuccess;

    /* Broadcasts the other setting which changed. */
    UPROPERTY(BlueprintAssignable, Category = "Delegates")
        FOnCustomSettingEvent OnOtherSettingRelevantToSelfCompatibilityChanged;

    /* Broadcasts this setting. */
    UPROPERTY(BlueprintAssignable, Category = "Delegates")
        FOnCustomSettingEvent OnSettingLocked;

    /* Broadcasts this setting. */
    UPROPERTY(BlueprintAssignable, Category = "Delegates")
        FOnCustomSettingEvent OnSettingUnlocked;

private:

protected:

    /**
    * Override if a setting applies a direct effect to a system (for example, setting display resolution). This is not required.
	* Settings can also be used to look up a value just when required in an external system. 
	* Implement DoesAppliedValueMatchSystemState to test system state post application if possible.
    * Do not call this manually. It is already called by ApplyValue, which is called from a setting solver.
    */
    UFUNCTION(BlueprintNativeEvent, Category = "Settings")
        void ApplyValueToSystem();

    virtual void ApplyValueToSystem_Implementation();

    /* Only called from CleanValue to perform minimal cleanup before a new value is applied. Don't call manually. Override if required. */
    UFUNCTION(BlueprintNativeEvent, Category = "Settings")
        void CleanValue_Internal();

    virtual void CleanValue_Internal_Implementation();

public:

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Settings")
        UPlayerSettingsSubsystem* GetPlayerSettingsSubsystem() const;

    /* Called once by the Player Settings Subsystem. Don't call manually. */
    virtual void RegisterPlayerSettingsSubsystem(UPlayerSettingsSubsystem* InPlayerSettingsSubsystem);

    /**
    * Must be overridden. Provides the value registrations to the setting solver.
    */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure = false, Category = "Settings")
        FS_SettingSolverSettingRegistration GetSolverRegistrationInfo() const;

    virtual FS_SettingSolverSettingRegistration GetSolverRegistrationInfo_Implementation() const;

    /** 
    * The value which must be compatible with all other registered settings their save mode, and preferrably with any of their values. 
    * Commonly this is an "off" state in which a feature simply does nothing, requires nothing, and does not modify any other setting.
    */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure = false, Category = "Settings")
        FString GetSaveModeValue() const;

    virtual FString GetSaveModeValue_Implementation() const;

    /** 
    * Returns the value applied through this setting, or empty. 
    * This stored value can mismatch from what is currently applied to the system (DoesAppliedValueMatchSystemState). 
    */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Settings")
        FString GetAppliedValue() const;

    /**
    * Returns if AppliedValue is not empty.
    */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Settings")
        bool HasAppliedValue() const;

    /** 
    * Must be overridden if applying a system effect. Returns if the applied value matches the state the system is currently in. 
    * For example, if a display resolution set by a setting is equal to what is currently in use by the system. 
    * When some external engine / plugin settings are not testable, could mark them as TODOs / log warnings.
    * It's not something you can assert on here, another reason because some settings could require an application restart to be applied etc.
    */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure = false, Category = "Settings")
        bool DoesAppliedValueMatchSystemState() const;

    virtual bool DoesAppliedValueMatchSystemState_Implementation() const;

    /** 
    * Use to test if a value is within bounds / choices of this setting, and to test system hardware / state support for it.
    */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure = false, Category = "Settings")
        bool IsValidValue(const FString& InValue) const;

    virtual bool IsValidValue_Implementation(const FString& InValue) const;

    /* Only called from the PlayerSettingsSubsystem to perform minimal cleanup before an initial (re)application is performed. Don't call manually. */
    void CleanValue();

    /** 
    * Applies a value, and optionally a system effect. Logs if !DoesAppliedValueMatchSystemState() post application.
    * Direct calls to this method are rerouted to UPlayerSettingsSubsystem::TryApplySetting.
    */
    UFUNCTION(BlueprintCallable, Category = "Settings")
        void ApplyValue(const FString& InValue);

    // Setting 

    /* Tries to find an applicable value for the setting. Asserts if it can't be found. */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Settings")
        bool FindApplicableValueInDirection(bool bInGetNextValue, bool bInWrapAround, FString& OutValue) const;

    /* Tries to apply the setting. Asserts if it can't be found. */
    UFUNCTION(BlueprintCallable, Category = "Settings")
        bool TryApplySettingInDirection(bool bInGetNextValue, bool bInWrapAround);

};