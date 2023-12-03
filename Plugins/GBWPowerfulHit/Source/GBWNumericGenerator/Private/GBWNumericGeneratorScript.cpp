// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "GBWNumericGeneratorScript.h"
#include "GBWNGDataAsset.h"
#include "GBWNGFuncLib.h"

void UGBWNumericGeneratorNetWorkObject::SetWorldContext(UObject* NewWorldContext)
{
	WorldContext = new FWorldContext();
	WorldContext->SetCurrentWorld(NewWorldContext->GetWorld());
}

UObject* UGBWNumericGeneratorNetWorkObject::GetWorldContextObject()
{
	return WorldContext->World();
}

UWorld* UGBWNumericGeneratorNetWorkObject::GetWorld() const
{
	return WorldContext ? WorldContext->World() : NULL;
}

bool UGBWNG_Bool::GetValue_Bool_Implementation(AActor* OwnerActor, bool DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_Bool(OwnerActor,Value,DefaultValue);
}

float UGBWNG_Float::GetValue_Float_Implementation(AActor* OwnerActor, float DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_Float(OwnerActor,Value,DefaultValue);
}

FVector UGBWNG_Vector::GetValue_Vector_Implementation(AActor* OwnerActor, FVector DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_Vector(OwnerActor,Value,DefaultValue);
}

FRotator UGBWNG_Rotator::GetValue_Rotator_Implementation(AActor* OwnerActor, FRotator DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_Rotator(OwnerActor,Value,DefaultValue);
}

FTransform UGBWNG_Transform::GetValue_Transform_Implementation(AActor* OwnerActor, FTransform DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_Transform(OwnerActor,Value,DefaultValue);
}

FGameplayTag UGBWNG_GameplayTag::GetValue_GameplayTag_Implementation(AActor* OwnerActor, FGameplayTag DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_GameplayTag(OwnerActor,Value,DefaultValue);
}

FText UGBWNG_Text::GetValue_Text_Implementation(AActor* OwnerActor, const FText& DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_Text(OwnerActor,Value,DefaultValue);
}

FString UGBWNG_String::GetValue_String_Implementation(AActor* OwnerActor, const FString& DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_String(OwnerActor,Value,DefaultValue);
}

FName UGBWNG_Name::GetValue_Name_Implementation(AActor* OwnerActor, const FName& DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_Name(OwnerActor,Value,DefaultValue);
}

UCurveFloat* UGBWNG_CurveFloat::GetValue_CurveFloat_Implementation(AActor* OwnerActor, UCurveFloat* DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_CurveFloat(OwnerActor,Value,DefaultValue);
}

UCurveVector* UGBWNG_CurveVector::GetValue_CurveVector_Implementation(AActor* OwnerActor, UCurveVector* DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_CurveVector(OwnerActor,Value,DefaultValue);
}

UAnimMontage* UGBWNG_Montage::GetValue_Montage_Implementation(AActor* OwnerActor, UAnimMontage* DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_Montage(OwnerActor,Value,DefaultValue);
}

UAnimSequence* UGBWNG_AnimSequence::GetValue_AnimSequence_Implementation(AActor* OwnerActor, UAnimSequence* DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_AnimSequence(OwnerActor,Value,DefaultValue);
}

UBlendSpace* UGBWNG_AnimBlendSpace::GetValue_AnimBlendSpace_Implementation(AActor* OwnerActor, UBlendSpace* DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_AnimBlendSpace(OwnerActor,Value,DefaultValue);
}

UAnimBlueprint* UGBWNG_AnimBlueprint::GetValue_AnimBlueprint_Implementation(AActor* OwnerActor, UAnimBlueprint* DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_AnimBlueprint(OwnerActor,Value,DefaultValue);
}

USkeletalMesh* UGBWNG_SkeletalMesh::GetValue_SkeletalMesh_Implementation(AActor* OwnerActor, USkeletalMesh* DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_SkeletalMesh(OwnerActor,Value,DefaultValue);
}

UStaticMesh* UGBWNG_StaticMesh::GetValue_StaticMesh_Implementation(AActor* OwnerActor, UStaticMesh* DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_StaticMesh(OwnerActor,Value,DefaultValue);
}

UNiagaraSystem* UGBWNG_Niagara::GetValue_Niagara_Implementation(AActor* OwnerActor, UNiagaraSystem* DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_Niagara(OwnerActor,Value,DefaultValue);
}

USoundBase* UGBWNG_Sound::GetValue_Sound_Implementation(AActor* OwnerActor, USoundBase* DefaultValue)
{
	return UGBWNGFuncLib::GetNGValue_Sound(OwnerActor,Value,DefaultValue);
}