/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "CustomSetting.h"

#include "TextureQualitySetting.generated.h"


class UPlayerSettingsSubsystem;


UCLASS()
class CUSTOMSETTINGSPLUGIN_API UTextureQualitySetting : public UCustomSetting {
    GENERATED_BODY()

private:

protected:

public:

private:

protected:

    virtual void ApplyValueToSystem_Implementation() override;

    // Delegates

    UFUNCTION()
        void ActOnBenchmarkRan(float InCPUPerfIndex, float InGPUPerfIndex);

public:

    virtual void RegisterPlayerSettingsSubsystem(UPlayerSettingsSubsystem* InPlayerSettingsSubsystem) override;

    virtual FS_SettingSolverSettingRegistration GetSolverRegistrationInfo_Implementation() const override;

    virtual FString GetSaveModeValue_Implementation() const override;

    virtual bool DoesAppliedValueMatchSystemState_Implementation() const override;

};