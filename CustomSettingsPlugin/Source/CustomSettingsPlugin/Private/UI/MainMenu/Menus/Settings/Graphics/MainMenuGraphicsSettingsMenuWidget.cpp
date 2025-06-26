/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "MainMenuGraphicsSettingsMenuWidget.h"
#include "PlayerSettingsSubsystem.h"
#include "SubsystemUtils.h"
#include "SlateUtils.h"
#include "CustomSettingsUtils.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "LogCustomSettingsPlugin.h"


// Setup

void UMainMenuGraphicsSettingsMenuWidget::NativeOnInitialized() {
	// Bind default input actions before Super call, so BP designers can alter behavior OnInitialized if required.
	BindRoutedInputAction(USlateUtils::InputActionNavExtra, false, true, this, &UMainMenuGraphicsSettingsMenuWidget::ActOnNavExtra);
	
    Super::NativeOnInitialized();
}

// Delegates | Input

void UMainMenuGraphicsSettingsMenuWidget::ActOnNavExtra() {
	ULocalPlayer* PL = GetOwningLocalPlayer();
	UPlayerSettingsSubsystem* PSS = IsValid(PL) ? PL->GetSubsystem<UPlayerSettingsSubsystem>() : nullptr;
	if (!IsValid(PSS)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("UPlayerSettingsSubsystem or player used to retrieve it invalid."));
		return;
	}
	
	// Settings currently use the same benchmark, and reply to the same "benchmark ran" delegate (PlayerSettingsSubsystem).
	// Usually anything ending up benchmarked is graphics related
	// , but it could still be modified into a category specific benchmark or even separate benchmark methods. 
	// For now running benchmark on the graphics menu just benchmarks anything listening anywhere.
	PSS->RunBenchmark();
}