/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InputKeyMode.h"
#include "GameFramework/PlayerInput.h"
#include "GenericPlatform/GenericWindow.h"
#include "SettingMultiChoiceWidget.h"

#include "CustomSettingsUtils.generated.h"


class USettingSliderWidget;
class UWidget;
class USettingWidget;
class UCustomSliderSetting;


UENUM(BlueprintType)
enum class E_RHIMode : uint8 {
	RHI_DX11,
	RHI_DX12,
	RHI_VULKAN
};


UENUM(BlueprintType)
enum class E_BenchmarkHWIndexToUse : uint8 {
	Min,
	CPU,
	GPU
};


USTRUCT(BlueprintType)
struct CUSTOMSETTINGSPLUGIN_API FS_KeyMappings {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInputActionKeyMapping> ActionKeyMappings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInputAxisKeyMapping> AxisKeyMappings;

	FS_KeyMappings() 
		: ActionKeyMappings (TArray<FInputActionKeyMapping>())
		, AxisKeyMappings (TArray<FInputAxisKeyMapping>())
	{}
	FS_KeyMappings(const TArray<FInputActionKeyMapping>& InActionKeyMappings, const TArray<FInputAxisKeyMapping>& InAxisKeyMappings) 
		: ActionKeyMappings (InActionKeyMappings)
		, AxisKeyMappings (InAxisKeyMappings)
	{}
};


/**
* The main functionality of this utility class is to convert data between human readable strings and datatypes used for settings.
* This utility class also provides string constants for setting names, sections and values.
* Plenty of conversion methods for setting values are provided. Disabled blueprint callable on them, because they assert if random input is thrown at them.
*/
UCLASS()
class CUSTOMSETTINGSPLUGIN_API UCustomSettingsUtils : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

private:

protected:

public:

	/* Setting sections */
	static const FString SectionWindowsTargetSettings;
	static const FString SectionCustomSettings;
	static const FString SectionCustomSettingsProfile;

	/* Custom Setting Profiles */
	static const FString CustomSettingsProfilesSaveGameName;

	/* Setting keys */
	static const FString ActionMappings;
	static const FString AxisMappings;
	static const FString DefaultGraphicsRHI;

	/* Settings (deduplicated), as used in the config sections */
	static const FString SettingAmbienceVolume;
	static const FString SettingAmbientOcclusion;
	static const FString SettingAntiAliasingMode;
	static const FString SettingAntiAliasingQuality;
	static const FString SettingBloom;
	static const FString SettingDifficulty;
	static const FString SettingDisplayResolution;
	static const FString SettingFOV;
	static const FString SettingFoliageQuality;
	static const FString SettingGamepadInvertXAxisRotate;
	static const FString SettingGamepadInvertYAxisRotate;
	static const FString SettingGamepadRumble;
	static const FString SettingGamepadXAxisRotateSensitivity;
	static const FString SettingGamepadYAxisRotateSensitivity;
	static const FString SettingGamma;
	static const FString SettingGlobalIlluminationQuality;
	static const FString SettingKeyboardMouseInvertXAxisRotate;
	static const FString SettingKeyboardMouseInvertYAxisRotate;
	static const FString SettingKeyboardMouseXAxisRotateSensitivity;
	static const FString SettingKeyboardMouseYAxisRotateSensitivity;
	static const FString SettingLanguage;
	static const FString SettingLastUsedCustomSettingsProfileP0;
	static const FString SettingLastUsedCustomSettingsProfileP1;
	static const FString SettingLastUsedCustomSettingsProfileP2;
	static const FString SettingLastUsedCustomSettingsProfileP3;
	static const FString SettingLensFlares;
	static const FString SettingLockFPS;
	static const FString SettingKeybinding;
	static const FString SettingMasterVolume;
	static const FString SettingMotionBlur;
	static const FString SettingMusicVolume;
	static const FString SettingOverallQualityLevel;
	static const FString SettingPostProcessingQuality;
	static const FString SettingRaytracedAmbientOcclusion;
	static const FString SettingRaytracedGlobalIllumination;
	static const FString SettingRaytracedReflections;
	static const FString SettingRaytracedShadows;
	static const FString SettingRaytracedTranslucency;
	static const FString SettingReflectionQuality;
	static const FString SettingRenderHardwareInterface;
	static const FString SettingResolutionScale;
	static const FString SettingSFXVolume;
	static const FString SettingSSR;
	static const FString SettingShadingQuality;
	static const FString SettingShadowQuality;
	static const FString SettingShowFPSCounter;
	static const FString SettingSubtitleMode;
	static const FString SettingSubtitleScale;
	static const FString SettingTextureQuality;
	static const FString SettingUIScale;
	static const FString SettingUIVolume;
	static const FString SettingVSync;
	static const FString SettingViewDistanceQuality;
	static const FString SettingVisionColorDeficiencySeverity;
	static const FString SettingVisionColorDeficiencyType;
	static const FString SettingVisualEffectQuality;
	static const FString SettingVoiceVolume;
	static const FString SettingWeatherVolume;
	static const FString SettingWindowMode;

	/* String values (deduplicated) */
	static const FString Value30;
	static const FString Value60;
	static const FString Value90;
	static const FString Value120;
	static const FString Value240;
	static const FString ValueAuto;
	static const FString ValueCinematic;
	static const FString ValueCustom;
	static const FString ValueDeuteranope;
	static const FString ValueEasy;
	static const FString ValueEpic;
	static const FString ValueExpert;
	static const FString ValueExtraLarge;
	static const FString ValueFullscreen;
	static const FString ValueFXAA;
	static const FString ValueHard;
	static const FString ValueHigh;
	static const FString ValueHold;
	static const FString ValueLarge;
	static const FString ValueLow;
	static const FString ValueMedium;
	static const FString ValueNone;
	static const FString ValueNormal;
	static const FString ValueOff;
	static const FString ValueOn;
	static const FString ValueProtanope;
	static const FString ValueRHIDX11;
	static const FString ValueRHIDX12;
	static const FString ValueRHIVulkan;
	static const FString ValueTAA;
	static const FString ValueToggle;
	static const FString ValueTritanope;
	static const FString ValueUnlimited;
	static const FString ValueWindowed;
	static const FString ValueWindowedFullscreen;

