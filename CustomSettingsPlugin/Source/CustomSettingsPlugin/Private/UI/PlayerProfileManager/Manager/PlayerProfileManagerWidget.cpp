/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "PlayerProfileManagerWidget.h"
#include "LogCustomSettingsPlugin.h"
#include "Framework/Commands/InputChord.h"
#include "PlayerProfileManagerEntryWidget.h"
#include "MenuNavigationButtonWidget.h"
#include "PlayerProfileCreatorWidget.h"
#include "PlayerProfile.h"
#include "PlayerIdentificationSubsystem.h"
#include "Components/PanelWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "UObject/SoftObjectPtr.h"
#include "CentralButton.h"
#include "SubsystemUtils.h"


// Setup

void UPlayerProfileManagerWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	UPlayerIdentificationSubsystem* PIS = USubsystemUtils::GetGameInstanceSubsystem<UPlayerIdentificationSubsystem>(this);
	check(IsValid(PIS));

	for (UPlayerProfile* ProfileX : PIS->GetProfiles()) {
		AddProfileWidget(ProfileX);
	}

	PIS->OnProfileAdded.AddDynamic(this, &UPlayerProfileManagerWidget::ActOnProfileAdded);
	PIS->OnProfileRemoved.AddDynamic(this, &UPlayerProfileManagerWidget::ActOnProfileRemoved);
}

// PlayerProfileManagerEntryWidget

void UPlayerProfileManagerWidget::AddProfileWidget(UPlayerProfile* InProfile) {
	if (!IsValid(InProfile)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("InProfile is invalid"));
		return;
	}
	if (!IsValid(PlayerProfileManagerEntryWidgetClass)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("PlayerProfileManagerEntryWidgetClass is invalid"));
		return;
	}
	UPlayerProfileManagerEntryWidget* ProfileWidget = CreateWidget<UPlayerProfileManagerEntryWidget>(this, PlayerProfileManagerEntryWidgetClass);
	if (!IsValid(ProfileWidget)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("ProfileWidget is invalid"));
		return;
	}

	ProfileWidget->SetPlayerProfile(InProfile);

	ProfileWidget->OnRequestAssignPlayerProfile.AddDynamic(this, &UPlayerProfileManagerWidget::ActOnProfileEntryRequestAssign);
	ProfileWidget->OnRequestUnAssignPlayerProfile.AddDynamic(this, &UPlayerProfileManagerWidget::ActOnProfileEntryRequestUnAssign);
	ProfileWidget->OnRequestRemovePlayerProfile.AddDynamic(this, &UPlayerProfileManagerWidget::ActOnProfileEntryRequestRemove);

	ProfileList->AddChild(ProfileWidget);
}

void UPlayerProfileManagerWidget::RemoveProfileWidget(UPlayerProfileManagerEntryWidget* InProfileWidget) {
	if (!IsValid(InProfileWidget)) {
		UE_LOG(LogCustomSettingsPlugin, Warning, TEXT("InProfileWidget is invalid"));
		return;
	}

	const bool bNeedRestoreFocus = InProfileWidget->HasFocusedDescendants() || InProfileWidget->HasUserFocus(InProfileWidget->GetOwningPlayer());
	if (bNeedRestoreFocus) {
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("A profile entry widget or descendant is focused, but will be removed. Focus restoration will be calculated."));
		// Rely on the default entry widget if the preferred widget is not set later on.
		SetPreferredFocusEntryWidget(nullptr);

		const int32 CurIndex = ProfileList->GetAllChildren().IndexOfByKey(InProfileWidget);
		if (CurIndex != INDEX_NONE) {
			UPlayerProfileManagerEntryWidget* RestoreFocusToWidget = nullptr;
			if (ProfileList->GetAllChildren().IsValidIndex(CurIndex - 1)) {
				RestoreFocusToWidget = Cast<UPlayerProfileManagerEntryWidget>(ProfileList->GetAllChildren()[CurIndex - 1]);
			}
			else if (ProfileList->GetAllChildren().IsValidIndex(CurIndex + 1)) {
				RestoreFocusToWidget = Cast<UPlayerProfileManagerEntryWidget>(ProfileList->GetAllChildren()[CurIndex + 1]);
			}

			if (IsValid(RestoreFocusToWidget) && RestoreFocusToWidget->IsFocusable()) {
				UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Found a new profile entry widget to focus (previous or next in list)."));
				SetPreferredFocusEntryWidget(RestoreFocusToWidget);
			}
		}
		else {
			UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Can't find the current profile widget on the ProfileList."));
		}
	}
	
	InProfileWidget->RemoveFromParent();

	if (bNeedRestoreFocus) {
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Calling RestoreFocus because a profile widget using focus has been removed."));
		RestoreFocus();
	}
}

UPlayerProfileManagerEntryWidget* UPlayerProfileManagerWidget::GetProfileWidgetByProfile(const UPlayerProfile* InProfile) const {
	UPlayerProfileManagerEntryWidget* ProfileWidget = nullptr;
	if (!IsValid(InProfile)) {
		UE_LOG(LogCustomSettingsPlugin, Warning, TEXT("InProfile is invalid"));
		return ProfileWidget;
	}

	for (UWidget* WidgetX : ProfileList->GetAllChildren()) {
		UPlayerProfileManagerEntryWidget* EntryWidgetX = Cast<UPlayerProfileManagerEntryWidget>(WidgetX);
		if (IsValid(EntryWidgetX)) {
			if (EntryWidgetX->GetPlayerProfile() == InProfile) {
				ProfileWidget = EntryWidgetX;
				break;
			}
		}
	}

	return ProfileWidget;
}

// Delegates | PlayerProfileManagerEntryWidget

void UPlayerProfileManagerWidget::ActOnProfileEntryRequestAssign(UPlayerProfile* InProfile) {
	UPlayerIdentificationSubsystem* PIS = USubsystemUtils::GetGameInstanceSubsystem<UPlayerIdentificationSubsystem>(this);
	check(IsValid(PIS));

	// If the owning player is currently assigned to a profile, unassign it so we can swap to the newly created one.
	check(IsValid(GetOwningPlayer()));
	UPlayerProfile* AssignedProfile = PIS->GetProfileAssignedToPlayer(GetOwningLocalPlayer());
	if (IsValid(AssignedProfile)) {
		PIS->UnAssignProfile(AssignedProfile);
	}

	PIS->AssignProfileToPlayer(InProfile, GetOwningLocalPlayer());
}

void UPlayerProfileManagerWidget::ActOnProfileEntryRequestUnAssign(UPlayerProfile* InProfile) {
	UPlayerIdentificationSubsystem* PIS = USubsystemUtils::GetGameInstanceSubsystem<UPlayerIdentificationSubsystem>(this);
	if (IsValid(PIS)) {
		PIS->UnAssignProfile(InProfile);
	}
}

void UPlayerProfileManagerWidget::ActOnProfileEntryRequestRemove(UPlayerProfile* InProfile) {
	UPlayerIdentificationSubsystem* PIS = USubsystemUtils::GetGameInstanceSubsystem<UPlayerIdentificationSubsystem>(this);
	if (IsValid(PIS)) {
		PIS->RemoveProfile(InProfile);
	}
}

// Delegates | PlayerIdentificationSubsystem

void UPlayerProfileManagerWidget::ActOnProfileAdded(UPlayerProfile* InProfile) {
	AddProfileWidget(InProfile);
}

void UPlayerProfileManagerWidget::ActOnProfileRemoved(UPlayerProfile* InProfile) {
	RemoveProfileWidget(GetProfileWidgetByProfile(InProfile));
}
