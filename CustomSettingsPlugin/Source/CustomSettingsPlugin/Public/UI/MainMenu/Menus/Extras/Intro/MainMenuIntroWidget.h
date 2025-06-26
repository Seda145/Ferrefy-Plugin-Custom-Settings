/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "Blueprint/UserWidget.h"

#include "MainMenuIntroWidget.generated.h"


class UWidgetAnimation;


USTRUCT(BlueprintType)
struct FS_MainMenuIntroStage {
    GENERATED_BODY()

     UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UWidgetAnimation* WidgetAnimation;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float StartAtTime;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        int32 NumberOfLoops;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TEnumAsByte<EUMGSequencePlayMode::Type> PlayMode;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float PlaybackSpeed;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        bool bRestoreState;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        bool bCanSkipIntroStage;

    // Initialize
    FS_MainMenuIntroStage() 
        : WidgetAnimation (nullptr)
        , StartAtTime (0.f)
        , NumberOfLoops (1)
        , PlayMode (EUMGSequencePlayMode::Forward)
        , PlaybackSpeed (1.f)
        , bRestoreState (false)
        , bCanSkipIntroStage (true)
    {}
    FS_MainMenuIntroStage(UWidgetAnimation* InWidgetAnimation, float InStartAtTime, int32 InNumberOfLoops, TEnumAsByte<EUMGSequencePlayMode::Type> InPlayMode, float InPlaybackSpeed, bool bInRestoreState, bool bInCanSkipIntroStage) 
        : WidgetAnimation (InWidgetAnimation)
        , StartAtTime (InStartAtTime)
        , NumberOfLoops (InNumberOfLoops)
        , PlayMode (InPlayMode)
        , PlaybackSpeed (InPlaybackSpeed)
        , bRestoreState (bInRestoreState)
        , bCanSkipIntroStage (bInCanSkipIntroStage)
    {}
};


UCLASS(Abstract, BlueprintType, Blueprintable) 
class CUSTOMSETTINGSPLUGIN_API UMainMenuIntroWidget : public UMenuWidget {
    GENERATED_BODY()

private:

    // Data

    UPROPERTY()
        int32 CurrentIntroStage = 0;

    UPROPERTY()
        TArray<FS_MainMenuIntroStage> IntroStages;

    // Delegates | WidgetAnimation | AutoDecide

    UPROPERTY()
        FWidgetAnimationDynamicEvent OnIntroStageEnded;

protected:

public:

private:

protected:

    // Setup

    virtual void NativeOnInitialized() override;

    // Navigation
    
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Navigation")
        const TArray<FS_MainMenuIntroStage>& GetIntroStages() const;

    UFUNCTION(BlueprintCallable, Category = "Navigation")
        int32 RegisterIntroStage(const FS_MainMenuIntroStage& InIntroStage);

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Navigation")
        int32 GetCurrentIntroStage() const;

    /* Sets and starts this intro stage. */
    UFUNCTION(BlueprintCallable, Category = "Navigation")
        void SetIntroStage(int32 InIntroStage);

    UFUNCTION(BlueprintImplementableEvent, Category = "Navigation")
        void OnIntroStageStarted();

    UFUNCTION(BlueprintCallable, Category = "Navigation")
        void SkipIntroStage();

    /* Utility to unbind OnIntroStageEnded and stop all intro stage animations.  */
    UFUNCTION(BlueprintCallable, Category = "Navigation")
        void StopIntroStages();

    UFUNCTION(BlueprintNativeEvent, Category = "Navigation")
        void OnReachedIntroEnd();

    void OnReachedIntroEnd_Implementation();

     // Delegates | WidgetAnimations

    UFUNCTION()
        void ActOnIntroStageEnded();

    // Delegates | Input

    virtual void ActOnNavBack() override;

public:

    // Navigation

    virtual void NativeShow() override;

    virtual void NativeHide() override;

};