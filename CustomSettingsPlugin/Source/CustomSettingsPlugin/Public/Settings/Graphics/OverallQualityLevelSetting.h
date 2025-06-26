/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "CustomSetting.h"

#include "OverallQualityLevelSetting.generated.h"


UCLASS()
class CUSTOMSETTINGSPLUGIN_API UOverallQualityLevelSetting : public UCustomSetting {
    GENERATED_BODY()

private:

protected:

public:

private:

protected:

public:

    virtual FS_SettingSolverSettingRegistration GetSolverRegistrationInfo_Implementation() const;

    virtual FString GetSaveModeValue_Implementation() const override;

    virtual bool DoesAppliedValueMatchSystemState_Implementation() const override;

};