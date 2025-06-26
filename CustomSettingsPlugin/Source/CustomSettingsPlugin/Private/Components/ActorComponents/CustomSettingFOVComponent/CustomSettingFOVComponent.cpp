/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "CustomSettingFOVComponent.h"
#include "CustomSettingsPlayerCameraManager.h"
#include "PlayerControllerCore.h"
#include "PlayerSettingsSubsystem.h"
#include "CustomSettingsUtils.h"
#include "CustomSetting.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "LogCustomSettingsPlugin.h"

// Setup

void UCustomSettingFOVComponent::BeginPlay() {
	Super::BeginPlay();

	ACustomSettingsPlayerCameraManager* OwnerCameraManager = Cast<ACustomSettingsPlayerCameraManager>(GetOwner());
	if (!IsValid(OwnerCameraManager)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid ACustomSettingsPlayerCameraManager. The owner of this component must be of this class."));
		return;
	}

	/**
	* The APlayerCameraManager can be in a state where it does not yet reference a player controller.
	* In that situation there is no player to retrieve from it.
	* ACustomSettingsPlayerCameraManager implements a delegate for when the camera manager is initialized with a player controller, so that we can continue looking for a player.
	*/
	OwnerCameraManager->OnPlayerCameraManagerInitializedFor.AddDynamic(this, &UCustomSettingFOVComponent::ActOnPlayerCameraManagerInitializedFor);
	if (OwnerCameraManager->GetIsInitializedFor()) {
		ActOnPlayerCameraManagerInitializedFor();
	}
}

// Delegates

void UCustomSettingFOVComponent::ActOnPlayerCameraManagerInitializedFor() {
	ACustomSettingsPlayerCameraManager* OwnerCameraManager = Cast<ACustomSettingsPlayerCameraManager>(GetOwner());
	if (!IsValid(OwnerCameraManager)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid ACustomSettingsPlayerCameraManager. The owner of this component must be of this class."));
		return;
	}
	APlayerControllerCore* PCCore = Cast<APlayerControllerCore>(OwnerCameraManager->PCOwner);
	if (!IsValid(PCCore)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid APlayerControllerCore."));
		return;
	}
	// At this point the camera manager is associated with a player controller, but the latter might not be associated with a player yet. Wait for that.
	PCCore->OnReceivedPlayer.AddDynamic(this, &UCustomSettingFOVComponent::ActOnPlayerControllerOnReceivedPlayer);
	if (IsValid(PCCore->GetLocalPlayer())) {
		ActOnPlayerControllerOnReceivedPlayer();
	}
}

void UCustomSettingFOVComponent::ActOnPlayerControllerOnReceivedPlayer() {
	APlayerCameraManager* OwnerCameraManager = Cast<APlayerCameraManager>(GetOwner());
	if (!IsValid(OwnerCameraManager)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("This component's owner must be a valid PlayerCameraManager."));
		return;
	}
	APlayerController* PC = OwnerCameraManager->PCOwner;
	if (!IsValid(PC)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid APlayerController."));
		return;
	}
	ULocalPlayer* PL = PC->GetLocalPlayer();
	if (!IsValid(PL)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid ULocalPlayer."));
		return;
	}

	// Finally we can get the FOV setting associated with a player associated with this camera manager.
	UPlayerSettingsSubsystem* PSS = IsValid(PL) ? PL->GetSubsystem<UPlayerSettingsSubsystem>() : nullptr;
	if (!IsValid(PSS)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid UPlayerSettingsSubsystem."));
		return;
	}
	// Using this component without a registered FOV setting would be pointless, as it has no purpose without it.
	UCustomSetting* Setting = PSS->FindSettingObject(UCustomSettingsUtils::SettingFOV);

	// Any time the FOV setting is updated, update this camera manager directly from here on.
	Setting->OnValueAppliedSuccess.AddDynamic(this, &UCustomSettingFOVComponent::ActOnFOVSettingValueChanged);
	if (Setting->HasAppliedValue()) {
		// Update FOV immediately.
		ActOnFOVSettingValueChanged(Setting);	
	}
}

void UCustomSettingFOVComponent::ActOnFOVSettingValueChanged(UCustomSetting* InSetting) {
	if (!IsValid(InSetting)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid InSetting."));
		return;
	}
	
	APlayerCameraManager* OwnerCameraManager = Cast<APlayerCameraManager>(GetOwner());
	if (!IsValid(OwnerCameraManager)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("This component's owner must be a valid PlayerCameraManager."));
		return;
	}

	OwnerCameraManager->SetFOV(UCustomSettingsUtils::StringToFloat(InSetting->GetAppliedValue()));
}