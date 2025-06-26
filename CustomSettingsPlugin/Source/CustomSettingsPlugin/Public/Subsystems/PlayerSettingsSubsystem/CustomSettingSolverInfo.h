/**Copyright 2025: Roy Wierer (Ferrefy). All Rights Reserved.**/
#pragma once

#include "CoreMinimal.h"
#include "Misc/Crc.h"

#include "CustomSettingSolverInfo.generated.h"


class UCustomSetting;


/* Struct so we can nest NotCompatible inside TMap for BP users. */
USTRUCT(BlueprintType)
struct CUSTOMSETTINGSPLUGIN_API FS_SettingSolverNotCompatibleValues {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TSet<FString> Values;

    // Initialize
    FS_SettingSolverNotCompatibleValues() 
        : Values (TSet<FString>())
    {}
    FS_SettingSolverNotCompatibleValues(const FS_SettingSolverNotCompatibleValues& InOther) 
        : Values (InOther.Values)
    {}
    FS_SettingSolverNotCompatibleValues(const TSet<FString>& InValues) 
        : Values (InValues)
    {}

    // Operators
    //bool operator==(const FS_SettingSolverNotCompatibleValues& InOther) const {
    //    return (

    //    );
    //}
};


USTRUCT(BlueprintType)
struct CUSTOMSETTINGSPLUGIN_API FS_SettingSolverValueSetterBehavior {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString ValueName;

