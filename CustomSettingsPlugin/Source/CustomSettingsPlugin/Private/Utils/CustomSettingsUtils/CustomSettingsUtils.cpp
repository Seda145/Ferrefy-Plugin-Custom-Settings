/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#include "CustomSettingsUtils.h"
#include "SettingSliderWidget.h"
#include "LocalizationUtils.h"
#include "SettingWidget.h"
#include "CustomSetting.h"
#include "CustomSliderSetting.h"
#include "SubsystemUtils.h"
#include "PlayerSettingsSubsystem.h"
#include "LogCustomSettingsPlugin.h"
#include "Internationalization/StringTable.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "UObject/Class.h"
#include "UObject/PropertyPortFlags.h"
#include "CoreGlobals.h"
#include "Modules/ModuleManager.h"
#include "DynamicRHI.h"
#include "UnrealEngine.h"
#include "Misc/FeedbackContext.h"


// Utilities | Benchmark

FString UCustomSettingsUtils::ComputeQualityPresetStringFromPerfIndex(float InCPUPerfIndex, float InGPUPerfIndex, E_BenchmarkHWIndexToUse InBenchmarkHWIndexToUse, const TArray<float>&  InThresholds) {
	if (InThresholds.Num() != 4) {
		UE_LOG(LogCustomSettingsPlugin, Error, TEXT("Exactly 4 threshold values must be in the Tresholds array. They represent the quality levels (medium, high, epic, cinematic)."));
		check(false);
	}

	float PerfIndex = 0.f;
	switch (InBenchmarkHWIndexToUse) {
	case (E_BenchmarkHWIndexToUse::Min):
		PerfIndex = FMath::Min(InCPUPerfIndex, InGPUPerfIndex);
		break;
	case (E_BenchmarkHWIndexToUse::CPU):
		PerfIndex = InCPUPerfIndex;
		break;
	case (E_BenchmarkHWIndexToUse::GPU):
		PerfIndex = InGPUPerfIndex;
		break;
	default:
		unimplemented();
	}

	FString OutValue = ValueLow;
	if (PerfIndex > InThresholds[3]) {
		OutValue = ValueCinematic;
	}
	else if (PerfIndex > InThresholds[2]) {
		OutValue = ValueEpic;
	}
	else if (PerfIndex > InThresholds[1]) {
		OutValue = ValueHigh;
	}
	else if (PerfIndex > InThresholds[0]) {
		OutValue = ValueMedium;
	}

	UE_LOG(LogCustomSettingsPlugin, Verbose, TEXT("Computed quality string value for benchmark: %s"), *OutValue);

	return OutValue;
}

// Utilities | RHI

E_RHIMode UCustomSettingsUtils::GetCurrentRenderHardwareInterface() {
	check(GDynamicRHI != nullptr);
	switch(GDynamicRHI->GetInterfaceType()) {
	case(ERHIInterfaceType::D3D11):
		return E_RHIMode::RHI_DX11;
	case(ERHIInterfaceType::D3D12):
		return E_RHIMode::RHI_DX12;
	case(ERHIInterfaceType::Vulkan):
		return E_RHIMode::RHI_VULKAN;
	default:
		unimplemented();
		return E_RHIMode::RHI_DX11;
	}
}

bool UCustomSettingsUtils::IsRenderHardwareInterfaceSupported(E_RHIMode InRHI) {
	/* Example files :
	* WindowsDynamicRHI.cpp
	* DynamicRHI.cpp
	*/
	switch(InRHI) {
	case (E_RHIMode::RHI_DX11):
		return FModuleManager::LoadModuleChecked<IDynamicRHIModule>(TEXT("D3D11RHI")).IsSupported();
	case (E_RHIMode::RHI_DX12):
		return FModuleManager::LoadModuleChecked<IDynamicRHIModule>(TEXT("D3D12RHI")).IsSupported();
	case (E_RHIMode::RHI_VULKAN):
		return FModuleManager::LoadModuleChecked<IDynamicRHIModule>(TEXT("VulkanRHI")).IsSupported();
	default:
		unimplemented();
		return false;
	}
}

// Utilities | Display

TArray<FIntPoint> UCustomSettingsUtils::GetSupportedScreenResolutions() {
	TArray<FIntPoint> SupportedScreenResolutions = TArray<FIntPoint>();

	if (GetCurrentRenderHardwareInterface() == E_RHIMode::RHI_VULKAN) {
		// Vulkan for whatever reason does not implement RHIGetAvailableResolutions, making it an odd one.
		// https://forums.unrealengine.com/t/ue5-1-1-supported-vulkan-rhi-returns-no-supported-screen-resolutions/873006
		SupportedScreenResolutions.Add(FIntPoint(GSystemResolution.ResX, GSystemResolution.ResY));
	}
	else {
		FScreenResolutionArray ResolutionsArray;
		check(GDynamicRHI != nullptr);
		GDynamicRHI->RHIGetAvailableResolutions(ResolutionsArray, true);
		for (const FScreenResolutionRHI& ResX : ResolutionsArray) {
			SupportedScreenResolutions.Add(FIntPoint(ResX.Width, ResX.Height));
		}
	}

	return SupportedScreenResolutions;
}

