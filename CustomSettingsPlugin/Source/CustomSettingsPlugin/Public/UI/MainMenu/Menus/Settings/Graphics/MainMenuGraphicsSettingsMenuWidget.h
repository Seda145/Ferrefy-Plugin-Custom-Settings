/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "MainMenuSettingsMenuWidget.h"

#include "MainMenuGraphicsSettingsMenuWidget.generated.h"


UCLASS(Abstract, BlueprintType, Blueprintable) 
class CUSTOMSETTINGSPLUGIN_API UMainMenuGraphicsSettingsMenuWidget : public UMainMenuSettingsMenuWidget {
    GENERATED_BODY()

private:

protected:

public:

private:

protected:

    // Setup

    virtual void NativeOnInitialized() override;

    // Delegates | Input

	UFUNCTION()
		virtual void ActOnNavExtra();

public:

};