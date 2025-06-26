/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "CustomSetting.h"
#include "CustomSettingsUtils.h"
#include "GameFramework/PlayerInput.h"
#include "InputCoreTypes.h"

#include "CustomKeybindingSetting.generated.h"


UCLASS(BlueprintType, Blueprintable)
class CUSTOMSETTINGSPLUGIN_API UCustomKeybindingSetting : public UCustomSetting {
    GENERATED_BODY()

private:

protected:

public:

private:

protected:

    virtual void ApplyValueToSystem_Implementation() override;

public:

    virtual FS_SettingSolverSettingRegistration GetSolverRegistrationInfo_Implementation() const override;

    virtual FString GetSaveModeValue_Implementation() const override;

    virtual bool DoesAppliedValueMatchSystemState_Implementation() const override;

    virtual bool IsValidValue_Implementation(const FString& InValue) const override;

    // Keybinding

    /* Returns a blacklist of input mapping names which are not allowed to rebind (RequestInputMappingKeyRebind). */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure = false, Category = "Settings")
        TSet<FName> GetBlacklistInputNames() const;

    virtual TSet<FName> GetBlacklistInputNames_Implementation() const;

    /* Returns a blacklist of keys which are not allowed to rebind (RequestInputMappingKeyRebind). */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure = false, Category = "Settings")
        TSet<FKey> GetBlacklistInputKeys() const;

    virtual TSet<FKey> GetBlacklistInputKeys_Implementation() const;

    UFUNCTION(BlueprintCallable, Category = "Settings")
        void RequestInputMappingKeyRebind(const FName& InInputName, const FInputChord& InOldValue, const FInputChord& InNewValue);

    // Delegates

    UFUNCTION()
        void ActOnInputKeySelectorsWidgetSelects(const FName& InInputName, const FInputChord& InOldValue, const FInputChord& InNewValue);

};