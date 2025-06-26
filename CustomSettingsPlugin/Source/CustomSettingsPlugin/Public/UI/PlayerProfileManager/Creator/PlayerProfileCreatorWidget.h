/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.h"
#include "UObject/SoftObjectPtr.h"

#include "PlayerProfileCreatorWidget.generated.h"

class UPlayerProfileCreatorEditableTextWidget;
class UTextBlock;
class UMenuNavigationButtonWidget;
class UButtonWidget;
class UStringTable;


UCLASS(Abstract, BlueprintType, Blueprintable, meta = (DontUseGenericSpawnObject = "True"))
class CUSTOMSETTINGSPLUGIN_API UPlayerProfileCreatorWidget : public UMenuWidget {
    GENERATED_BODY()

private:

protected:

    // Setup | Dependencies	

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UPlayerProfileCreatorEditableTextWidget* CreatorEditableTextBoxWidget = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UTextBlock* FeedbackTextWidget = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UButtonWidget* ButtonAccept = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UMenuNavigationButtonWidget* ButtonBack = nullptr;

    // Localization

    UPROPERTY(EditAnywhere, Category = "Localization")
    TSoftObjectPtr<UStringTable> StringTable = nullptr;

public:

private:

    // Appearance

    void SetProfileCreatorFeedback(const FText& InFeedback);

    // Delegates | Dependencies

    UFUNCTION()
        void ActOnProfileCreatorTextInputCommited(const FText& Text, ETextCommit::Type CommitMethod);

    // Delegates | Navigation

    UFUNCTION()
        void ActOnButtonAcceptActivated(UButtonWidget* InButtonWidget);

protected:

    // Setup

    virtual void NativeOnInitialized() override;

    // Localization

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Localization")
    TSoftObjectPtr<UStringTable> GetStringTable() const;

    // Appearance

    /* Blueprint event to implement visual feedback. */
    UFUNCTION(BlueprintImplementableEvent, Category = "Appearance")
        void OnProfileCreatorFeedbackSet();

    // Navigation
    
    void AcceptEditableTextBoxInput();

public:

    // Setup

    UPlayerProfileCreatorWidget(const FObjectInitializer& ObjectInitializer);

};