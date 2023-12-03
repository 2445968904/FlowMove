// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "GBWNGDataAsset.h"
#include "GBWNGFuncLib.h"

UGBWNumericGenerator_Value_Base* UGBWNGDataAsset::GetNGO(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag)
{
	for (auto NGItem:NGSet)
	{
		if (NGItem.Value.ValueGroups.FindRef(GroupTag).Values.Find(ValueTag))
		{
			return NGItem.Value.ValueGroups.FindRef(GroupTag).Values.FindRef(ValueTag);
		}
	}
	
	return nullptr;
}

bool UGBWNGDataAsset::GetValue_Bool(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
                                    bool DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_Bool(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

float UGBWNGDataAsset::GetValue_Float(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
                                      float DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_Float(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

FVector UGBWNGDataAsset::GetValue_Vector(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
	FVector DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_Vector(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

FRotator UGBWNGDataAsset::GetValue_Rotator(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
	FRotator DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_Rotator(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

FTransform UGBWNGDataAsset::GetValue_Transform(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
                                               FTransform DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_Transform(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

FGameplayTag UGBWNGDataAsset::GetValue_GameplayTag(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
	FGameplayTag DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_GameplayTag(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

FText UGBWNGDataAsset::GetValue_Text(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
	FText DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_Text(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

FString UGBWNGDataAsset::GetValue_String(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
	FString DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_String(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

FName UGBWNGDataAsset::GetValue_Name(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
	FName DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_Name(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

UCurveFloat* UGBWNGDataAsset::GetValue_CurveFloat(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
                                                  UCurveFloat* DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_CurveFloat(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

UCurveVector* UGBWNGDataAsset::GetValue_CurveVector(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
                                                    UCurveVector* DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_CurveVector(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

UAnimMontage* UGBWNGDataAsset::GetValue_Montage(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
	UAnimMontage* DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_Montage(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

UAnimSequence* UGBWNGDataAsset::GetValue_AnimSequence(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
	UAnimSequence* DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_AnimSequence(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

UBlendSpace* UGBWNGDataAsset::GetValue_AnimBlendSpace(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
	UBlendSpace* DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_AnimBlendSpace(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

UAnimBlueprint* UGBWNGDataAsset::GetValue_AnimBlueprint(AActor* OwnerActor, FGameplayTag GroupTag,
	FGameplayTag ValueTag, UAnimBlueprint* DefaultValue)
{	
	return UGBWNGFuncLib::GetNGOValue_AnimBlueprint(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

USkeletalMesh* UGBWNGDataAsset::GetValue_SkeletalMesh(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
	USkeletalMesh* DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_SkeletalMesh(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

UStaticMesh* UGBWNGDataAsset::GetValue_StaticMesh(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
	UStaticMesh* DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_StaticMesh(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

UNiagaraSystem* UGBWNGDataAsset::GetValue_Niagara(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
	UNiagaraSystem* DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_Niagara(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}

USoundBase* UGBWNGDataAsset::GetValue_Sound(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,
	USoundBase* DefaultValue)
{
	return UGBWNGFuncLib::GetNGOValue_Sound(OwnerActor,GetNGO(OwnerActor,GroupTag,ValueTag),DefaultValue);
}