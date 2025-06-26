/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "MainMenuSettingDescriptionPanelWidget.generated.h"

class UImage;
class UTextBlock;
class USettingWidget;


UCLASS(Abstract, BlueprintType, Blueprintable) 
class CUSTOMSETTINGSPLUGIN_API UMainMenuSettingDescriptionPanelWidget : public UUserWidget {
    GENERATED_BODY()

private:

protected:

    // Setup | Dependencies

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UTextBlock* TextTitleWidget = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UTextBlock* TextDescriptionWidget = nullptr;

public:

private:

protected:

	// Setup

	virtual void NativePreConstruct() override;

public:

    // Data

	UFUNCTION(BlueprintCallable)
		void ClearPanel();

	UFUNCTION(BlueprintCallable)
		void FillPanel(const USettingWidget* InSettingWidget);

};