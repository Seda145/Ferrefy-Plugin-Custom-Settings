/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "CustomSettingFOVComponent.generated.h"


class ULocalPlayer;
class UCustomSetting;


/**
 * Component which responds to a change in a player's FOV setting, to update the FOV on an associated camera manager. 
 * The owner must be a ACustomSettingsPlayerCameraManager, for that camera manager allows us to retrieve a valid player association.
 **/
UCLASS(ClassGroup = CommonLogic, editinlinenew, meta = (BlueprintSpawnableComponent))
class CUSTOMSETTINGSPLUGIN_API UCustomSettingFOVComponent : public UActorComponent {
   GENERATED_BODY()

private:

protected:

public:

private:

    // Delegates

    UFUNCTION()
        void ActOnPlayerCameraManagerInitializedFor();

	UFUNCTION()
        void ActOnPlayerControllerOnReceivedPlayer();

	UFUNCTION()
		void ActOnFOVSettingValueChanged(UCustomSetting* InSetting);
    
protected:

public:

    // Setup

    virtual void BeginPlay() override;

};