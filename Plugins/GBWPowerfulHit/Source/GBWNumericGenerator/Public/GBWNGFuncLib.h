// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GBWNGFuncLib.generated.h"

UCLASS()
class GBWNUMERICGENERATOR_API UGBWNGFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static bool GetNGValue_Bool(AActor* OwnerActor, FNG_Bool NG_Bool, bool DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static float GetNGValue_Float(AActor* OwnerActor, FNG_Float NG_Float, float DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static FVector GetNGValue_Vector(AActor* OwnerActor, FNG_Vector NG_Vector, FVector DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static FRotator GetNGValue_Rotator(AActor* OwnerActor, FNG_Rotator NG_Rotator, FRotator DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static FTransform GetNGValue_Transform(AActor* OwnerActor, FNG_Transform NG_Transform, FTransform DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static FGameplayTag GetNGValue_GameplayTag(AActor* OwnerActor, FNG_GameplayTag NG_GameplayTag, FGameplayTag DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static FText GetNGValue_Text(AActor* OwnerActor, FNG_Text NG_Text, FText DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static FString GetNGValue_String(AActor* OwnerActor, FNG_String NG_String, FString DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static FName GetNGValue_Name(AActor* OwnerActor, FNG_Name NG_Name, FName DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static UCurveFloat* GetNGValue_CurveFloat(AActor* OwnerActor, FNG_CurveFloat NG_CurveFloat, UCurveFloat* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static UCurveVector* GetNGValue_CurveVector(AActor* OwnerActor, FNG_CurveVector NG_CurveVector, UCurveVector* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static UAnimMontage* GetNGValue_Montage(AActor* OwnerActor, FNG_Montage NG_Montage, UAnimMontage* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static UAnimSequence* GetNGValue_AnimSequence(AActor* OwnerActor, FNG_AnimSequence NG_AnimSequence, UAnimSequence* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static UBlendSpace* GetNGValue_AnimBlendSpace(AActor* OwnerActor, FNG_AnimBlendSpace NG_AnimBlendSpace, UBlendSpace* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static UAnimBlueprint* GetNGValue_AnimBlueprint(AActor* OwnerActor, FNG_AnimBlueprint NG_AnimBlueprint, UAnimBlueprint* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static USkeletalMesh* GetNGValue_SkeletalMesh(AActor* OwnerActor, FNG_SkeletalMesh NG_SkeletalMesh, USkeletalMesh* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static UStaticMesh* GetNGValue_StaticMesh(AActor* OwnerActor, FNG_StaticMesh NG_StaticMesh, UStaticMesh* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static UNiagaraSystem* GetNGValue_Niagara(AActor* OwnerActor, FNG_Niagara NG_Niagara, UNiagaraSystem* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	static USoundBase* GetNGValue_Sound(AActor* OwnerActor, FNG_Sound NG_Sound, USoundBase* DefaultValue);

	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static bool GetNGOValue_Bool(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, bool DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static float GetNGOValue_Float(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, float DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static FVector GetNGOValue_Vector(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, FVector DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static FRotator GetNGOValue_Rotator(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, FRotator DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static FTransform GetNGOValue_Transform(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, FTransform DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static FGameplayTag GetNGOValue_GameplayTag(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, FGameplayTag DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static FText GetNGOValue_Text(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, FText DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static FString GetNGOValue_String(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, FString DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static FName GetNGOValue_Name(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, FName DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static UCurveFloat* GetNGOValue_CurveFloat(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, UCurveFloat* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static UCurveVector* GetNGOValue_CurveVector(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, UCurveVector* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static UAnimMontage* GetNGOValue_Montage(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, UAnimMontage* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static UAnimSequence* GetNGOValue_AnimSequence(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, UAnimSequence* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static UBlendSpace* GetNGOValue_AnimBlendSpace(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, UBlendSpace* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static UAnimBlueprint* GetNGOValue_AnimBlueprint(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, UAnimBlueprint* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static USkeletalMesh* GetNGOValue_SkeletalMesh(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, USkeletalMesh* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static UStaticMesh* GetNGOValue_StaticMesh(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, UStaticMesh* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static UNiagaraSystem* GetNGOValue_Niagara(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, UNiagaraSystem* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Object", meta=(DefaultToSelf="OwnerActor"))
	static USoundBase* GetNGOValue_Sound(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, USoundBase* DefaultValue);

	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static bool GetDANGValue_Bool(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag,  bool DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static float GetDANGValue_Float(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag,  float DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static FVector GetDANGValue_Vector(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag,  FVector DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static FRotator GetDANGValue_Rotator(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag,  FRotator DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static FTransform GetDANGValue_Transform(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag,  FTransform DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static FGameplayTag GetDANGValue_GameplayTag(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag,  FGameplayTag DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static FText GetDANGValue_Text(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag, FText DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static FString GetDANGValue_String(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag, FString DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static FName GetDANGValue_Name(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag, FName DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static UCurveFloat* GetDANGValue_CurveFloat(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag,  UCurveFloat* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static UCurveVector* GetDANGValue_CurveVector(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag,  UCurveVector* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static UAnimMontage* GetDANGValue_Montage(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag,  UAnimMontage* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static UAnimSequence* GetDANGValue_AnimSequence(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag,  UAnimSequence* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static UBlendSpace* GetDANGValue_AnimBlendSpace(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag,  UBlendSpace* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static UAnimBlueprint* GetDANGValue_AnimBlueprint(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag,  UAnimBlueprint* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static USkeletalMesh* GetDANGValue_SkeletalMesh(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag,  USkeletalMesh* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static UStaticMesh* GetDANGValue_StaticMesh(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag,  UStaticMesh* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static UNiagaraSystem* GetDANGValue_Niagara(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag,  UNiagaraSystem* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|DataAsset", meta=(DefaultToSelf="OwnerActor"))
	static USoundBase* GetDANGValue_Sound(AActor* OwnerActor,  UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag, FGameplayTag ValueTag,  USoundBase* DefaultValue);

	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static bool GetNGV_Bool(AActor* OwnerActor,  FNGV_Bool NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static float GetNGV_Float(AActor* OwnerActor,  FNGV_Float NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static FVector GetNGV_Vector(AActor* OwnerActor,  FNGV_Vector NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static FRotator GetNGV_Rotator(AActor* OwnerActor,  FNGV_Rotator NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static FTransform GetNGV_Transform(AActor* OwnerActor,  FNGV_Transform NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static FGameplayTag GetNGV_GameplayTag(AActor* OwnerActor,  FNGV_GameplayTag NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static FText GetNGV_Text(AActor* OwnerActor,  FNGV_Text NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static FString GetNGV_String(AActor* OwnerActor,  FNGV_String NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static FName GetNGV_Name(AActor* OwnerActor,  FNGV_Name NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static UCurveFloat* GetNGV_CurveFloat(AActor* OwnerActor,  FNGV_CurveFloat NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static UCurveVector* GetNGV_CurveVector(AActor* OwnerActor,  FNGV_CurveVector NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static UAnimMontage* GetNGV_Montage(AActor* OwnerActor,  FNGV_Montage NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static UAnimSequence* GetNGV_AnimSequence(AActor* OwnerActor,  FNGV_AnimSequence NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static UBlendSpace* GetNGV_AnimBlendSpace(AActor* OwnerActor,  FNGV_AnimBlendSpace NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static UAnimBlueprint* GetNGV_AnimBlueprint(AActor* OwnerActor,  FNGV_AnimBlueprint NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static USkeletalMesh* GetNGV_SkeletalMesh(AActor* OwnerActor,  FNGV_SkeletalMesh NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static UStaticMesh* GetNGV_StaticMesh(AActor* OwnerActor,  FNGV_StaticMesh NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static UNiagaraSystem* GetNGV_Niagara(AActor* OwnerActor,  FNGV_Niagara NGV);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|NGValue", meta=(DefaultToSelf="OwnerActor"))
	static USoundBase* GetNGV_Sound(AActor* OwnerActor,  FNGV_Sound NGV);
};