    /* If a value is required by a Set (to be compatible), or if application can be skipped. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        bool bRequire;

    // Initialize
    FS_SettingSolverValueSetterBehavior()
        : ValueName ("")
        , bRequire (false)
    {}
    FS_SettingSolverValueSetterBehavior(const FS_SettingSolverValueSetterBehavior& InOther)
        : ValueName (InOther.ValueName)
        , bRequire (InOther.bRequire)
    {}
    FS_SettingSolverValueSetterBehavior(const FString& InValueName, bool bInRequire = false)
        : ValueName (InValueName)
        , bRequire (bInRequire)
    {}

    // Operators
    bool operator==(const FS_SettingSolverValueSetterBehavior& Other) const {
        return (
            ValueName == Other.ValueName
        );
    }
};


FORCEINLINE uint32 GetTypeHash(const FS_SettingSolverValueSetterBehavior& InStruct) {
    return FCrc::MemCrc32(&InStruct, sizeof(FS_SettingSolverValueSetterBehavior));
}


USTRUCT(BlueprintType)
struct CUSTOMSETTINGSPLUGIN_API FS_SettingSolverValueInfo {
    GENERATED_BODY()

    /* Name of this value. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString ValueName;
    
    /* Map of Settings : Values. If this setting value is not compatible with another. Not changed after setup. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TMap<FString, FS_SettingSolverNotCompatibleValues> NotCompatible;

    /* Map of Settings : Value. If this setting value applies other settings. Not changed after setup. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TMap<FString, FS_SettingSolverValueSetterBehavior> Sets;

    // Initialize
    FS_SettingSolverValueInfo() 
        : ValueName ("")
        , NotCompatible (TMap<FString, FS_SettingSolverNotCompatibleValues>())
        , Sets (TMap<FString, FS_SettingSolverValueSetterBehavior>())
    {}
    FS_SettingSolverValueInfo(const FS_SettingSolverValueInfo& InOther) 
        : ValueName (InOther.ValueName)
        , NotCompatible (InOther.NotCompatible)
        , Sets (InOther.Sets)
    {}
    FS_SettingSolverValueInfo(const FString& InValueName) 
        : ValueName (InValueName)
        , NotCompatible (TMap<FString, FS_SettingSolverNotCompatibleValues>())
        , Sets (TMap<FString, FS_SettingSolverValueSetterBehavior>())
    {}
    FS_SettingSolverValueInfo(const FString& InValueName, const TMap<FString, FS_SettingSolverNotCompatibleValues>& InNotCompatible, const TMap<FString, FS_SettingSolverValueSetterBehavior>& InSets) 
        : ValueName (InValueName)
        , NotCompatible (InNotCompatible)
        , Sets (InSets)
    {}

    // Operators
    bool operator==(const FS_SettingSolverValueInfo& InOther) const {
        return (
            ValueName == InOther.ValueName
        );
    }
};


FORCEINLINE uint32 GetTypeHash(const FS_SettingSolverValueInfo& InStruct) {
    return FCrc::MemCrc32(&InStruct, sizeof(FS_SettingSolverValueInfo));
}


USTRUCT(BlueprintType)
struct CUSTOMSETTINGSPLUGIN_API FS_SettingSolverValueRegistration {
    GENERATED_BODY()

    /* Name of this value. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString ValueName;

    /* Map of Settings : Values. If this setting value is not compatible with another within the setting solver. Not changed after setup. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TMap<FString, FS_SettingSolverNotCompatibleValues> NotCompatible;

    /* Map of Settings : Value. If this setting value applies other settings within the setting solver. Not changed after setup. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TMap<FString, FS_SettingSolverValueSetterBehavior> Sets;

    /* Map of Settings : Value. If this setting value applies another setting within the setting solver. Not changed after setup. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TMap<FString, FS_SettingSolverValueSetterBehavior> SetBy;

    // Initialize
    FS_SettingSolverValueRegistration() 
        : ValueName ("")
        , NotCompatible (TMap<FString, FS_SettingSolverNotCompatibleValues>())
        , Sets (TMap<FString, FS_SettingSolverValueSetterBehavior>())
        , SetBy (TMap<FString, FS_SettingSolverValueSetterBehavior>())
    {}
    FS_SettingSolverValueRegistration(const FS_SettingSolverValueRegistration& InOther) 
        : ValueName (InOther.ValueName)
        , NotCompatible (InOther.NotCompatible)
        , Sets (InOther.Sets)
        , SetBy (InOther.SetBy)
    {}
    FS_SettingSolverValueRegistration(const FString& InValueName) 
        : ValueName (InValueName)
        , NotCompatible (TMap<FString, FS_SettingSolverNotCompatibleValues>())
        , Sets (TMap<FString, FS_SettingSolverValueSetterBehavior>())
        , SetBy (TMap<FString, FS_SettingSolverValueSetterBehavior>())
    {}
    FS_SettingSolverValueRegistration(const FString& InValueName, const TMap<FString, FS_SettingSolverNotCompatibleValues>& InNotCompatible, const TMap<FString, FS_SettingSolverValueSetterBehavior>& InSets, const TMap<FString, FS_SettingSolverValueSetterBehavior>& InSetBy) 
        : ValueName (InValueName)
        , NotCompatible (InNotCompatible)
        , Sets (InSets)
        , SetBy (InSetBy)
    {}

    // Operators
    bool operator==(const FS_SettingSolverValueRegistration& InOther) const {
        return (
            ValueName == InOther.ValueName
        );
    }
};


FORCEINLINE uint32 GetTypeHash(const FS_SettingSolverValueRegistration& InStruct) {
    return FCrc::MemCrc32(&InStruct, sizeof(FS_SettingSolverValueRegistration));
}


USTRUCT(BlueprintType)
struct CUSTOMSETTINGSPLUGIN_API FS_SettingSolverSettingRegistration {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString SettingName;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FS_SettingSolverValueRegistration> ValueRegistrations;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        bool bSettingDirectlyCausesGlobalEffect;

    // Initialize
    FS_SettingSolverSettingRegistration() 
        : SettingName ("")
        , ValueRegistrations (TArray<FS_SettingSolverValueRegistration>())
        , bSettingDirectlyCausesGlobalEffect (false)
    {}
    FS_SettingSolverSettingRegistration(const FString& InSettingName, const TArray<FS_SettingSolverValueRegistration>& InValueRegists) 
        : SettingName (InSettingName)
        , ValueRegistrations (InValueRegists)
        , bSettingDirectlyCausesGlobalEffect (false)
    {}
    FS_SettingSolverSettingRegistration(const FString& InSettingName, const TArray<FS_SettingSolverValueRegistration>& InValueRegists, bool bInSettingDirectlyCausesGlobalEffect) 
        : SettingName (InSettingName)
        , ValueRegistrations (InValueRegists)
        , bSettingDirectlyCausesGlobalEffect (bInSettingDirectlyCausesGlobalEffect)
    {}

    // Operators
    bool operator==(const FS_SettingSolverSettingRegistration& InOther) const {
        return (
            SettingName == InOther.SettingName
        );
    }
};


FORCEINLINE uint32 GetTypeHash(const FS_SettingSolverSettingRegistration& InStruct) {
    return FCrc::MemCrc32(&InStruct, sizeof(FS_SettingSolverSettingRegistration));
}


USTRUCT(BlueprintType)
struct CUSTOMSETTINGSPLUGIN_API FS_SettingSolverSettingInfo {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UCustomSetting* SettingObject;

    /**
    * Array storing the values of this setting. Not a TMap of name to value
    * , because array keeps registration order (for choice navigation) and TMap does not. 
    */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FS_SettingSolverValueInfo> ValueInfo;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TSet<FString> LockedBySettings;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        bool bSettingDependsOnGlobalEffect;

