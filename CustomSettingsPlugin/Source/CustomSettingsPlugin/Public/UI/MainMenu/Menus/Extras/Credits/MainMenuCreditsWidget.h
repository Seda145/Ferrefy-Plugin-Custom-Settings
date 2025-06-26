/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "Engine/DataTable.h"

#include "MainMenuCreditsWidget.generated.h"


USTRUCT(BlueprintType)
struct CUSTOMSETTINGSPLUGIN_API FS_MainMenuCreditsTextBlock : public FTableRowBase {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FText Title;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FText> People;

    // Initialize
    FS_MainMenuCreditsTextBlock() 
        : Title (FText())
        , People (TArray<FText>())
    {}
};


USTRUCT(BlueprintType)
struct CUSTOMSETTINGSPLUGIN_API FS_MainMenuCreditsData : public FTableRowBase {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FS_MainMenuCreditsTextBlock> CreditsBlocks;

    // Initialize
    FS_MainMenuCreditsData() 
        : CreditsBlocks (TArray<FS_MainMenuCreditsTextBlock>())
    {}
};


UCLASS(Abstract, BlueprintType, Blueprintable) 
class CUSTOMSETTINGSPLUGIN_API UMainMenuCreditsWidget : public UMenuWidget {
    GENERATED_BODY()

private:

protected:

    // Setup

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|DT", meta = (RowType = "/Script/CustomSettingsPlugin.S_MainMenuCreditsData"))
        FDataTableRowHandle CreditsDT = FDataTableRowHandle();

public:

private:

protected:

public:

    // Navigation

    virtual void NativeShow() override;

	/* Implement in Blueprints: the process to reset and restart the credits. Automatically called from c++ on Show(). The credits data is in CreditsDT. */
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Setup")
        void ReplayCredits();

};