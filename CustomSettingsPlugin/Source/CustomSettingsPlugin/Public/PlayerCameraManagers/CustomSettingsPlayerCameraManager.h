/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "Camera/PlayerCameraManager.h"

#include "CustomSettingsPlayerCameraManager.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerCameraManagerInitializedFor);


UCLASS()
class ACustomSettingsPlayerCameraManager : public APlayerCameraManager {
	GENERATED_BODY()

private:

	UPROPERTY(Transient)
		bool bIsInitializedFor = false;

protected:

public:

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FOnPlayerCameraManagerInitializedFor OnPlayerCameraManagerInitializedFor;

private:

protected:

public:

	// Setup

	ACustomSettingsPlayerCameraManager();

	virtual void InitializeFor(class APlayerController* InPC) override;

	bool GetIsInitializedFor() const;

};