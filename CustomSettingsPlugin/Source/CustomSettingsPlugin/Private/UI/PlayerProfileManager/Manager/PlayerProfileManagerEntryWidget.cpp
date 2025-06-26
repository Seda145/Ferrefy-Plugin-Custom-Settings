/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "PlayerProfileManagerEntryWidget.h"
#include "LogCustomSettingsPlugin.h"
#include "ButtonWidget.h"
#include "CentralButton.h"
#include "PlayerIdentificationSubsystem.h"
#include "PlayerProfile.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "SlateUtils.h"
#include "SubsystemUtils.h"
 

// Setup

UPlayerProfileManagerEntryWidget::UPlayerProfileManagerEntryWidget(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer) {
	
	// This widget must be focusable and hit testable so we can pass on focus to an inner widget when focused.
	SetIsFocusable(true);
	SetVisibility(ESlateVisibility::Visible);
}

void UPlayerProfileManagerEntryWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	// Create the button bindings.
	if (IsValid(ButtonAssign)) {
		ButtonAssign->OnButtonReleased.AddDynamic(this, &UPlayerProfileManagerEntryWidget::ActOnButtonAssignActivated);
	}	
	if (IsValid(ButtonRemove)) {
		ButtonRemove->OnButtonReleased.AddDynamic(this, &UPlayerProfileManagerEntryWidget::ActOnButtonRemoveActivated);
	}	

	UPlayerIdentificationSubsystem* PIS = USubsystemUtils::GetGameInstanceSubsystem<UPlayerIdentificationSubsystem>(this);
	check(IsValid(PIS));

	// PlayerIdentificationSubsystem delegates
	PIS->OnPlayerAssignedToProfile.AddDynamic(this, &UPlayerProfileManagerEntryWidget::ActOnPlayerProfileAssignedToPlayer);
	PIS->OnPlayerUnAssignedFromProfile.AddDynamic(this, &UPlayerProfileManagerEntryWidget::ActOnPlayerProfileUnAssignedToPlayer);
}

void UPlayerProfileManagerEntryWidget::NativePreConstruct() {
	Super::NativePreConstruct();

#if WITH_EDITOR
	if (IsDesignTime()) {
		if (IsValid(ProfileTextWidget)) {
			ProfileTextWidget->SetText(INVTEXT("Placeholder"));
		}
		if (IsValid(ButtonAssign)) {
			ButtonAssign->SetIsEnabled(true);
		}
		if (IsValid(ButtonRemove)) {
			ButtonRemove->SetIsEnabled(true);
		}
	}
#endif // WITH_EDITOR
}

// Navigation

FReply UPlayerProfileManagerEntryWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) {
	Super::NativeOnFocusReceived(InGeometry, InFocusEvent);

	/**
	* When we receive focus we want to pass it on to the inner buttons. 
	* This is a utility required because with slate navigation keys (up left etc.) any buttons positioned diagonal are skipped, resulting in a failed focus.
	* Because the entry widgets are just added to a horizontal or vertical box we can focus this UserWidget and just pass on focus to a valid inner button.
	*/

	RestoreFocus();

	return FReply::Handled();
}

void UPlayerProfileManagerEntryWidget::RestoreFocus() {
	if (IsFocusable() && !HasUserFocus(GetOwningPlayer())) {
		// Assume this triggers NativeOnFocusReceived, which retriggers RestoreFocus for additional restoration attempt.
		UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Restoring focus to player profile manager entry widget."));
		SetFocus();
		return;
	}

	// If we already have focus, or if direct focus is not supported, then try the inner buttons.

	auto SetFocusIfPossible = [](UButtonWidget* InButtonWidget) -> bool {
		if (IsValid(InButtonWidget)) {
			UCentralButton* ButtonX = InButtonWidget->GetButtonWidget();
			if (IsValid(ButtonX) && ButtonX->GetIsEnabled() && ButtonX->GetIsFocusable()) {
				UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("Restoring focus to player profile manager entry widget > %s."), *ButtonX->GetName());
				ButtonX->SetFocus();
				return true;
			}
		}
		return false;
	};

	// The inner buttons are inside ButtonContainerWidget which is visible only while a profile is not active.
	if (USlateUtils::IsVisible(ButtonContainerWidget->GetVisibility())) {
		if (SetFocusIfPossible(ButtonAssign) || SetFocusIfPossible(ButtonRemove)) {
			// Assume success as SetFocus was used on one of the buttons.
			// Check the logs if it misbehaves.
			return;
		}
	}
}