bool UCustomSettingsUtils::IsScreenResolutionValid(const FIntPoint& InScreenResolution) {
	return GetSupportedScreenResolutions().Contains(InScreenResolution);
}

bool UCustomSettingsUtils::IsCurrentScreenResolutionValid() {
	const bool bIsValid = IsScreenResolutionValid(FIntPoint(GSystemResolution.ResX, GSystemResolution.ResY));
	UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("current screen resolution is: %s."), (bIsValid ? TEXT("Valid") : TEXT("Invalid")));
	return bIsValid;
}

// Utilities | Conversion

FString UCustomSettingsUtils::KeyMappingsStructToString(const FS_KeyMappings& InKeyMappings) {
	const UScriptStruct* Struct = InKeyMappings.StaticStruct();
	FString Output = "";
	Struct->ExportText(Output, &InKeyMappings, nullptr, UCustomSettingsUtils::StaticClass(), (PPF_ExportsNotFullyQualified | PPF_Copy | PPF_Delimited | PPF_IncludeTransient), nullptr);
	return Output;
}

FS_KeyMappings UCustomSettingsUtils::StringToKeyMappingsStruct(const FString& InKeyMappings) {
	FS_KeyMappings KeyMappings;
	UScriptStruct* Struct = KeyMappings.StaticStruct();
	Struct->ImportText(*InKeyMappings, &KeyMappings, UCustomSettingsUtils::StaticClass(), PPF_None, GWarn, Struct->GetName());
	return KeyMappings;
}
 
FString UCustomSettingsUtils::BoolToString(bool InBool) {
	return (FString::FString(InBool ? ValueOn : ValueOff));
}

bool UCustomSettingsUtils::StringToBool(const FString& InString) {
	if (InString == ValueOn) {
		return true;
	}
	else if (InString == ValueOff) {
		return false;
	}

	unimplemented();
	return false;
}

float UCustomSettingsUtils::StringToFloat(const FString& InString) {
	return FCString::Atof(*InString);
}

FString UCustomSettingsUtils::FloatToString(float InFloat) {
	// 0 fract digits makes it work.
	return FString::SanitizeFloat(InFloat, 0);
}

E_InputKeyModes UCustomSettingsUtils::StringToInputKeyMode(const FString& InString) {
	if (InString == ValueToggle) {
		return E_InputKeyModes::Toggle;
	}
	else if  (InString == ValueHold) {
		return E_InputKeyModes::Hold;
	}

	unimplemented();
	return E_InputKeyModes::Hold;
}

FString UCustomSettingsUtils::InputKeyModeToString(E_InputKeyModes InInputKeyMode) {
	switch (InInputKeyMode) {
	case (E_InputKeyModes::Hold):
		return ValueHold;
	case (E_InputKeyModes::Toggle):
		return ValueToggle;
	}

	unimplemented();
	return "";
}

int32 UCustomSettingsUtils::GetQualityPresetStringAsInt(const FString& InQualityPreset) {
	// This follows UE's original quality scalability system indexes.

	if (InQualityPreset == ValueLow) {
		return 0;
	}
	else if (InQualityPreset == ValueMedium) {
		return 1;
	}
	else if (InQualityPreset == ValueHigh) {
		return 2;
	}
	else if (InQualityPreset == ValueEpic) {
		return 3;
	}
	else if (InQualityPreset == ValueCinematic) {
		return 4;
	}
	else if (InQualityPreset == ValueCustom) {
		return -1;
	}

	unimplemented();
	return INDEX_NONE;
}

FString UCustomSettingsUtils::GetQualityPresetIntAsString(int32 InQualityPreset) {
	// This follows UE's original quality scalability system indexes.

	switch (InQualityPreset) {
	case (0):
		return ValueLow;
	case (1):
		return ValueMedium;
	case (2):
		return ValueHigh;
	case (3):
		return ValueEpic;
	case (4):
		return ValueCinematic;
	case (-1):
		return ValueCustom;
	}

	unimplemented();
	return "";
}

