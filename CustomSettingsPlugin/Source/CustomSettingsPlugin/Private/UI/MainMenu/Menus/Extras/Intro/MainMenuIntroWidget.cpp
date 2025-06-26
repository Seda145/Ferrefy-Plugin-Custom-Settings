/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "MainMenuIntroWidget.h"
#include "LogCustomSettingsPlugin.h"
#include "Animation/WidgetAnimation.h"


// Setup

void UMainMenuIntroWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	OnIntroStageEnded.BindDynamic(this, &UMainMenuIntroWidget::ActOnIntroStageEnded);
}

// Navigation

void UMainMenuIntroWidget::NativeShow() {
    Super::NativeShow();

	SetIntroStage(0);
}

void UMainMenuIntroWidget::NativeHide() {
	Super::NativeHide();

	StopIntroStages();
}

const TArray<FS_MainMenuIntroStage>& UMainMenuIntroWidget::GetIntroStages() const {
	return IntroStages;
}

int32 UMainMenuIntroWidget::RegisterIntroStage(const FS_MainMenuIntroStage& InIntroStage) {
	return IntroStages.Add(InIntroStage);
}

int32 UMainMenuIntroWidget::GetCurrentIntroStage() const {
	return CurrentIntroStage;
}

void UMainMenuIntroWidget::SetIntroStage(int32 InIntroStage) {
	StopIntroStages();

	// Now we can set the intro stage and check if we reached the end or need to start the new stage.
	CurrentIntroStage = InIntroStage;
	if (!GetIntroStages().IsValidIndex(GetCurrentIntroStage())) {
		UE_LOG(LogCustomSettingsPlugin, Verbose, TEXT("InIntroStage invalid. Most likely we just reached the end. OnReachedIntroEnd will be called."));
		OnReachedIntroEnd();
		return;
	}

	// Test if the new stage is valid.
	const FS_MainMenuIntroStage& IntroStage = GetIntroStages()[GetCurrentIntroStage()];
	if (!IsValid(IntroStage.WidgetAnimation)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("WidgetAnimation is not valid. OnReachedIntroEnd will be called."));
		OnReachedIntroEnd();
		return;
	}

	// Update time and states of previous and next stages, to simulate playing / resetting them to a start or end position.
	for (int32 i = 0; i < GetCurrentIntroStage(); i++) {
		const FS_MainMenuIntroStage& StageX = GetIntroStages()[i];
		if (!IsValid(StageX.WidgetAnimation)) {
			continue;
		}

		SetAnimationCurrentTime(StageX.WidgetAnimation, StageX.WidgetAnimation->GetEndTime());
	}
	for (int32 i = GetIntroStages().Num() - 1; i >= GetCurrentIntroStage(); i--) {
		const FS_MainMenuIntroStage& StageX = GetIntroStages()[i];
		if (!IsValid(StageX.WidgetAnimation)) {
			continue;
		}

		// Update time and states.
		SetAnimationCurrentTime(StageX.WidgetAnimation, StageX.StartAtTime);
	}

	// Bind the delegate to the new stage animation and play the current stage animation.
	BindToAnimationFinished(IntroStage.WidgetAnimation, OnIntroStageEnded);
	PlayAnimation(IntroStage.WidgetAnimation, IntroStage.StartAtTime, IntroStage.NumberOfLoops, IntroStage.PlayMode, IntroStage.PlaybackSpeed, IntroStage.bRestoreState);
	OnIntroStageStarted();
}

void UMainMenuIntroWidget::SkipIntroStage() {
	if (GetIntroStages().IsValidIndex(GetCurrentIntroStage())) {
		const FS_MainMenuIntroStage& IntroStage = GetIntroStages()[GetCurrentIntroStage()];
		if (!IntroStage.bCanSkipIntroStage) {
			return;
		}
	}

	SetIntroStage(GetCurrentIntroStage() + 1);
}

void UMainMenuIntroWidget::StopIntroStages() {
	// First clean up by unbinding a delegate from a previous animation and then stopping all animations.
	if (GetIntroStages().IsValidIndex(GetCurrentIntroStage())) {
		const FS_MainMenuIntroStage& OldIntroStage = GetIntroStages()[GetCurrentIntroStage()];
		if (IsValid(OldIntroStage.WidgetAnimation)) {
			UnbindFromAnimationFinished(OldIntroStage.WidgetAnimation, OnIntroStageEnded);
		}
	}

	for (const FS_MainMenuIntroStage& StageX : GetIntroStages()) {
		StopAnimation(StageX.WidgetAnimation);
	}
}

void UMainMenuIntroWidget::OnReachedIntroEnd_Implementation() {
	RequestUINavigation(NAME_None);
}

// Delegates | WidgetAnimations

void UMainMenuIntroWidget::ActOnIntroStageEnded() {
	SetIntroStage(GetCurrentIntroStage() + 1);
}

// Delegates | Input

void UMainMenuIntroWidget::ActOnNavBack() {
	// Instead of requesting UI navigation like SUPER, we use this button to skip a stage.
	SkipIntroStage();
}