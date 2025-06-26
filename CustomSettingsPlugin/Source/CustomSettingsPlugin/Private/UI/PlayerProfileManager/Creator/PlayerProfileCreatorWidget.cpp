/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "PlayerProfileCreatorWidget.h"
#include "LogCustomSettingsPlugin.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "MenuNavigationButtonWidget.h"
#include "ButtonWidget.h"
#include "PlayerIdentificationSubsystem.h"
#include "PlayerProfile.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/Commands/InputChord.h"
#include "UObject/SoftObjectPtr.h"
#include "CustomSettingsUtils.h"
#include "LocalizationUtils.h"
#include "SubsystemUtils.h"
#include "Internationalization/StringTable.h"
#include "PlayerProfileCreatorEditableTextWidget.h"
#include "UObject/ConstructorHelpers.h"


// Setup

UPlayerProfileCreatorWidget::UPlayerProfileCreatorWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	ConstructorHelpers::FObjectFinderOptional<UStringTable> Obj(TEXT("/CustomSettingsPlugin/Localization/Text/ST_MainMenu.ST_MainMenu"));
	if (Obj.Succeeded()) {
		StringTable = Obj.Get();
	}
	else {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Could not set a default string table for StringTable."));
	}
}

void UPlayerProfileCreatorWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	// Set the button bindings.
	if (IsValid(ButtonAccept)) {
		ButtonAccept->OnButtonReleased.AddDynamic(this, &UPlayerProfileCreatorWidget::ActOnButtonAcceptActivated);
	}	

	if (IsValid(CreatorEditableTextBoxWidget) && IsValid(CreatorEditableTextBoxWidget->GetEditableTextBoxWidget())) {
		CreatorEditableTextBoxWidget->GetEditableTextBoxWidget()->OnTextCommitted.AddDynamic(this, &UPlayerProfileCreatorWidget::ActOnProfileCreatorTextInputCommited);
	}
}

// Localization

TSoftObjectPtr<UStringTable> UPlayerProfileCreatorWidget::GetStringTable() const {
	return StringTable;
}

// Appearance

void UPlayerProfileCreatorWidget::SetProfileCreatorFeedback(const FText& InFeedback) {
	if (IsValid(FeedbackTextWidget)) {
		// Update the feedback text on the widget.
		FeedbackTextWidget->SetText(InFeedback);
		// Blueprint event for the designer. Might want to hide a empty feedback widgets or draw attention to feedback visually etc.
		OnProfileCreatorFeedbackSet();
	}
}

// Navigation

void UPlayerProfileCreatorWidget::AcceptEditableTextBoxInput() {
	if (!IsValid(CreatorEditableTextBoxWidget) || !IsValid(CreatorEditableTextBoxWidget->GetEditableTextBoxWidget())) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("EditableTextBoxWidget invalid."));
		return;
	}

	// UE5.1.1 GetText returns nothing. Synchronization issue in the editable textbox.
	// https://forums.unrealengine.com/t/bugs-bugs-bugs-slate-widget-synchronization/738172
	// Todo. UE5.2.1, GetText is broken now.
	// https://forums.unrealengine.com/t/ue5-2-1-bug-report-editabletextbox-broke-further-gettext-not-returning-set-value/1213966
	// Seems to work again in UE5.5.4
	const FString UserInput = CreatorEditableTextBoxWidget->GetEditableTextBoxWidget()->GetText().ToString();
	//const FString UserInput = CreatorEditableTextBoxWidget->GetEditableTextBoxWidget()->Text.ToString();
	FString OutProfileName = "";
	if (!UPlayerProfile::SanitizeStringAsProfileName(UserInput, OutProfileName)) {
		// I'm using ULocalizationUtils instead of NSLOCTEXT, because in 5.5 I was getting the C++ string instead of the string I entered in my string table.
		// I don't want to risk that happening in-game, so I'm using my utils instead of EPIC's macro.
		SetProfileCreatorFeedback(ULocalizationUtils::Localize(GetStringTable()->GetStringTableId(), "Feedback_ProfileNameInvalid"));
		return;
	}

	if (!UserInput.Equals(OutProfileName)) {
		// TODO, engine bug in 5.1.1 https://forums.unrealengine.com/t/ue5-1-1-ueditabletextbox-settext-reverts-to-previous-text-cant-set-text/1180739
		// Seems to work in UE5.5.4
		CreatorEditableTextBoxWidget->GetEditableTextBoxWidget()->SetText(FText::FromString(OutProfileName));
		// Notify the player the name is changed and return. Wait for the player to commit again.
		SetProfileCreatorFeedback(ULocalizationUtils::Localize(GetStringTable()->GetStringTableId(), "Feedback_ProfileNameSpecialCharactersRemoved"));
		return;
	}

	// At this point the user input was sanitized properly and decided final. 
	// Next we check if a profile already exists with this name.

	UPlayerIdentificationSubsystem* PIS = USubsystemUtils::GetGameInstanceSubsystem<UPlayerIdentificationSubsystem>(this);
	check(IsValid(PIS));

	if (IsValid(PIS->GetProfileByName(OutProfileName))) {
		SetProfileCreatorFeedback(ULocalizationUtils::Localize(GetStringTable()->GetStringTableId(), "Feedback_ProfileNameAlreadyExists"));
		return;
	}

	// If it doesn't exist, add it.
	UPlayerProfile* NewProfile = PIS->AddProfile(OutProfileName);
	if (!IsValid(NewProfile)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("NewProfile is invalid"));
		return;
	}

	// If the owning player is currently assigned to a profile, unassign it so we can swap to the newly created one.
	if (IsValid(GetOwningLocalPlayer())) {
		UPlayerProfile* AssignedProfile = PIS->GetProfileAssignedToPlayer(GetOwningLocalPlayer());
		if (IsValid(AssignedProfile)) {
			if (!PIS->UnAssignProfile(AssignedProfile)) {
				UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Could not unassign the player to the new profile."));
				return;
			}
		}

		if (!PIS->AssignProfileToPlayer(NewProfile, GetOwningLocalPlayer())) {
			UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Could not assign the player to the new profile."));
			return;
		}
	}

	// Success.
	SetProfileCreatorFeedback(FText());
	RequestUINavigation(NAME_None);
}

// Delegates | Dependencies

void UPlayerProfileCreatorWidget::ActOnProfileCreatorTextInputCommited(const FText& Text, ETextCommit::Type CommitMethod) {
	if (CommitMethod == ETextCommit::Type::OnEnter) {
		AcceptEditableTextBoxInput();
	}
}

// Delegates | Navigation

void UPlayerProfileCreatorWidget::ActOnButtonAcceptActivated(UButtonWidget* InButtonWidget) {
	AcceptEditableTextBoxInput();
}