FText UCustomSettingsUtils::GetScreenResolutionAsDisplayText(const FIntPoint& InResolution) {
	FFormatOrderedArguments Args;
	Args.Add(InResolution.X);
	Args.Add(InResolution.Y);
	return FText::Format(FTextFormat::FromString("{0} x {1}"), Args);
}

FIntPoint UCustomSettingsUtils::ScreenResolutionStringToIntPoint(const FString& InValue) {
	FString LeftStr = "";
	FString RightStr = "";
	InValue.Split("y", &LeftStr, &RightStr, ESearchCase::IgnoreCase, ESearchDir::FromStart);
	FString TempString = LeftStr;

	verify(TempString.Split("=", &LeftStr, &RightStr, ESearchCase::IgnoreCase, ESearchDir::FromEnd));
	const int32 X = FCString::Atoi(*RightStr);

	verify(InValue.Split("=", &LeftStr, &RightStr, ESearchCase::IgnoreCase, ESearchDir::FromEnd));
	const int32 Y = FCString::Atoi(*RightStr);

	return FIntPoint(X, Y);
}

FString UCustomSettingsUtils::ScreenResolutionIntPointToString(const FIntPoint& InIntPoint) {
	// UE_LOG(LogCustomSettingsPlugin, VeryVerbose, TEXT("ScreenResolutionIntPointToString: %s"), *InIntPoint.ToString());
	// Seems fine, use ToString for now.
	// verify(ScreenResolutionStringToIntPoint(InIntPoint.ToString()) == InIntPoint);
	return InIntPoint.ToString();
}

TEnumAsByte<EWindowMode::Type> UCustomSettingsUtils::GetWindowModeFromString(const FString& InString) {
	if (InString == ValueFullscreen) {
		return EWindowMode::Fullscreen;
	}
	else if (InString == ValueWindowedFullscreen) {
		return EWindowMode::WindowedFullscreen;
	}
	else if (InString == ValueWindowed) {
		return EWindowMode::Windowed;
	}

	unimplemented();
	return EWindowMode::Fullscreen;
}

FString UCustomSettingsUtils::GetStringFromWindowMode(TEnumAsByte<EWindowMode::Type> InWindowMode) {
	switch(InWindowMode) {
	case (EWindowMode::Fullscreen):
		return ValueFullscreen;
	case (EWindowMode::WindowedFullscreen):
		return ValueWindowedFullscreen;
	case (EWindowMode::Windowed):
		return ValueWindowed;
	}

	unimplemented();
	return "";
}

FString UCustomSettingsUtils::GetRHIModeAsString(E_RHIMode InMode) {
	switch(InMode) {
	case(E_RHIMode::RHI_DX11):
		return ValueRHIDX11;
	case(E_RHIMode::RHI_DX12):
		return ValueRHIDX12;
	case(E_RHIMode::RHI_VULKAN):
		return ValueRHIVulkan;
	}

	unimplemented();
	return "";
}

E_RHIMode UCustomSettingsUtils::GetStringAsRHIMode(const FString& InString) {
	if (InString == ValueRHIDX11) {
		return E_RHIMode::RHI_DX11;
	}
	else if (InString == ValueRHIDX12) {
		return E_RHIMode::RHI_DX12;
	}
	else if (InString == ValueRHIVulkan) {
		return E_RHIMode::RHI_VULKAN;
	}

	unimplemented();
	return E_RHIMode::RHI_VULKAN;
}

FString UCustomSettingsUtils::GetColorVisionDeficiencyAsString(EColorVisionDeficiency InColorVisionDeficiency) {
	switch(InColorVisionDeficiency) {
	case(EColorVisionDeficiency::NormalVision):
		return ValueNormal;
	case(EColorVisionDeficiency::Deuteranope):
		return ValueDeuteranope;
	case(EColorVisionDeficiency::Protanope):
		return ValueProtanope;
	case(EColorVisionDeficiency::Tritanope):
		return ValueTritanope;
	}

	unimplemented();
	return "";
}

EColorVisionDeficiency UCustomSettingsUtils::GetStringAsColorVisionDeficiency(const FString& InString) {
	if (InString == ValueNormal) {
		return EColorVisionDeficiency::NormalVision;
	}
	else if (InString == ValueDeuteranope) {
		return EColorVisionDeficiency::Deuteranope;
	}
	else if (InString == ValueProtanope) {
		return EColorVisionDeficiency::Protanope;
	}
	else if (InString == ValueTritanope) {
		return EColorVisionDeficiency::Tritanope;
	}

	unimplemented();
	return EColorVisionDeficiency::NormalVision;
}

/* Setting sections */
const FString UCustomSettingsUtils::SectionWindowsTargetSettings 			= TEXT("/Script/WindowsTargetPlatform.WindowsTargetSettings");
const FString UCustomSettingsUtils::SectionCustomSettings					= TEXT("/Script/CustomSettings");
const FString UCustomSettingsUtils::SectionCustomSettingsProfile			= TEXT("/Script/CustomSettings/Profile");

