/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "CustomSetting.h"

#include "AntiAliasingModeSetting.generated.h"


UCLASS()
class CUSTOMSETTINGSPLUGIN_API UAntiAliasingModeSetting : public UCustomSetting {
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

};