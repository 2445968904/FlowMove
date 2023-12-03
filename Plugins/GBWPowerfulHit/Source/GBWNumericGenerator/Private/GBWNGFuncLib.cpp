// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "GBWNGFuncLib.h"

#include "GBWNGDataAsset.h"

bool UGBWNGFuncLib::GetNGValue_Bool(AActor* OwnerActor, FNG_Bool NG_Bool, bool DefaultValue)
{
	if (NG_Bool.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_Bool.Value;
	}

	if (NG_Bool.Generator)
	{
		return NG_Bool.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

float UGBWNGFuncLib::GetNGValue_Float(AActor* OwnerActor, FNG_Float NG_Float, float DefaultValue)
{
	if (NG_Float.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_Float.Value;
	}

	if (NG_Float.Generator)
	{
		return NG_Float.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

FVector UGBWNGFuncLib::GetNGValue_Vector(AActor* OwnerActor, FNG_Vector NG_Vector, FVector DefaultValue)
{
	if (NG_Vector.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_Vector.Value;
	}

	if (NG_Vector.Generator)
	{
		return NG_Vector.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

FRotator UGBWNGFuncLib::GetNGValue_Rotator(AActor* OwnerActor, FNG_Rotator NG_Rotator, FRotator DefaultValue)
{
	if (NG_Rotator.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_Rotator.Value;
	}

	if (NG_Rotator.Generator)
	{
		return NG_Rotator.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

FTransform UGBWNGFuncLib::GetNGValue_Transform(AActor* OwnerActor, FNG_Transform NG_Transform, FTransform DefaultValue)
{
	if (NG_Transform.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_Transform.Value;
	}

	if (NG_Transform.Generator)
	{
		return NG_Transform.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

FGameplayTag UGBWNGFuncLib::GetNGValue_GameplayTag(AActor* OwnerActor, FNG_GameplayTag NG_GameplayTag, FGameplayTag DefaultValue)
{
	if (NG_GameplayTag.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_GameplayTag.Value;
	}

	if (NG_GameplayTag.Generator)
	{
		return NG_GameplayTag.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

FText UGBWNGFuncLib::GetNGValue_Text(AActor* OwnerActor, FNG_Text NG_Text, FText DefaultValue)
{
	if (NG_Text.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_Text.Value;
	}

	if (NG_Text.Generator)
	{
		return NG_Text.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

FString UGBWNGFuncLib::GetNGValue_String(AActor* OwnerActor, FNG_String NG_String, FString DefaultValue)
{
	if (NG_String.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_String.Value;
	}

	if (NG_String.Generator)
	{
		return NG_String.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

FName UGBWNGFuncLib::GetNGValue_Name(AActor* OwnerActor, FNG_Name NG_Name, FName DefaultValue)
{
	if (NG_Name.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_Name.Value;
	}

	if (NG_Name.Generator)
	{
		return NG_Name.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

UCurveFloat* UGBWNGFuncLib::GetNGValue_CurveFloat(AActor* OwnerActor, FNG_CurveFloat NG_CurveFloat,
                                                  UCurveFloat* DefaultValue)
{
	if (NG_CurveFloat.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_CurveFloat.Value;
	}

	if (NG_CurveFloat.Generator)
	{
		return NG_CurveFloat.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

UCurveVector* UGBWNGFuncLib::GetNGValue_CurveVector(AActor* OwnerActor, FNG_CurveVector NG_CurveVector, UCurveVector* DefaultValue)
{
	if (NG_CurveVector.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_CurveVector.Value;
	}

	if (NG_CurveVector.Generator)
	{
		return NG_CurveVector.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

UAnimMontage* UGBWNGFuncLib::GetNGValue_Montage(AActor* OwnerActor, FNG_Montage NG_Montage, UAnimMontage* DefaultValue)
{
	if (NG_Montage.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_Montage.Value;
	}

	if (NG_Montage.Generator)
	{
		return NG_Montage.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

UAnimSequence* UGBWNGFuncLib::GetNGValue_AnimSequence(AActor* OwnerActor, FNG_AnimSequence NG_AnimSequence, UAnimSequence* DefaultValue)
{
	if (NG_AnimSequence.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_AnimSequence.Value;
	}

	if (NG_AnimSequence.Generator)
	{
		return NG_AnimSequence.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

UBlendSpace* UGBWNGFuncLib::GetNGValue_AnimBlendSpace(AActor* OwnerActor, FNG_AnimBlendSpace NG_AnimBlendSpace, UBlendSpace* DefaultValue)
{
	if (NG_AnimBlendSpace.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_AnimBlendSpace.Value;
	}

	if (NG_AnimBlendSpace.Generator)
	{
		return NG_AnimBlendSpace.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

UAnimBlueprint* UGBWNGFuncLib::GetNGValue_AnimBlueprint(AActor* OwnerActor, FNG_AnimBlueprint NG_AnimBlueprint, UAnimBlueprint* DefaultValue)
{
	if (NG_AnimBlueprint.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_AnimBlueprint.Value;
	}

	if (NG_AnimBlueprint.Generator)
	{
		return NG_AnimBlueprint.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

USkeletalMesh* UGBWNGFuncLib::GetNGValue_SkeletalMesh(AActor* OwnerActor, FNG_SkeletalMesh NG_SkeletalMesh, USkeletalMesh* DefaultValue)
{
	if (NG_SkeletalMesh.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_SkeletalMesh.Value;
	}

	if (NG_SkeletalMesh.Generator)
	{
		return NG_SkeletalMesh.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

UStaticMesh* UGBWNGFuncLib::GetNGValue_StaticMesh(AActor* OwnerActor, FNG_StaticMesh NG_StaticMesh, UStaticMesh* DefaultValue)
{
	if (NG_StaticMesh.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_StaticMesh.Value;
	}

	if (NG_StaticMesh.Generator)
	{
		return NG_StaticMesh.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

UNiagaraSystem* UGBWNGFuncLib::GetNGValue_Niagara(AActor* OwnerActor, FNG_Niagara NG_Niagara, UNiagaraSystem* DefaultValue)
{
	if (NG_Niagara.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_Niagara.Value;
	}

	if (NG_Niagara.Generator)
	{
		return NG_Niagara.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

USoundBase* UGBWNGFuncLib::GetNGValue_Sound(AActor* OwnerActor, FNG_Sound NG_Sound, USoundBase* DefaultValue)
{
	if (NG_Sound.GeneratorMode == EGBWNumericGeneratorMode::DirectlyValue)
	{
		return NG_Sound.Value;
	}

	if (NG_Sound.Generator)
	{
		return NG_Sound.Generator->GetValue(OwnerActor);
	}

	return DefaultValue;
}

bool UGBWNGFuncLib::GetNGOValue_Bool(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, bool DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::Bool)
	{
		return NGO->GetValue_Bool(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

float UGBWNGFuncLib::GetNGOValue_Float(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, float DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::Float)
	{
		return NGO->GetValue_Float(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

FVector UGBWNGFuncLib::GetNGOValue_Vector(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, FVector DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::Vector)
	{
		return NGO->GetValue_Vector(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

FRotator UGBWNGFuncLib::GetNGOValue_Rotator(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO,
	FRotator DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::Rotator)
	{
		return NGO->GetValue_Rotator(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

FTransform UGBWNGFuncLib::GetNGOValue_Transform(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, FTransform DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::Transform)
	{
		return NGO->GetValue_Transform(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

FGameplayTag UGBWNGFuncLib::GetNGOValue_GameplayTag(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, FGameplayTag DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::GameplayTag)
	{
		return NGO->GetValue_GameplayTag(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

FText UGBWNGFuncLib::GetNGOValue_Text(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, FText DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::Text)
	{
		return NGO->GetValue_Text(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

FString UGBWNGFuncLib::GetNGOValue_String(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, FString DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::String)
	{
		return NGO->GetValue_String(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

FName UGBWNGFuncLib::GetNGOValue_Name(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, FName DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::Name)
	{
		return NGO->GetValue_Name(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

UCurveFloat* UGBWNGFuncLib::GetNGOValue_CurveFloat(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO,
                                                   UCurveFloat* DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::CurveFloat)
	{
		return NGO->GetValue_CurveFloat(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

UCurveVector* UGBWNGFuncLib::GetNGOValue_CurveVector(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, UCurveVector* DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::CurveVector)
	{
		return NGO->GetValue_CurveVector(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

UAnimMontage* UGBWNGFuncLib::GetNGOValue_Montage(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, UAnimMontage* DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::Montage)
	{
		return NGO->GetValue_Montage(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

UAnimSequence* UGBWNGFuncLib::GetNGOValue_AnimSequence(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, UAnimSequence* DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::AnimSequence)
	{
		return NGO->GetValue_AnimSequence(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

UBlendSpace* UGBWNGFuncLib::GetNGOValue_AnimBlendSpace(AActor* OwnerActor,
                                                       UGBWNumericGenerator_Value_Base* NGO, UBlendSpace* DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::AnimBlendSpace)
	{
		return NGO->GetValue_AnimBlendSpace(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

UAnimBlueprint* UGBWNGFuncLib::GetNGOValue_AnimBlueprint(AActor* OwnerActor,
                                                         UGBWNumericGenerator_Value_Base* NGO, UAnimBlueprint* DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::AnimBlueprint)
	{
		return NGO->GetValue_AnimBlueprint(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

USkeletalMesh* UGBWNGFuncLib::GetNGOValue_SkeletalMesh(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, USkeletalMesh* DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::SkeletalMesh)
	{
		return NGO->GetValue_SkeletalMesh(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

UStaticMesh* UGBWNGFuncLib::GetNGOValue_StaticMesh(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, UStaticMesh* DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::StaticMesh)
	{
		return NGO->GetValue_StaticMesh(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

UNiagaraSystem* UGBWNGFuncLib::GetNGOValue_Niagara(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, UNiagaraSystem* DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::Niagara)
	{
		return NGO->GetValue_Niagara(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

USoundBase* UGBWNGFuncLib::GetNGOValue_Sound(AActor* OwnerActor, UGBWNumericGenerator_Value_Base* NGO, USoundBase* DefaultValue)
{
	if (NGO && NGO->GetValueType() == EGBWNumericGeneratorValueType::Sound)
	{
		return NGO->GetValue_Sound(OwnerActor,DefaultValue);
	}

	return DefaultValue;
}

bool UGBWNGFuncLib::GetDANGValue_Bool(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag,
	FGameplayTag ValueTag, bool DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_Bool(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

float UGBWNGFuncLib::GetDANGValue_Float(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag,
                                        FGameplayTag ValueTag, float DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_Float(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

FVector UGBWNGFuncLib::GetDANGValue_Vector(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag,
	FGameplayTag ValueTag, FVector DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_Vector(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

FRotator UGBWNGFuncLib::GetDANGValue_Rotator(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag,
	FGameplayTag ValueTag, FRotator DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_Rotator(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

FTransform UGBWNGFuncLib::GetDANGValue_Transform(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag,
                                                 FGameplayTag ValueTag, FTransform DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_Transform(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

FGameplayTag UGBWNGFuncLib::GetDANGValue_GameplayTag(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA,
	FGameplayTag GroupTag, FGameplayTag ValueTag, FGameplayTag DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_GameplayTag(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

FText UGBWNGFuncLib::GetDANGValue_Text(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag,
	FGameplayTag ValueTag, FText DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_Text(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

FString UGBWNGFuncLib::GetDANGValue_String(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag,
	FGameplayTag ValueTag, FString DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_String(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

FName UGBWNGFuncLib::GetDANGValue_Name(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag,
	FGameplayTag ValueTag, FName DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_Name(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

UCurveFloat* UGBWNGFuncLib::GetDANGValue_CurveFloat(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag,
                                                    FGameplayTag ValueTag, UCurveFloat* DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_CurveFloat(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

UCurveVector* UGBWNGFuncLib::GetDANGValue_CurveVector(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA,
                                                      FGameplayTag GroupTag, FGameplayTag ValueTag, UCurveVector* DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_CurveVector(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

UAnimMontage* UGBWNGFuncLib::GetDANGValue_Montage(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag,
	FGameplayTag ValueTag, UAnimMontage* DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_Montage(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

UAnimSequence* UGBWNGFuncLib::GetDANGValue_AnimSequence(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA,
	FGameplayTag GroupTag, FGameplayTag ValueTag, UAnimSequence* DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_AnimSequence(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

UBlendSpace* UGBWNGFuncLib::GetDANGValue_AnimBlendSpace(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA,
	FGameplayTag GroupTag, FGameplayTag ValueTag, UBlendSpace* DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_AnimBlendSpace(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

UAnimBlueprint* UGBWNGFuncLib::GetDANGValue_AnimBlueprint(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA,
	FGameplayTag GroupTag, FGameplayTag ValueTag, UAnimBlueprint* DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_AnimBlueprint(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

USkeletalMesh* UGBWNGFuncLib::GetDANGValue_SkeletalMesh(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA,
	FGameplayTag GroupTag, FGameplayTag ValueTag, USkeletalMesh* DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_SkeletalMesh(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

UStaticMesh* UGBWNGFuncLib::GetDANGValue_StaticMesh(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag,
	FGameplayTag ValueTag, UStaticMesh* DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_StaticMesh(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

UNiagaraSystem* UGBWNGFuncLib::GetDANGValue_Niagara(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag,
	FGameplayTag ValueTag, UNiagaraSystem* DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_Niagara(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

USoundBase* UGBWNGFuncLib::GetDANGValue_Sound(AActor* OwnerActor, UGBWNGDataAsset* NGSetDA, FGameplayTag GroupTag,
	FGameplayTag ValueTag, USoundBase* DefaultValue)
{
	if (NGSetDA)
	{
		return NGSetDA->GetValue_Sound(OwnerActor,GroupTag,ValueTag,DefaultValue);
	}
	return DefaultValue;
}

bool UGBWNGFuncLib::GetNGV_Bool(AActor* OwnerActor, FNGV_Bool NGV)
{
	return NGV.GetValue(OwnerActor);
}

float UGBWNGFuncLib::GetNGV_Float(AActor* OwnerActor, FNGV_Float NGV)
{
	return NGV.GetValue(OwnerActor);
}

FVector UGBWNGFuncLib::GetNGV_Vector(AActor* OwnerActor, FNGV_Vector NGV)
{
	return NGV.GetValue(OwnerActor);
}

FRotator UGBWNGFuncLib::GetNGV_Rotator(AActor* OwnerActor, FNGV_Rotator NGV)
{
	return NGV.GetValue(OwnerActor);
}

FTransform UGBWNGFuncLib::GetNGV_Transform(AActor* OwnerActor, FNGV_Transform NGV)
{
	return NGV.GetValue(OwnerActor);
}

FGameplayTag UGBWNGFuncLib::GetNGV_GameplayTag(AActor* OwnerActor, FNGV_GameplayTag NGV)
{
	return NGV.GetValue(OwnerActor);
}

FText UGBWNGFuncLib::GetNGV_Text(AActor* OwnerActor, FNGV_Text NGV)
{
	return NGV.GetValue(OwnerActor);
}

FString UGBWNGFuncLib::GetNGV_String(AActor* OwnerActor, FNGV_String NGV)
{
	return NGV.GetValue(OwnerActor);
}

FName UGBWNGFuncLib::GetNGV_Name(AActor* OwnerActor, FNGV_Name NGV)
{
	return NGV.GetValue(OwnerActor);
}

UCurveFloat* UGBWNGFuncLib::GetNGV_CurveFloat(AActor* OwnerActor, FNGV_CurveFloat NGV)
{
	return NGV.GetValue(OwnerActor);
}

UCurveVector* UGBWNGFuncLib::GetNGV_CurveVector(AActor* OwnerActor, FNGV_CurveVector NGV)
{
	return NGV.GetValue(OwnerActor);
}

UAnimMontage* UGBWNGFuncLib::GetNGV_Montage(AActor* OwnerActor, FNGV_Montage NGV)
{
	return NGV.GetValue(OwnerActor);
}

UAnimSequence* UGBWNGFuncLib::GetNGV_AnimSequence(AActor* OwnerActor, FNGV_AnimSequence NGV)
{
	return NGV.GetValue(OwnerActor);
}

UBlendSpace* UGBWNGFuncLib::GetNGV_AnimBlendSpace(AActor* OwnerActor, FNGV_AnimBlendSpace NGV)
{
	return NGV.GetValue(OwnerActor);
}

UAnimBlueprint* UGBWNGFuncLib::GetNGV_AnimBlueprint(AActor* OwnerActor, FNGV_AnimBlueprint NGV)
{
	return NGV.GetValue(OwnerActor);
}

USkeletalMesh* UGBWNGFuncLib::GetNGV_SkeletalMesh(AActor* OwnerActor, FNGV_SkeletalMesh NGV)
{
	return NGV.GetValue(OwnerActor);
}

UStaticMesh* UGBWNGFuncLib::GetNGV_StaticMesh(AActor* OwnerActor, FNGV_StaticMesh NGV)
{
	return NGV.GetValue(OwnerActor);
}

UNiagaraSystem* UGBWNGFuncLib::GetNGV_Niagara(AActor* OwnerActor, FNGV_Niagara NGV)
{
	return NGV.GetValue(OwnerActor);
}

USoundBase* UGBWNGFuncLib::GetNGV_Sound(AActor* OwnerActor, FNGV_Sound NGV)
{
	return NGV.GetValue(OwnerActor);
}