/* Custom Setting Profiles */
const FString UCustomSettingsUtils::CustomSettingsProfilesSaveGameName		= TEXT("CustomSettingsProfiles");

/* Setting keys */
const FString UCustomSettingsUtils::AxisMappings							= TEXT("AxisMappings");
const FString UCustomSettingsUtils::ActionMappings							= TEXT("ActionMappings");
const FString UCustomSettingsUtils::DefaultGraphicsRHI						= TEXT("DefaultGraphicsRHI");

/* Settings (deduplicated), as used in the config sections */
const FString UCustomSettingsUtils::SettingAmbienceVolume 						= TEXT("AmbienceVolume");
const FString UCustomSettingsUtils::SettingAmbientOcclusion 					= TEXT("AmbientOcclusion");
const FString UCustomSettingsUtils::SettingAntiAliasingMode 					= TEXT("AntiAliasingMode");
const FString UCustomSettingsUtils::SettingAntiAliasingQuality 					= TEXT("AntiAliasingQuality");
const FString UCustomSettingsUtils::SettingBloom 								= TEXT("Bloom");
const FString UCustomSettingsUtils::SettingDifficulty 							= TEXT("Difficulty");
const FString UCustomSettingsUtils::SettingDisplayResolution 					= TEXT("DisplayResolution");
const FString UCustomSettingsUtils::SettingFOV 									= TEXT("FOV");
const FString UCustomSettingsUtils::SettingFoliageQuality 						= TEXT("FoliageQuality");
const FString UCustomSettingsUtils::SettingGamepadInvertXAxisRotate				= TEXT("GamepadInvertXAxisRotate");
const FString UCustomSettingsUtils::SettingGamepadInvertYAxisRotate 			= TEXT("GamepadInvertYAxisRotate");
const FString UCustomSettingsUtils::SettingGamepadRumble 						= TEXT("GamepadRumble");
const FString UCustomSettingsUtils::SettingGamepadXAxisRotateSensitivity 		= TEXT("GamepadXAxisRotateSensitivity");
const FString UCustomSettingsUtils::SettingGamepadYAxisRotateSensitivity 		= TEXT("GamepadYAxisRotateSensitivity");
const FString UCustomSettingsUtils::SettingGamma 								= TEXT("Gamma");
const FString UCustomSettingsUtils::SettingGlobalIlluminationQuality			= TEXT("GlobalIlluminationQuality");
const FString UCustomSettingsUtils::SettingKeyboardMouseInvertXAxisRotate		= TEXT("KeyboardMouseInvertXAxisRotate");
const FString UCustomSettingsUtils::SettingKeyboardMouseInvertYAxisRotate 		= TEXT("KeyboardMouseInvertYAxisRotate");
const FString UCustomSettingsUtils::SettingKeyboardMouseXAxisRotateSensitivity 	= TEXT("KeyboardMouseXAxisRotateSensitivity");
const FString UCustomSettingsUtils::SettingKeyboardMouseYAxisRotateSensitivity 	= TEXT("KeyboardMouseYAxisRotateSensitivity");
const FString UCustomSettingsUtils::SettingLanguage 							= TEXT("Language");
const FString UCustomSettingsUtils::SettingLastUsedCustomSettingsProfileP0		= TEXT("LastUsedCustomSettingsProfileP0");
const FString UCustomSettingsUtils::SettingLastUsedCustomSettingsProfileP1		= TEXT("LastUsedCustomSettingsProfileP1");
const FString UCustomSettingsUtils::SettingLastUsedCustomSettingsProfileP2		= TEXT("LastUsedCustomSettingsProfileP2");
const FString UCustomSettingsUtils::SettingLastUsedCustomSettingsProfileP3		= TEXT("LastUsedCustomSettingsProfileP3");
const FString UCustomSettingsUtils::SettingLensFlares 							= TEXT("LensFlares");
const FString UCustomSettingsUtils::SettingLockFPS								= TEXT("LockFPS");
const FString UCustomSettingsUtils::SettingKeybinding							= TEXT("Keybinding");
const FString UCustomSettingsUtils::SettingMasterVolume 						= TEXT("MasterVolume");
const FString UCustomSettingsUtils::SettingMotionBlur 							= TEXT("MotionBlur");
const FString UCustomSettingsUtils::SettingMusicVolume 							= TEXT("MusicVolume");
const FString UCustomSettingsUtils::SettingOverallQualityLevel 					= TEXT("OverallQualityLevel");
const FString UCustomSettingsUtils::SettingPostProcessingQuality 				= TEXT("PostProcessingQuality");
const FString UCustomSettingsUtils::SettingRaytracedAmbientOcclusion 			= TEXT("RaytracedAmbientOcclusion");
const FString UCustomSettingsUtils::SettingRaytracedGlobalIllumination 			= TEXT("RaytracedGlobalIllumination");
const FString UCustomSettingsUtils::SettingRaytracedReflections 				= TEXT("RaytracedReflections");
const FString UCustomSettingsUtils::SettingRaytracedShadows 					= TEXT("RaytracedShadows");
const FString UCustomSettingsUtils::SettingRaytracedTranslucency 				= TEXT("RaytracedTranslucency");
const FString UCustomSettingsUtils::SettingReflectionQuality 					= TEXT("ReflectionQuality");
const FString UCustomSettingsUtils::SettingRenderHardwareInterface 				= TEXT("RenderHardwareInterface");
const FString UCustomSettingsUtils::SettingResolutionScale 						= TEXT("ResolutionScale");
const FString UCustomSettingsUtils::SettingSFXVolume 							= TEXT("SFXVolume");
const FString UCustomSettingsUtils::SettingSSR 									= TEXT("SSR");
const FString UCustomSettingsUtils::SettingShadingQuality 						= TEXT("ShadingQuality");
const FString UCustomSettingsUtils::SettingShadowQuality 						= TEXT("ShadowQuality");
const FString UCustomSettingsUtils::SettingShowFPSCounter						= TEXT("ShowFPSCounter");
const FString UCustomSettingsUtils::SettingSubtitleMode 						= TEXT("SubtitleMode");
const FString UCustomSettingsUtils::SettingSubtitleScale 						= TEXT("SubtitleScale");
const FString UCustomSettingsUtils::SettingTextureQuality 						= TEXT("TextureQuality");
const FString UCustomSettingsUtils::SettingUIScale 								= TEXT("UIScale");
const FString UCustomSettingsUtils::SettingUIVolume 							= TEXT("UIVolume");
const FString UCustomSettingsUtils::SettingVSync 								= TEXT("VSync");
const FString UCustomSettingsUtils::SettingViewDistanceQuality					= TEXT("ViewDistanceQuality");
const FString UCustomSettingsUtils::SettingVisionColorDeficiencySeverity 		= TEXT("VisionColorDeficiencySeverity");
const FString UCustomSettingsUtils::SettingVisionColorDeficiencyType 			= TEXT("VisionColorDeficiencyType");
const FString UCustomSettingsUtils::SettingVisualEffectQuality					= TEXT("VisualEffectQuality");
const FString UCustomSettingsUtils::SettingVoiceVolume 							= TEXT("VoiceVolume");
const FString UCustomSettingsUtils::SettingWeatherVolume 						= TEXT("WeatherVolume");
const FString UCustomSettingsUtils::SettingWindowMode 							= TEXT("WindowMode");

