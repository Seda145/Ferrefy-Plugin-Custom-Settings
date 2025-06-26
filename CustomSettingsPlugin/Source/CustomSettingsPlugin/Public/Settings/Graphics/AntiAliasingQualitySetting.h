/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "CustomSetting.h"

#include "AntiAliasingQualitySetting.generated.h"


class UPlayerSettingsSubsystem;


UCLASS()
class CUSTOMSETTINGSPLUGIN_API UAntiAliasingQualitySetting : public UCustomSetting {
    GENERATED_BODY()

private:

protected:

public:

private:

    virtual void ApplyValueToSystem_Implementation() override;

    // Delegates

    UFUNCTION()
        void ActOnBenchmarkRan(float InCPUPerfIndex, float InGPUPerfIndex);

protected:

public:

    virtual void RegisterPlayerSettingsSubsystem(UPlayerSettingsSubsystem* InPlayerSettingsSubsystem) override;

    /* Mapping value to value solver registration information */
    virtual FS_SettingSolverSettingRegistration GetSolverRegistrationInfo_Implementation() const override;

    virtual FString GetSaveModeValue_Implementation() const override;

    virtual bool DoesAppliedValueMatchSystemState_Implementation() const override;

};