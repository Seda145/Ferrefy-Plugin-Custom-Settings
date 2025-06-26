/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "MainMenuWidget.h"
#include "LogCustomSettingsPlugin.h"
#include "DecisionDialogWidget.h"
#include "UObject/SoftObjectPtr.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SlateUtils.h"


// Setup

UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer) {

	SetAutoRegisterRoute(USlateUtils::UIRouteMainMenu);
}

void UMainMenuWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	// DecisionDialogs delegates
	if (IsValid(DecisionDialog_BackToMainMenu)) {
		DecisionDialog_BackToMainMenu->OnDecided.AddDynamic(this, &UMainMenuWidget::ActOnDecisionDialogBackToMainMenuDecided);
	}
	if (IsValid(DecisionDialog_ExitGame)) {
		DecisionDialog_ExitGame->OnDecided.AddDynamic(this, &UMainMenuWidget::ActOnDecisionDialogExitGameDecided);
	}
}

// Delegates

void UMainMenuWidget::ActOnDecisionDialogExitGameDecided_Implementation(E_DecisionDialogDecisions InDecision) {
	if (InDecision == E_DecisionDialogDecisions::Yes) {
		// Handling quitting a game might differ per platform. However, then this dialog should not have been shown.
		UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
	}

	// Else the dialog should request navigation to "NAME_None" so we end up on the main menu again.
}

void UMainMenuWidget::ActOnDecisionDialogBackToMainMenuDecided_Implementation(E_DecisionDialogDecisions InDecision) {
	if (InDecision == E_DecisionDialogDecisions::Yes) {
		// We should be moving from an "ingame main menu" to the main menu level then.
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, LevelMainMenu);
	}

	// Else the dialog should request navigation to "NAME_None" so we end up on the in-game main menu again.
}