/* String values (deduplicated) */
const FString UCustomSettingsUtils::Value30					= TEXT("30");
const FString UCustomSettingsUtils::Value60					= TEXT("60");
const FString UCustomSettingsUtils::Value90					= TEXT("90");
const FString UCustomSettingsUtils::Value120				= TEXT("120");
const FString UCustomSettingsUtils::Value240				= TEXT("240");
const FString UCustomSettingsUtils::ValueAuto				= TEXT("Auto");
const FString UCustomSettingsUtils::ValueCinematic			= TEXT("Cinematic");
const FString UCustomSettingsUtils::ValueCustom				= TEXT("Custom");
const FString UCustomSettingsUtils::ValueDeuteranope		= TEXT("Deuteranope");
const FString UCustomSettingsUtils::ValueEasy				= TEXT("Easy");
const FString UCustomSettingsUtils::ValueEpic				= TEXT("Epic");
const FString UCustomSettingsUtils::ValueExpert				= TEXT("Expert");
const FString UCustomSettingsUtils::ValueExtraLarge			= TEXT("ExtraLarge");
const FString UCustomSettingsUtils::ValueFullscreen			= TEXT("Fullscreen");
const FString UCustomSettingsUtils::ValueFXAA				= TEXT("FXAA");
const FString UCustomSettingsUtils::ValueHard				= TEXT("Hard");
const FString UCustomSettingsUtils::ValueHigh				= TEXT("High");
const FString UCustomSettingsUtils::ValueHold				= TEXT("Hold");
const FString UCustomSettingsUtils::ValueLarge				= TEXT("Large");
const FString UCustomSettingsUtils::ValueLow				= TEXT("Low");
const FString UCustomSettingsUtils::ValueMedium				= TEXT("Medium");
const FString UCustomSettingsUtils::ValueNone				= TEXT("None");
const FString UCustomSettingsUtils::ValueNormal				= TEXT("Normal");
const FString UCustomSettingsUtils::ValueOff				= TEXT("Off");
const FString UCustomSettingsUtils::ValueOn					= TEXT("On");
const FString UCustomSettingsUtils::ValueProtanope			= TEXT("Protanope");
const FString UCustomSettingsUtils::ValueRHIDX11			= TEXT("DefaultGraphicsRHI_DX11");
const FString UCustomSettingsUtils::ValueRHIDX12			= TEXT("DefaultGraphicsRHI_DX12");
const FString UCustomSettingsUtils::ValueRHIVulkan			= TEXT("DefaultGraphicsRHI_Vulkan");
const FString UCustomSettingsUtils::ValueTAA				= TEXT("TAA");
const FString UCustomSettingsUtils::ValueToggle				= TEXT("Toggle");
const FString UCustomSettingsUtils::ValueTritanope			= TEXT("Tritanope");
const FString UCustomSettingsUtils::ValueUnlimited			= TEXT("Unlimited");
const FString UCustomSettingsUtils::ValueWindowed			= TEXT("Windowed");
const FString UCustomSettingsUtils::ValueWindowedFullscreen	= TEXT("WindowedFullscreen");


