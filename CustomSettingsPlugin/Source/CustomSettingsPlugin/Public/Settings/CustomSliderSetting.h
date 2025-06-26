/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "CustomSetting.h"

#include "CustomSliderSetting.generated.h"



UCLASS(BlueprintType, Blueprintable)
class CUSTOMSETTINGSPLUGIN_API UCustomSliderSetting : public UCustomSetting {
    GENERATED_BODY()

private:

protected:

public:

private:

protected:

public:

    // Setting 

    /* Override this. Get the step size used to IncreaseValue or DecreaseValue. */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure = false, Category = "Settings")
        float GetStepSize() const;

    virtual float GetStepSize_Implementation() const;

    /* The minimum value that can be applied (inclusive) */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure = false, Category = "Settings")
        float GetMinValue() const;

    virtual float GetMinValue_Implementation() const;


    /* The maximum value that can be applied (inclusive) */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure = false, Category = "Settings")
        float GetMaxValue() const;

    virtual float GetMaxValue_Implementation() const;

    /* Return true if Super does and InValue is between GetMinValue and GetMaxValue inclusive */
    virtual bool IsValidValue_Implementation(const FString& InValue) const override;

    /* Tries to find an applicable value for the setting. Asserts if it can't be found. */
    bool FindApplicableValueInDirection(bool bInGetNextValue, FString& OutValue) const;

    /* Tries to find an applicable value for the setting. Asserts if it can't be found. Blueprint version of overload TryApplySettingInDirection! */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Settings")
        float FindApplicableSliderValueInDirection(bool bInGetNextValue, FString& OutValue) const;

    /* Tries to apply the setting. Asserts if it can't be found. */
    bool TryApplySettingInDirection(bool bInGetNextValue);

    /* Tries to apply the setting. Asserts if it can't be found. Blueprint version of overload TryApplySettingInDirection! */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Settings")
        bool TryApplySliderSettingInDirection(bool bInGetNextValue);


};