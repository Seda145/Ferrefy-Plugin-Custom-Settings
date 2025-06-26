/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "CustomSettingRumbleComponent.h"
#include "LogCustomSettingsPlugin.h"
#include "PlayerSettingsSubsystem.h"
#include "CustomSettingsUtils.h"
#include "CustomSetting.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/ForceFeedbackEffect.h"


// Setup

UCustomSettingRumbleComponent::UCustomSettingRumbleComponent() {
	ConstructorHelpers::FObjectFinderOptional<UForceFeedbackEffect> Obj(TEXT("/CustomSettingsPlugin/ForceFeedback/FFE_GamepadPreview.FFE_GamepadPreview"));
	if (Obj.Succeeded()) {
		PreviewForceFeedbackEffect = Obj.Get();
	}
	else {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Could not find asset for setting's PreviewForceFeedbackEffect."));
	}
}

void UCustomSettingRumbleComponent::BeginPlay() {
	Super::BeginPlay();

	APlayerController* OwnerPC = Cast<APlayerController>(GetOwner());
	if (!IsValid(OwnerPC)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("The owner of this component must be a valid APlayerController."));
		return;
	}
	ULocalPlayer* PL = OwnerPC->GetLocalPlayer();
	if (!IsValid(PL)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid ULocalPlayer."));
		return;
	}

	// Now we can get to the setting for this player.
	UPlayerSettingsSubsystem* PSS = IsValid(PL) ? PL->GetSubsystem<UPlayerSettingsSubsystem>() : nullptr;
	if (!IsValid(PSS)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid UPlayerSettingsSubsystem."));
		return;
	}
	// Using this component without a registered Rumble setting would be pointless, as it has no purpose without it.
	UCustomSetting* Setting = PSS->FindSettingObject(UCustomSettingsUtils::SettingGamepadRumble);

	// Any time the Rumble setting is updated, respond to this change.
	Setting->OnValueAppliedSuccess.AddDynamic(this, &UCustomSettingRumbleComponent::ActOnRumbleSettingValueChanged);
	if (Setting->HasAppliedValue()) {
		// Update Rumble immediately.
		ActOnRumbleSettingValueChanged(Setting);
	}
}

// Delegates

void UCustomSettingRumbleComponent::ActOnRumbleSettingValueChanged(UCustomSetting* InSetting) {
	if (!IsValid(InSetting)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Invalid InSetting."));
		return;
	}
	APlayerController* OwnerPC = Cast<APlayerController>(GetOwner());
	if (!IsValid(OwnerPC)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("The owner of this component must be a valid APlayerController."));
		return;
	}

	if (InSetting->GetAppliedValue() == UCustomSettingsUtils::ValueOff) {
		OwnerPC->bForceFeedbackEnabled = false;
	}
	else if (InSetting->GetAppliedValue() == UCustomSettingsUtils::ValueOn) {
		if (IsValid(PreviewForceFeedbackEffect)) {
			FForceFeedbackParameters FeedbackParameters = FForceFeedbackParameters();
			FeedbackParameters.bPlayWhilePaused = true;
			OwnerPC->ClientPlayForceFeedback(PreviewForceFeedbackEffect, FeedbackParameters);
		}
		else {
			UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Can't preview the force feedback effect, because it is invalid."));
		}
		OwnerPC->bForceFeedbackEnabled = true;
	}
}