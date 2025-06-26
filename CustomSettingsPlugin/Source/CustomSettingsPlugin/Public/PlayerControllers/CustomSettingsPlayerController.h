/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "PlayerControllerCore.h"

#include "CustomSettingsPlayerController.generated.h"


class UCustomSettingRumbleComponent;


/* A Player Controller is an actor responsible for controlling a Pawn used by the player. */
UCLASS(config = Game, BlueprintType, Blueprintable)
class ACustomSettingsPlayerController : public APlayerControllerCore {
	GENERATED_BODY()

private:

protected:

	UPROPERTY()
		UCustomSettingRumbleComponent* CustomSettingRumbleComponent = nullptr;

public:

private:

protected:

public:

	// Setup

	ACustomSettingsPlayerController();

};