// For the blueprint users, let's expose the constants (methods are quick to generate with notepad++ multi line editing):


/* Setting sections */

const FString& UCustomSettingsUtils::ConstantSectionWindowsTargetSettings() {
	return SectionWindowsTargetSettings;
}

const FString& UCustomSettingsUtils::ConstantSectionCustomSettings() {
	return SectionCustomSettings;
}

const FString& UCustomSettingsUtils::ConstantSectionCustomSettingsProfile() {
	return SectionCustomSettingsProfile;
}

/* Custom Setting Profiles */

const FString& UCustomSettingsUtils::ConstantCustomSettingsProfilesSaveGameName() {
	return CustomSettingsProfilesSaveGameName;
}

/* Setting keys */

const FString& UCustomSettingsUtils::ConstantActionMappings() {
	return ActionMappings;
}

const FString& UCustomSettingsUtils::ConstantAxisMappings() {
	return AxisMappings;
}

const FString& UCustomSettingsUtils::ConstantDefaultGraphicsRHI() {
	return DefaultGraphicsRHI;
}

/* Settings (deduplicated), as used in the config sections */

const FString& UCustomSettingsUtils::ConstantSettingAmbienceVolume() {
	return SettingAmbienceVolume;
}

const FString& UCustomSettingsUtils::ConstantSettingAmbientOcclusion() {
	return SettingAmbientOcclusion;
}

const FString& UCustomSettingsUtils::ConstantSettingAntiAliasingMode() {
	return SettingAntiAliasingMode;
}

const FString& UCustomSettingsUtils::ConstantSettingAntiAliasingQuality() {
	return SettingAntiAliasingQuality;
}

const FString& UCustomSettingsUtils::ConstantSettingBloom() {
	return SettingBloom;
}

const FString& UCustomSettingsUtils::ConstantSettingDifficulty() {
	return SettingDifficulty;
}

const FString& UCustomSettingsUtils::ConstantSettingDisplayResolution() {
	return SettingDisplayResolution;
}

const FString& UCustomSettingsUtils::ConstantSettingFOV() {
	return SettingFOV;
}

const FString& UCustomSettingsUtils::ConstantSettingFoliageQuality() {
	return SettingFoliageQuality;
}

const FString& UCustomSettingsUtils::ConstantSettingGamepadInvertXAxisRotate() {
	return SettingGamepadInvertXAxisRotate;
}

const FString& UCustomSettingsUtils::ConstantSettingGamepadInvertYAxisRotate() {
	return SettingGamepadInvertYAxisRotate;
}

const FString& UCustomSettingsUtils::ConstantSettingGamepadRumble() {
	return SettingGamepadRumble;
}

const FString& UCustomSettingsUtils::ConstantSettingGamepadXAxisRotateSensitivity() {
	return SettingGamepadXAxisRotateSensitivity;
}

const FString& UCustomSettingsUtils::ConstantSettingGamepadYAxisRotateSensitivity() {
	return SettingGamepadYAxisRotateSensitivity;
}

const FString& UCustomSettingsUtils::ConstantSettingGamma() {
	return SettingGamma;
}

const FString& UCustomSettingsUtils::ConstantSettingGlobalIlluminationQuality() {
	return SettingGlobalIlluminationQuality;
}