private:

protected:

public:

	// Utilities | Benchmark

	/* Exactly 4 threshold values must be in the Tresholds array. They represent the quality levels (medium, high, epic, cinematic). If a threshold is not reached, return 'low'. This follows UE's original quality scalability system levels, except 'Custom'. The benchmark index to use is CPU, GPU, or the minimum of both. */
	static FString ComputeQualityPresetStringFromPerfIndex(float InCPUPerfIndex, float InGPUPerfIndex, E_BenchmarkHWIndexToUse InBenchmarkHWIndexToUse = E_BenchmarkHWIndexToUse::Min, const TArray<float>& InThresholds = TArray<float>({20, 50, 70, 90}));

	// Utilities | RHI

	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|RHI", meta = (CallableWithoutWorldContext))
		static E_RHIMode GetCurrentRenderHardwareInterface();

	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|RHI", meta = (CallableWithoutWorldContext))
		static bool IsRenderHardwareInterfaceSupported(E_RHIMode InRHI);

	// Utilities | Display

	/* Get Supported Screen Resolutions. Asserts if none could be retrieved or when a current resolution index could not be retrieved. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Display", meta = (CallableWithoutWorldContext))
		static TArray<FIntPoint> GetSupportedScreenResolutions();

	/* Returns true if the current resolution is present in GetSupportedScreenResolutions().  */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Display", meta = (CallableWithoutWorldContext))
		static bool IsScreenResolutionValid(const FIntPoint& InScreenResolution);

	/* Returns true if the current resolution is present in GetSupportedScreenResolutions().  */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Display", meta = (CallableWithoutWorldContext))
		static bool IsCurrentScreenResolutionValid();

	// Utilities | Conversion
	
	/* Converts between setting value and INI string value. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static FString KeyMappingsStructToString(const FS_KeyMappings& InKeyMappings);

	/* Converts between setting value and INI string value. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static FS_KeyMappings StringToKeyMappingsStruct(const FString& InKeyMappings);
	
	/* Converts between setting value and INI string value. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static FString BoolToString(bool InBool);
	
	/* Converts between setting value and INI string value. Unimplemented input will result in an assert. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static bool StringToBool(const FString& InString);

	/* Converts between setting value and INI string value. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static float StringToFloat(const FString& InString);

	/* Converts between setting value and INI string value. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static FString FloatToString(float InFloat);
	
	/* Converts between setting value and INI string value. Unimplemented input will result in an assert. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static E_InputKeyModes StringToInputKeyMode(const FString& InString);
	
	/* Converts between setting value and INI string value. Unimplemented input will result in an assert. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static FString InputKeyModeToString(E_InputKeyModes InInputKeyMode);
	
	/* Converts between setting value and INI string value. Unimplemented input will result in an assert. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static FString GetQualityPresetIntAsString(int32 InQualityPreset);
	
	/* Converts between setting value and INI string value. Unimplemented input will result in an assert. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static int32 GetQualityPresetStringAsInt(const FString& InQualityPreset);

	/* Converts between setting value and INI string value. Unimplemented input will result in an assert. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static FText GetScreenResolutionAsDisplayText(const FIntPoint& InResolution);

	/* Converts between setting value and INI string value, readable to "ScreenResolutionIntPointToString". */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static FIntPoint ScreenResolutionStringToIntPoint(const FString& InIntPointString);

	/* Converts an IntPoint to string, readable to "ScreenResolutionStringToIntPoint". */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static FString ScreenResolutionIntPointToString(const FIntPoint& InIntPoint);

	/* Converts between setting value and INI string value. Unimplemented input will result in an assert. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static TEnumAsByte<EWindowMode::Type> GetWindowModeFromString(const FString& InString);

	/* Converts between setting value and INI string value. Unimplemented input will result in an assert. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static FString GetStringFromWindowMode(TEnumAsByte<EWindowMode::Type> InWindowMode);

	/* Converts between setting value and INI string value. Unimplemented input will result in an assert. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static FString GetRHIModeAsString(E_RHIMode InMode);

	/* Converts between setting value and INI string value. Unimplemented input will result in an assert. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static E_RHIMode GetStringAsRHIMode(const FString& InString);

	/* Converts between setting value and INI string value. Unimplemented input will result in an assert. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static FString GetColorVisionDeficiencyAsString(EColorVisionDeficiency InColorVisionDeficiency);

	/* Converts between setting value and INI string value. Unimplemented input will result in an assert. */
	UFUNCTION(BlueprintCallable, Category = "BPFL|CustomSettingsUtils|Conversion", meta = (CallableWithoutWorldContext))
		static EColorVisionDeficiency GetStringAsColorVisionDeficiency(const FString& InString);

		
	// For the blueprint users, let's expose the constants (methods are quick to generate with notepad++ multi line editing):
	
	
	/* Setting sections */ 

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSectionWindowsTargetSettings();
		
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSectionCustomSettings();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSectionCustomSettingsProfile();
	
	/* Custom Setting Profiles */ 
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantCustomSettingsProfilesSaveGameName();
	
	/* Setting keys */ 
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantActionMappings();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantAxisMappings();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantDefaultGraphicsRHI();
	
	/* Settings (deduplicated), as used in the config sections */ 
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingAmbienceVolume();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingAmbientOcclusion();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingAntiAliasingMode();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingAntiAliasingQuality();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingBloom();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingDifficulty();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingDisplayResolution();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingFOV();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingFoliageQuality();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingGamepadInvertXAxisRotate();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingGamepadInvertYAxisRotate();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingGamepadRumble();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingGamepadXAxisRotateSensitivity();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingGamepadYAxisRotateSensitivity();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingGamma();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingGlobalIlluminationQuality();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingKeyboardMouseInvertXAxisRotate();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingKeyboardMouseInvertYAxisRotate();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingKeyboardMouseXAxisRotateSensitivity();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingKeyboardMouseYAxisRotateSensitivity();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingLanguage();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingLastUsedCustomSettingsProfileP0();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingLastUsedCustomSettingsProfileP1();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingLastUsedCustomSettingsProfileP2();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingLastUsedCustomSettingsProfileP3();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingLensFlares();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingLockFPS();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingKeybinding();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingMasterVolume();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingMotionBlur();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingMusicVolume();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingOverallQualityLevel();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingPostProcessingQuality();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingRaytracedAmbientOcclusion();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingRaytracedGlobalIllumination();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingRaytracedReflections();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingRaytracedShadows();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingRaytracedTranslucency();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingReflectionQuality();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingRenderHardwareInterface();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingResolutionScale();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingSFXVolume();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingSSR();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingShadingQuality();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingShadowQuality();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingShowFPSCounter();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingSubtitleMode();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingSubtitleScale();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingTextureQuality();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingUIScale();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingUIVolume();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingVSync();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingViewDistanceQuality();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingVisionColorDeficiencySeverity();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingVisionColorDeficiencyType();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingVisualEffectQuality();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingVoiceVolume();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingWeatherVolume();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantSettingWindowMode();
	
	/* String values (deduplicated) */ 
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValue30();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValue60();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValue90();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValue120();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValue240();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueAuto();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueCinematic();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueCustom();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueDeuteranope();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueEasy();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueEpic();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueExpert();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueExtraLarge();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueFullscreen();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueFXAA();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueHard();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueHigh();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueHold();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueLarge();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueLow();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueMedium();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueNone();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueNormal();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueOff();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueOn();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueProtanope();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueRHIDX11();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueRHIDX12();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueRHIVulkan();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueTAA();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueToggle();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueTritanope();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueUnlimited();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueWindowed();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BPFL|CustomSettingsUtils|Constants", meta = (CallableWithoutWorldContext))
		static const FString& ConstantValueWindowedFullscreen();

};