/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "CustomSettingsPlayerController.h"
#include "CustomSettingsPlayerCameraManager.h"
#include "CustomSettingRumbleComponent.h"


// Setup

ACustomSettingsPlayerController::ACustomSettingsPlayerController() {
	PlayerCameraManagerClass = ACustomSettingsPlayerCameraManager::StaticClass();
	CustomSettingRumbleComponent = CreateDefaultSubobject<UCustomSettingRumbleComponent>(TEXT("CustomSettingRumbleComponent"));
}
