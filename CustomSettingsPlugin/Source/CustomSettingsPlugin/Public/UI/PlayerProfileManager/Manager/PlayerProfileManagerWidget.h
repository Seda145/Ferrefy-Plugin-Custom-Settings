/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"

#include "PlayerProfileManagerWidget.generated.h"

class UPanelWidget;
class UMenuNavigationButtonWidget;
class UPlayerProfileManagerEntryWidget;
class UPlayerProfileCreatorWidget;
class UPlayerProfile;


UCLASS(Abstract, BlueprintType, Blueprintable, meta = (DontUseGenericSpawnObject = "True"))
class CUSTOMSETTINGSPLUGIN_API UPlayerProfileManagerWidget : public UMenuWidget {
    GENERATED_BODY()

private:

protected:

	// Setup

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widgets")
        TSubclassOf<UPlayerProfileManagerEntryWidget> PlayerProfileManagerEntryWidgetClass = nullptr;

    // Setup | Dependencies

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UPanelWidget* ProfileList = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UPlayerProfileCreatorWidget* ProfileCreatorWidget = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UMenuNavigationButtonWidget* ButtonAddProfile = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UMenuNavigationButtonWidget* ButtonBack = nullptr;

public:

private:

    // PlayerProfileManagerEntryWidget

    /* Add a widget to ProfileList. */
    void AddProfileWidget(UPlayerProfile* InProfile);

    /* Remove a widget present on ProfileList. */
    void RemoveProfileWidget(UPlayerProfileManagerEntryWidget* InProfileWidget);

    UPlayerProfileManagerEntryWidget* GetProfileWidgetByProfile(const UPlayerProfile* InProfile) const;

    // Delegates | PlayerProfileManagerEntryWidget

    UFUNCTION()
        void ActOnProfileEntryRequestAssign(UPlayerProfile* InProfile);

    UFUNCTION()
        void ActOnProfileEntryRequestUnAssign(UPlayerProfile* InProfile);

    UFUNCTION()
        void ActOnProfileEntryRequestRemove(UPlayerProfile* InProfile);

    // Delegates | PlayerIdentificationSubsystem

    UFUNCTION()
        void ActOnProfileAdded(UPlayerProfile* InProfile);

    UFUNCTION()
        void ActOnProfileRemoved(UPlayerProfile* InProfile);

protected:

    // Setup

    virtual void NativeOnInitialized() override;

public:

};