const FString& UCustomSettingsUtils::ConstantSettingKeyboardMouseInvertXAxisRotate() {
	return SettingKeyboardMouseInvertXAxisRotate;
}

const FString& UCustomSettingsUtils::ConstantSettingKeyboardMouseInvertYAxisRotate() {
	return SettingKeyboardMouseInvertYAxisRotate;
}

const FString& UCustomSettingsUtils::ConstantSettingKeyboardMouseXAxisRotateSensitivity() {
	return SettingKeyboardMouseXAxisRotateSensitivity;
}

const FString& UCustomSettingsUtils::ConstantSettingKeyboardMouseYAxisRotateSensitivity() {
	return SettingKeyboardMouseYAxisRotateSensitivity;
}

const FString& UCustomSettingsUtils::ConstantSettingLanguage() {
	return SettingLanguage;
}

const FString& UCustomSettingsUtils::ConstantSettingLastUsedCustomSettingsProfileP0() {
	return SettingLastUsedCustomSettingsProfileP0;
}

const FString& UCustomSettingsUtils::ConstantSettingLastUsedCustomSettingsProfileP1() {
	return SettingLastUsedCustomSettingsProfileP1;
}

const FString& UCustomSettingsUtils::ConstantSettingLastUsedCustomSettingsProfileP2() {
	return SettingLastUsedCustomSettingsProfileP2;
}

const FString& UCustomSettingsUtils::ConstantSettingLastUsedCustomSettingsProfileP3() {
	return SettingLastUsedCustomSettingsProfileP3;
}

const FString& UCustomSettingsUtils::ConstantSettingLensFlares() {
	return SettingLensFlares;
}

const FString& UCustomSettingsUtils::ConstantSettingLockFPS() {
	return SettingLockFPS;
}

const FString& UCustomSettingsUtils::ConstantSettingKeybinding() {
	return SettingKeybinding;
}

const FString& UCustomSettingsUtils::ConstantSettingMasterVolume() {
	return SettingMasterVolume;
}

const FString& UCustomSettingsUtils::ConstantSettingMotionBlur() {
	return SettingMotionBlur;
}

const FString& UCustomSettingsUtils::ConstantSettingMusicVolume() {
	return SettingMusicVolume;
}

const FString& UCustomSettingsUtils::ConstantSettingOverallQualityLevel() {
	return SettingOverallQualityLevel;
}

const FString& UCustomSettingsUtils::ConstantSettingPostProcessingQuality() {
	return SettingPostProcessingQuality;
}

const FString& UCustomSettingsUtils::ConstantSettingRaytracedAmbientOcclusion() {
	return SettingRaytracedAmbientOcclusion;
}

const FString& UCustomSettingsUtils::ConstantSettingRaytracedGlobalIllumination() {
	return SettingRaytracedGlobalIllumination;
}

const FString& UCustomSettingsUtils::ConstantSettingRaytracedReflections() {
	return SettingRaytracedReflections;
}

const FString& UCustomSettingsUtils::ConstantSettingRaytracedShadows() {
	return SettingRaytracedShadows;
}

const FString& UCustomSettingsUtils::ConstantSettingRaytracedTranslucency() {
	return SettingRaytracedTranslucency;
}

const FString& UCustomSettingsUtils::ConstantSettingReflectionQuality() {
	return SettingReflectionQuality;
}

const FString& UCustomSettingsUtils::ConstantSettingRenderHardwareInterface() {
	return SettingRenderHardwareInterface;
}

const FString& UCustomSettingsUtils::ConstantSettingResolutionScale() {
	return SettingResolutionScale;
}

const FString& UCustomSettingsUtils::ConstantSettingSFXVolume() {
	return SettingSFXVolume;
}

const FString& UCustomSettingsUtils::ConstantSettingSSR() {
	return SettingSSR;
}

const FString& UCustomSettingsUtils::ConstantSettingShadingQuality() {
	return SettingShadingQuality;
}

const FString& UCustomSettingsUtils::ConstantSettingShadowQuality() {
	return SettingShadowQuality;
}

const FString& UCustomSettingsUtils::ConstantSettingShowFPSCounter() {
	return SettingShowFPSCounter;
}

const FString& UCustomSettingsUtils::ConstantSettingSubtitleMode() {
	return SettingSubtitleMode;
}

const FString& UCustomSettingsUtils::ConstantSettingSubtitleScale() {
	return SettingSubtitleScale;
}

const FString& UCustomSettingsUtils::ConstantSettingTextureQuality() {
	return SettingTextureQuality;
}

const FString& UCustomSettingsUtils::ConstantSettingUIScale() {
	return SettingUIScale;
}

const FString& UCustomSettingsUtils::ConstantSettingUIVolume() {
	return SettingUIVolume;
}

