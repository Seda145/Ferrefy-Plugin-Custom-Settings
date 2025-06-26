/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/SoftObjectPtr.h"
#include "UObject/SoftObjectPath.h"
#include "InputCoreTypes.h"
#include "Templates/SubclassOf.h"
#include "CustomSetting.h"

#include "CustomSettingsPluginSettings.generated.h"

class UWorld;
class USoundClass;
class UForceFeedbackEffect;
class UMainMenuWidget;
class UCustomSetting;


UCLASS(config=Game, defaultconfig, meta = (DisplayName = "Custom Settings Plugin"))
class CUSTOMSETTINGSPLUGIN_API UCustomSettingsPluginSettings : public UDeveloperSettings {
	GENERATED_BODY()

private:

protected:

public:

	// World

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "World")
		TSoftObjectPtr<UWorld> LevelMainMenu = TSoftObjectPtr<UWorld>(FSoftObjectPath("/CustomSettingsPlugin/Levels/Level_MainMenu.Level_MainMenu"));

	// Settings
	
	/** 
	* This is configurable from project settings just for users who make blueprint based settings. 
	* Users with c++ only settings should consider making this property transient instead. 
	* Overrides from a config / non transient props can override code registrations.
	*/
	//UPROPERTY(Transient, BlueprintReadOnly, Category = "Settings")
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Settings")
		TSet<TSubclassOf<UCustomSetting>> CustomSettingClasses = TSet<TSubclassOf<UCustomSetting>>();

private:

	void InstallCustomSettings();

protected:

public:

	UCustomSettingsPluginSettings();

};

