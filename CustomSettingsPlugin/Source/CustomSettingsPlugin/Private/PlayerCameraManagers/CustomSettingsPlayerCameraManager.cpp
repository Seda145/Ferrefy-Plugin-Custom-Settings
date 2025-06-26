/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "CustomSettingsPlayerCameraManager.h"
#include "CustomSettingFOVComponent.h"


ACustomSettingsPlayerCameraManager::ACustomSettingsPlayerCameraManager() {
	CreateDefaultSubobject<UCustomSettingFOVComponent>(TEXT("CustomSettingFOVComponent"));
}

void ACustomSettingsPlayerCameraManager::InitializeFor(class APlayerController* InPC) {
	Super::InitializeFor(InPC);

	// This method lives so that we can be notified when Super sets the PlayerController.
	bIsInitializedFor = true;
	OnPlayerCameraManagerInitializedFor.Broadcast();
}

bool ACustomSettingsPlayerCameraManager::GetIsInitializedFor() const {
	return bIsInitializedFor;
}

