/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"

#include "MainMenuWidget.generated.h"

class UDecisionDialogWidget;


UCLASS(Abstract, BlueprintType, Blueprintable, meta = (DontUseGenericSpawnObject = "True"))
class CUSTOMSETTINGSPLUGIN_API UMainMenuWidget : public UMenuWidget {
    GENERATED_BODY()

private:

protected:

    // World

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
        TSoftObjectPtr<UWorld> LevelMainMenu = TSoftObjectPtr<UWorld>(FSoftObjectPath("/CustomSettingsPlugin/Levels/Level_MainMenu.Level_MainMenu"));

    // Widgets

	/* Optional dependency of the base class. This DecisionDialog is common on in-game main menus which can travel to a main menu level. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets|DecisionDialogs", meta = (BindWidgetOptional))
        UDecisionDialogWidget* DecisionDialog_BackToMainMenu = nullptr;

	/* Optional dependency of the base class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets|DecisionDialogs", meta = (BindWidgetOptional))
        UDecisionDialogWidget* DecisionDialog_ExitGame = nullptr;

public:

private:

protected:

    // Setup

    virtual void NativeOnInitialized() override;

    // Delegates | DecisionDialogs

    /* This method calls UKismetSystemLibrary::QuitGame by default. */
    UFUNCTION(BlueprintNativeEvent, Category = "Delegates|DecisionDialogs")
        void ActOnDecisionDialogExitGameDecided(E_DecisionDialogDecisions InDecision);

    virtual void ActOnDecisionDialogExitGameDecided_Implementation(E_DecisionDialogDecisions InDecision);

    /* This method calls UGameplayStatics::OpenLevelBySoftObjectPtr by default, to open the configured main menu level. */
    UFUNCTION(BlueprintNativeEvent, Category = "Delegates|DecisionDialogs")
        void ActOnDecisionDialogBackToMainMenuDecided(E_DecisionDialogDecisions InDecision);

    virtual void ActOnDecisionDialogBackToMainMenuDecided_Implementation(E_DecisionDialogDecisions InDecision);

public:

    UMainMenuWidget(const FObjectInitializer& InObjectInitializer);

};