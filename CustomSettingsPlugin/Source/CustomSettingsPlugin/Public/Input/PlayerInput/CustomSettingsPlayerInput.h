/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "CorePlayerInput.h"
#include "SlateUtils.h"

#include "CustomSettingsPlayerInput.generated.h"


class APawn;
class APlayerController;

/**
* Class inheriting from UCorePlayerInput, which pulls information from settings (keybindings, mentioned key modifications).
*/
UCLASS()
class CUSTOMSETTINGSPLUGIN_API UCustomSettingsPlayerInput : public UCorePlayerInput {
	GENERATED_BODY()

private:

protected:

public:

private:

	void PullCustomKeybindings();

	// Delegates

	UFUNCTION()
		void ActOnKeybindSettingValueAppliedSuccess(UCustomSetting* InSetting);

    UFUNCTION()
        void ActOnGamepadInvertXAxisRotateSettingValueAppliedSuccess(UCustomSetting* InSetting);

    UFUNCTION()
        void ActOnGamepadInvertYAxisRotateSettingValueAppliedSuccess(UCustomSetting* InSetting);

    UFUNCTION()
        void ActOnGamepadXAxisRotateSensitivitySettingValueAppliedSuccess(UCustomSetting* InSetting);

    UFUNCTION()
        void ActOnGamepadYAxisRotateSensitivitySettingValueAppliedSuccess(UCustomSetting* InSetting);

    UFUNCTION()
        void ActOnKeyboardMouseInvertXAxisRotateSettingValueAppliedSuccess(UCustomSetting* InSetting);

    UFUNCTION()
        void ActOnKeyboardMouseInvertYAxisRotateSettingValueAppliedSuccess(UCustomSetting* InSetting);

    UFUNCTION()
        void ActOnKeyboardMouseXAxisRotateSensitivitySettingValueAppliedSuccess(UCustomSetting* InSetting);

    UFUNCTION()
        void ActOnKeyboardMouseYAxisRotateSensitivitySettingValueAppliedSuccess(UCustomSetting* InSetting);

protected:

    // Delegates

    virtual void ActOnWorldBeginPlay() override;

public:

};

