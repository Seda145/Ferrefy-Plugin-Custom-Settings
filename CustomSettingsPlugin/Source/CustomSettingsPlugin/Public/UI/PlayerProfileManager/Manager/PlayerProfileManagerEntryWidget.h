/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "PlayerProfileManagerEntryWidget.generated.h"

class UTextBlock;
class UButtonWidget;
class UPlayerProfile;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestAssignPlayerProfile, UPlayerProfile*, InPlayerProfile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestUnAssignPlayerProfile, UPlayerProfile*, InPlayerProfile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestRemovePlayerProfile, UPlayerProfile*, InPlayerProfile);


UCLASS(Abstract, BlueprintType, Blueprintable, meta = (DontUseGenericSpawnObject = "True"))
class CUSTOMSETTINGSPLUGIN_API UPlayerProfileManagerEntryWidget : public UUserWidget {
    GENERATED_BODY()

private:

    // Setup

    UPROPERTY(Transient)
        UPlayerProfile* PlayerProfile = nullptr;

protected:

    // Setup | Dependencies	

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UTextBlock* ProfileTextWidget = nullptr;


    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
    UWidget* ProfileActiveWidget = nullptr;

    /* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
    UWidget* ButtonContainerWidget = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UButtonWidget* ButtonAssign = nullptr;

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UButtonWidget* ButtonRemove = nullptr;

public:

    // Delegates | PlayerIdentificationSubsystem

    UPROPERTY(BlueprintAssignable, Category = "Delegates|PlayerIdentificationSubsystem")
        FOnRequestAssignPlayerProfile OnRequestAssignPlayerProfile;

    UPROPERTY(BlueprintAssignable, Category = "Delegates|PlayerIdentificationSubsystem")
        FOnRequestUnAssignPlayerProfile OnRequestUnAssignPlayerProfile;

    UPROPERTY(BlueprintAssignable, Category = "Delegates|PlayerIdentificationSubsystem")
        FOnRequestRemovePlayerProfile OnRequestRemovePlayerProfile;

private:

    // Delegates | Navigation

    UFUNCTION()
        void ActOnButtonAssignActivated(UButtonWidget* InButtonWidget);

    UFUNCTION()
        void ActOnButtonRemoveActivated(UButtonWidget* InButtonWidget);

protected:

    // Setup

    virtual void NativeOnInitialized() override;

    virtual void NativePreConstruct() override;

    // Navigation

    virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

    // Delegates | PlayerIdentificationSubsystem

    /* Method implementing a visual response to when a player profile is assigned. By default this hides / disables buttons and restores focus if required. */
    UFUNCTION(BlueprintNativeEvent, Category = "Delegates|PlayerIdentificationSubsystem")
        void ActOnPlayerProfileAssignedToPlayer(UPlayerProfile* InPlayerProfile, ULocalPlayer* InPlayer);

    virtual void ActOnPlayerProfileAssignedToPlayer_Implementation(UPlayerProfile* InPlayerProfile, ULocalPlayer* InPlayer);

    /* Method implementing a visual response to when a player profile is unassigned. By default this shows / enables buttons. */
    UFUNCTION(BlueprintNativeEvent, Category = "Delegates|PlayerIdentificationSubsystem")
        void ActOnPlayerProfileUnAssignedToPlayer(UPlayerProfile* InPlayerProfile, ULocalPlayer* InPlayer);

    virtual void ActOnPlayerProfileUnAssignedToPlayer_Implementation(UPlayerProfile* InPlayerProfile, ULocalPlayer* InPlayer);

public:

    // Setup

    UPlayerProfileManagerEntryWidget(const FObjectInitializer& InObjectInitializer);

    // Navigation
    
    void RestoreFocus();
    
    // Profile

    /* The profile to display this widget for. */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Profile")
        UPlayerProfile* GetPlayerProfile() const;

    /* The profile to display this widget for. Asserts if InPlayerProfile is invalid. */
    UFUNCTION(BlueprintCallable, Category = "Profile")
        void SetPlayerProfile(UPlayerProfile* InPlayerProfile);

};