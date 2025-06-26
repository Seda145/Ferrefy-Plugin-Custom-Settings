/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomSetting.h"

#include "SettingKeybindingImageWidget.generated.h"


class UImage;


/**
* 
*/
UCLASS(Abstract, BlueprintType, Blueprintable) 
class CUSTOMSETTINGSPLUGIN_API USettingKeybindingImageWidget : public UUserWidget {
    GENERATED_BODY()

private:

protected:

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UImage* KeyImageWidget = nullptr;

public:

private:

protected:

public:

	UImage* GetKeyImageWidget() const;

};