/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "PlayerProfileCreatorEditableTextWidget.generated.h"

class UEditableTextBox;


/** This class is just a wrapper for an editable text box
* , used with the player profile creator to input a new profile name. 
* Reason for its existence is that a BP widget can implement / override some input behavior related to this editable text box alone.
*/
UCLASS(Abstract, BlueprintType, Blueprintable, meta = (DontUseGenericSpawnObject = "True"))
class CUSTOMSETTINGSPLUGIN_API UPlayerProfileCreatorEditableTextWidget : public UUserWidget {
    GENERATED_BODY()

private:

protected:

    // Setup | Dependencies	

	/* Dependency required from a deriving class. */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
        UEditableTextBox* EditableTextBoxWidget = nullptr;

public:

private:

protected:

public:

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Widgets")
		UEditableTextBox* GetEditableTextBoxWidget() const;

};