    // Initialize
    FS_SettingSolverSettingInfo() 
        : SettingObject (nullptr)
        , ValueInfo (TArray<FS_SettingSolverValueInfo>())
        , LockedBySettings (TSet<FString>())
        , bSettingDependsOnGlobalEffect (false)
    {}
    FS_SettingSolverSettingInfo(const FS_SettingSolverSettingInfo& InOther) 
        : SettingObject (InOther.SettingObject)
        , ValueInfo (InOther.ValueInfo)
        , LockedBySettings (InOther.LockedBySettings)
        , bSettingDependsOnGlobalEffect (InOther.bSettingDependsOnGlobalEffect)
    {}
    FS_SettingSolverSettingInfo(UCustomSetting* InSettingObject, const TArray<FS_SettingSolverValueInfo>& InValueInfo, const TSet<FString>& InLockedBySettings, bool bInSettingDirectlyCausesGlobalEffect) 
        : SettingObject (InSettingObject)
        , ValueInfo (InValueInfo)
        , LockedBySettings (InLockedBySettings)
        , bSettingDependsOnGlobalEffect (bInSettingDirectlyCausesGlobalEffect)
    {}

    // Operators
    bool operator==(const FS_SettingSolverSettingInfo& InOther) const {
        return (
            ValueInfo == InOther.ValueInfo
        );
    }
};


FORCEINLINE uint32 GetTypeHash(const FS_SettingSolverSettingInfo& InStruct) {
    return FCrc::MemCrc32(&InStruct, sizeof(FS_SettingSolverSettingInfo));
}


USTRUCT(BlueprintType)
struct CUSTOMSETTINGSPLUGIN_API FS_SettingSolverInfo {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TMap<FString, FS_SettingSolverSettingInfo> Settings;

    // Initialize
    FS_SettingSolverInfo() 
        : Settings (TMap<FString, FS_SettingSolverSettingInfo>())
    {}
    FS_SettingSolverInfo(const FS_SettingSolverInfo& InOther) 
        : Settings (InOther.Settings)
    {}
    FS_SettingSolverInfo(const TMap<FString, FS_SettingSolverSettingInfo>& InSettings) 
        : Settings (InSettings)
    {}

    // Operators
    bool operator==(const FS_SettingSolverInfo& InOther) const {
        return (
            InOther.Settings.OrderIndependentCompareEqual(InOther.Settings)
        );
    }
};


FORCEINLINE uint32 GetTypeHash(const FS_SettingSolverInfo& InStruct) {
    return FCrc::MemCrc32(&InStruct, sizeof(FS_SettingSolverInfo));
}


USTRUCT(BlueprintType)
struct CUSTOMSETTINGSPLUGIN_API FS_SettingSolverSimSettingBranch {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Setting;

    /* Value we want to change to if possible */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString DesiredValue;

    /* To apply this branch, the settings + value in RequiredCue must be applied */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TMap<FString, FString> RequiredSetCue;

    // Initialize
    FS_SettingSolverSimSettingBranch() 
        : Setting ("")
        , DesiredValue ("")
        , RequiredSetCue (TMap<FString, FString>())
    {}
    FS_SettingSolverSimSettingBranch(const FS_SettingSolverSimSettingBranch& InOther) 
        : Setting (InOther.Setting)
        , DesiredValue (InOther.DesiredValue)
        , RequiredSetCue (InOther.RequiredSetCue)
    {}
    FS_SettingSolverSimSettingBranch(const FString& InSetting, const FString& InDesiredValue, const TMap<FString, FString>& InRequiredSetCue, const TMap<FString, FString>& InOptionalSetCue) 
        : Setting (InSetting)
        , DesiredValue (InDesiredValue)
        , RequiredSetCue (InRequiredSetCue)
    {}

    // Operators
    bool operator==(const FS_SettingSolverSimSettingBranch& InOther) const {
        return (
            Setting == InOther.Setting
        );
    }
};


FORCEINLINE uint32 GetTypeHash(const FS_SettingSolverSimSettingBranch& InStruct) {
    return FCrc::MemCrc32(&InStruct, sizeof(FS_SettingSolverSimSettingBranch));
}


USTRUCT(BlueprintType)
struct CUSTOMSETTINGSPLUGIN_API FS_SettingSolverSimBranchCue {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Setting;

    /* Value we want to change to if possible */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Value;

    /* If this branch is required to apply, or optional */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        bool bIsRequired;

    // Initialize
    FS_SettingSolverSimBranchCue() 
        : Setting ("")
        , Value ("")
        , bIsRequired (true)
    {}
    FS_SettingSolverSimBranchCue(const FS_SettingSolverSimBranchCue& InOther) 
        : Setting (InOther.Setting)
        , Value (InOther.Value)
        , bIsRequired (InOther.bIsRequired)
    {}
    FS_SettingSolverSimBranchCue(const FString& InSetting, const FString& InValue, bool bInIsRequired) 
        : Setting (InSetting)
        , Value (InValue)
        , bIsRequired (bInIsRequired)
    {}

    // Operators
    bool operator==(const FS_SettingSolverSimBranchCue& InOther) const {
        return (
            Setting == InOther.Setting
        );
    }
};


FORCEINLINE uint32 GetTypeHash(const FS_SettingSolverSimBranchCue& InStruct) {
    return FCrc::MemCrc32(&InStruct, sizeof(FS_SettingSolverSimBranchCue));
}
