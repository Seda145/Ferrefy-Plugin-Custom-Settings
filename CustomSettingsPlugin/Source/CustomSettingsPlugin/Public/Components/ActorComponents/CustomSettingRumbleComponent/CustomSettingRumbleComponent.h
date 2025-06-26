/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "CustomSettingRumbleComponent.generated.h"


class UForceFeedbackEffect;


/**
 * This component responds to changes in a player's gamepad rumble setting, 
 * by communicating the value to a playercontroller and by playing a feedback preview.
 * The owner of this component must be a APlayerController.
 **/
UCLASS(ClassGroup = CommonLogic, editinlinenew, meta = (BlueprintSpawnableComponent))
class CUSTOMSETTINGSPLUGIN_API UCustomSettingRumbleComponent : public UActorComponent {
   GENERATED_BODY()

private:

	UPROPERTY()
		UForceFeedbackEffect* PreviewForceFeedbackEffect = nullptr;

protected:

public:

private:

	// Delegates

	UFUNCTION()
		void ActOnRumbleSettingValueChanged(UCustomSetting* InSetting);

protected:

public:

	// Setup

	UCustomSettingRumbleComponent();

	virtual void BeginPlay() override;

};