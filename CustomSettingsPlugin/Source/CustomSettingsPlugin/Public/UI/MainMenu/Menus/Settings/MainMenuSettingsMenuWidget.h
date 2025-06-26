/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "DecisionDialogWidget.h"

#include "MainMenuSettingsMenuWidget.generated.h"

class UDecisionDialogWidget;
class UMainMenuSettingDescriptionPanelWidget;


UCLASS(Abstract, BlueprintType, Blueprintable) 
class CUSTOMSETTINGSPLUGIN_API UMainMenuSettingsMenuWidget : public UMenuWidget {
    GENERATED_BODY()

private:

protected:

    // Setup | Dependencies

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UMainMenuSettingDescriptionPanelWidget* SettingDescriptionPanel = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UDecisionDialogWidget* DecisionDialog_ResetSettings = nullptr;

public:

private:

protected:

    // Setup

    virtual void NativeOnInitialized() override;

    // Settings

    /* Find setting widgets directly placed into this menu's widget tree. If it encounters a SettingSetWidget, dive into that and look for setting widgets. Does not find recursively. */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Widgets")
        TArray<USettingWidget*> FindSettingWidgets() const;

    /* Reset settings. Do this in "setup mode" as I do not intend to directly apply all changes on a menu per affected setting. */
    virtual void ResetSettings();

    virtual void RequestFullSettingsReset();

    // Delegates | DecisionDialog
    
    UFUNCTION()
        void ActOnDecisionDialog_ResetSettingsDecided(E_DecisionDialogDecisions InDecision);

    // Delegates | Settings

    UFUNCTION()
        virtual void ActOnDesiresShowSettingDescription(USettingWidget* InSettingWidget);

    UFUNCTION()
        virtual void ActOnStopDesiresShowSettingDescription(USettingWidget* InSettingWidget);

    // Delegates | Input

	UFUNCTION()
		virtual void ActOnNavReset();

public:

};