const FString& UCustomSettingsUtils::ConstantSettingVSync() {
	return SettingVSync;
}

const FString& UCustomSettingsUtils::ConstantSettingViewDistanceQuality() {
	return SettingViewDistanceQuality;
}

const FString& UCustomSettingsUtils::ConstantSettingVisionColorDeficiencySeverity() {
	return SettingVisionColorDeficiencySeverity;
}

const FString& UCustomSettingsUtils::ConstantSettingVisionColorDeficiencyType() {
	return SettingVisionColorDeficiencyType;
}

const FString& UCustomSettingsUtils::ConstantSettingVisualEffectQuality() {
	return SettingVisualEffectQuality;
}

const FString& UCustomSettingsUtils::ConstantSettingVoiceVolume() {
	return SettingVoiceVolume;
}

const FString& UCustomSettingsUtils::ConstantSettingWeatherVolume() {
	return SettingWeatherVolume;
}

const FString& UCustomSettingsUtils::ConstantSettingWindowMode() {
	return SettingWindowMode;
}

/* String values (deduplicated) */

const FString& UCustomSettingsUtils::ConstantValue30() {
	return Value30;
}

const FString& UCustomSettingsUtils::ConstantValue60() {
	return Value60;
}

const FString& UCustomSettingsUtils::ConstantValue90() {
	return Value90;
}

const FString& UCustomSettingsUtils::ConstantValue120() {
	return Value120;
}

const FString& UCustomSettingsUtils::ConstantValue240() {
	return Value240;
}

const FString& UCustomSettingsUtils::ConstantValueAuto() {
	return ValueAuto;
}

const FString& UCustomSettingsUtils::ConstantValueCinematic() {
	return ValueCinematic;
}

const FString& UCustomSettingsUtils::ConstantValueCustom() {
	return ValueCustom;
}

const FString& UCustomSettingsUtils::ConstantValueDeuteranope() {
	return ValueDeuteranope;
}

const FString& UCustomSettingsUtils::ConstantValueEasy() {
	return ValueEasy;
}

const FString& UCustomSettingsUtils::ConstantValueEpic() {
	return ValueEpic;
}

const FString& UCustomSettingsUtils::ConstantValueExpert() {
	return ValueExpert;
}

const FString& UCustomSettingsUtils::ConstantValueExtraLarge() {
	return ValueExtraLarge;
}

const FString& UCustomSettingsUtils::ConstantValueFullscreen() {
	return ValueFullscreen;
}

const FString& UCustomSettingsUtils::ConstantValueFXAA() {
	return ValueFXAA;
}

const FString& UCustomSettingsUtils::ConstantValueHard() {
	return ValueHard;
}

const FString& UCustomSettingsUtils::ConstantValueHigh() {
	return ValueHigh;
}

const FString& UCustomSettingsUtils::ConstantValueHold() {
	return ValueHold;
}

const FString& UCustomSettingsUtils::ConstantValueLarge() {
	return ValueLarge;
}

const FString& UCustomSettingsUtils::ConstantValueLow() {
	return ValueLow;
}

const FString& UCustomSettingsUtils::ConstantValueMedium() {
	return ValueMedium;
}

const FString& UCustomSettingsUtils::ConstantValueNone() {
	return ValueNone;
}

const FString& UCustomSettingsUtils::ConstantValueNormal() {
	return ValueNormal;
}

const FString& UCustomSettingsUtils::ConstantValueOff() {
	return ValueOff;
}

const FString& UCustomSettingsUtils::ConstantValueOn() {
	return ValueOn;
}

const FString& UCustomSettingsUtils::ConstantValueProtanope() {
	return ValueProtanope;
}

const FString& UCustomSettingsUtils::ConstantValueRHIDX11() {
	return ValueRHIDX11;
}

const FString& UCustomSettingsUtils::ConstantValueRHIDX12() {
	return ValueRHIDX12;
}

const FString& UCustomSettingsUtils::ConstantValueRHIVulkan() {
	return ValueRHIVulkan;
}

const FString& UCustomSettingsUtils::ConstantValueTAA() {
	return ValueTAA;
}

const FString& UCustomSettingsUtils::ConstantValueToggle() {
	return ValueToggle;
}

const FString& UCustomSettingsUtils::ConstantValueTritanope() {
	return ValueTritanope;
}

const FString& UCustomSettingsUtils::ConstantValueUnlimited() {
	return ValueUnlimited;
}

const FString& UCustomSettingsUtils::ConstantValueWindowed() {
	return ValueWindowed;
}

const FString& UCustomSettingsUtils::ConstantValueWindowedFullscreen() {
	return ValueWindowedFullscreen;
}