// Profile

UPlayerProfile* UPlayerProfileManagerEntryWidget::GetPlayerProfile() const {
	return PlayerProfile;
}

void UPlayerProfileManagerEntryWidget::SetPlayerProfile(UPlayerProfile* InPlayerProfile) {
#if WITH_EDITOR
	if (IsDesignTime()) {
		// A guard, we can not access the subsystem and player etc in the editor.
		return;
	}
#endif // WITH_EDITOR

	PlayerProfile = InPlayerProfile;
	check(IsValid(GetPlayerProfile()));

	if (IsValid(ProfileTextWidget)) {
		ProfileTextWidget->SetText(FText::FromString(GetPlayerProfile()->GetProfileName()));
	}

	const UPlayerIdentificationSubsystem* PIS = USubsystemUtils::GetGameInstanceSubsystem<UPlayerIdentificationSubsystem>(this);
	if (IsValid(PIS)) {
		ULocalPlayer* PlayerX = PIS->GetPlayerAssignedToProfile(GetPlayerProfile());
		if (IsValid(PlayerX)) {
			ActOnPlayerProfileAssignedToPlayer(GetPlayerProfile(), PlayerX);
		}
		else {
			ActOnPlayerProfileUnAssignedToPlayer(GetPlayerProfile(), PlayerX);
		}
	}
}

// Delegates | PlayerIdentificationSubsystem

void UPlayerProfileManagerEntryWidget::ActOnPlayerProfileAssignedToPlayer_Implementation(UPlayerProfile* InPlayerProfile, ULocalPlayer* InPlayer) {
	if (InPlayerProfile != GetPlayerProfile()) {
		return;
	}

	bool bMustRestoreFocus = false;
	auto DoIf = [this, &bMustRestoreFocus](UButtonWidget* InButtonWidget) {
		if (IsValid(InButtonWidget)) {
			if (InButtonWidget->GetButtonWidget()->HasUserFocus(InButtonWidget->GetButtonWidget()->GetOwningPlayer())) {
				bMustRestoreFocus = true;
			}
			InButtonWidget->SetIsEnabled(false);
		}
	};
	DoIf(ButtonAssign);
	DoIf(ButtonRemove);
	if (IsValid(ProfileActiveWidget)) {
		ProfileActiveWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	if (IsValid(ButtonContainerWidget)) {
		ButtonContainerWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (bMustRestoreFocus) {
		RestoreFocus();
	}
}

void UPlayerProfileManagerEntryWidget::ActOnPlayerProfileUnAssignedToPlayer_Implementation(UPlayerProfile* InPlayerProfile, ULocalPlayer* InPlayer) {
	if (!IsValid(InPlayerProfile)) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("InPlayerProfile invalid."));
		return;
	}
	if (InPlayerProfile != GetPlayerProfile()) {
		// Skip if the profile is not relevant to this widget.
		return;
	}

	auto DoIf = [this](UButtonWidget* InButtonWidget) {
		if (IsValid(InButtonWidget)) {
			InButtonWidget->SetIsEnabled(true);
		}
	};
	DoIf(ButtonAssign);
	DoIf(ButtonRemove);

	if (IsValid(ProfileActiveWidget)) {
		ProfileActiveWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (IsValid(ButtonContainerWidget)) {
		ButtonContainerWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

// Delegates | Navigation

void UPlayerProfileManagerEntryWidget::ActOnButtonAssignActivated(UButtonWidget* InButtonWidget) {
	OnRequestAssignPlayerProfile.Broadcast(GetPlayerProfile());
}

void UPlayerProfileManagerEntryWidget::ActOnButtonRemoveActivated(UButtonWidget* InButtonWidget) {
	OnRequestRemovePlayerProfile.Broadcast(GetPlayerProfile());
}
