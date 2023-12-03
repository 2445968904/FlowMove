// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "GBWPowerfulEffectFuncLib.h"

#include "GBWEffectActorBase.h"
#include "GBWPowerfulEffectComponent.h"

UGBWPowerfulEffectComponent* UGBWPowerfulEffectFuncLib::GetGBWEffectComponent(AActor* InActor, bool TryAddComponent)
{
	if (!InActor)
	{
		return nullptr;
	}
	UGBWPowerfulEffectComponent* AComp = nullptr;
	if (UActorComponent* Component = InActor->GetComponentByClass(UGBWPowerfulEffectComponent::StaticClass()))
	{
		AComp = Cast<UGBWPowerfulEffectComponent>(Component);
	}

	if (!AComp && InActor->HasAuthority() && TryAddComponent)
	{
		if (UActorComponent* Comp = InActor->AddComponentByClass(UGBWPowerfulEffectComponent::StaticClass(),
		                                                         false,
		                                                         FTransform(),
		                                                         false))
		{
			AComp = Cast<UGBWPowerfulEffectComponent>(Comp);
			AComp->SetIsReplicated(true);
		}
	}

	return AComp;
}

void UGBWPowerfulEffectFuncLib::SetEffectCompActive(AActor* InActor, bool bActive)
{
	if (UGBWPowerfulEffectComponent* PEComp = UGBWPowerfulEffectFuncLib::GetGBWEffectComponent(InActor, true))
	{
		PEComp->SetEffectCompActive(bActive);
	}
}

void UGBWPowerfulEffectFuncLib::ExecuteEffect(
	AActor* InActor,
	FGuid& EffectGuid,
	TSubclassOf<AGBWEffectActorBase> EffectClass,
	FGBWSpawnRules EffectSpawnSettings,
	bool bIsReplicated,
	bool bOnlyTriggerFromServer)
{
	if (UGBWPowerfulEffectComponent* PEComp = UGBWPowerfulEffectFuncLib::GetGBWEffectComponent(InActor))
	{
		PEComp->ExecuteEffect(EffectGuid,EffectClass,EffectSpawnSettings,bIsReplicated,bOnlyTriggerFromServer);
	}
}

void UGBWPowerfulEffectFuncLib::ActivePostureAdjust(AActor* InActor, FGBWEffect_PostureAdjustData InPostureAdjustData)
{
	if (UGBWPowerfulEffectComponent* PEComp = UGBWPowerfulEffectFuncLib::GetGBWEffectComponent(InActor))
	{
		PEComp->ActivePostureAdjust(InPostureAdjustData);
	}
}

void UGBWPowerfulEffectFuncLib::ActiveBodyShake(AActor* InActor, FGBWEffect_BodyShakeData InBodyShakeData)
{
	if (UGBWPowerfulEffectComponent* PEComp = UGBWPowerfulEffectFuncLib::GetGBWEffectComponent(InActor))
	{
		PEComp->ActiveBodyShake(InBodyShakeData);
	}
}

void UGBWPowerfulEffectFuncLib::ActiveBodyTwist(AActor* InActor, FGBWEffect_BodyTwistData InBodyTwistData)
{
	if (UGBWPowerfulEffectComponent* PEComp = UGBWPowerfulEffectFuncLib::GetGBWEffectComponent(InActor))
	{
		PEComp->ActiveBodyTwist(InBodyTwistData);
	}
}

void UGBWPowerfulEffectFuncLib::UpdatePostureAdjustTargetPoint(AActor* InActor, FVector TargetPoint)
{
	if (UGBWPowerfulEffectComponent* PEComp = UGBWPowerfulEffectFuncLib::GetGBWEffectComponent(InActor))
	{
		PEComp->UpdatePostureAdjustTargetPoint(TargetPoint);
	}
}

void UGBWPowerfulEffectFuncLib::DeactivePostureAdjust(AActor* InActor)
{
	if (UGBWPowerfulEffectComponent* PEComp = UGBWPowerfulEffectFuncLib::GetGBWEffectComponent(InActor))
	{
		PEComp->DeactivePostureAdjust();
	}
}

void UGBWPowerfulEffectFuncLib::DeactiveBodyShake(AActor* InActor)
{
	if (UGBWPowerfulEffectComponent* PEComp = UGBWPowerfulEffectFuncLib::GetGBWEffectComponent(InActor))
	{
		PEComp->DeactiveBodyShake();
	}
}

void UGBWPowerfulEffectFuncLib::DeactiveBodyTwist(AActor* InActor)
{
	if (UGBWPowerfulEffectComponent* PEComp = UGBWPowerfulEffectFuncLib::GetGBWEffectComponent(InActor))
	{
		PEComp->DeactiveBodyTwist();
	}
}

void UGBWPowerfulEffectFuncLib::execExecuteEffect_Pro(UObject* Context, FFrame& Stack, void* const Z_Param__Result)
{
	P_GET_OBJECT(AActor,Z_Param_InActor);
	P_GET_STRUCT_REF(FGuid,Z_Param_Out_EffectGuid);
	P_GET_OBJECT(UClass,Z_Param_EffectClass);
	P_GET_STRUCT(FGBWSpawnRules,Z_Param_EffectSpawnSettings);
	
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentProperty = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	const void* StructPtr = Stack.MostRecentPropertyAddress;
	FProperty* StructProperty = Stack.MostRecentProperty;
	
	P_GET_UBOOL(Z_Param_bIsReplicated);
	P_GET_UBOOL(Z_Param_bOnlyTriggerFromServer);
	
	P_FINISH;
	P_NATIVE_BEGIN;
	if (UGBWPowerfulEffectComponent* PEComp = UGBWPowerfulEffectFuncLib::GetGBWEffectComponent(Z_Param_InActor))
	{
		FString STStr = UGBWPowerfulToolsFuncLib::StructToText_Imp(StructPtr, StructProperty);
		PEComp->ExecuteEffect_WithStructParamStr(Z_Param_Out_EffectGuid,Z_Param_EffectClass,Z_Param_EffectSpawnSettings,STStr,Z_Param_bIsReplicated,Z_Param_bOnlyTriggerFromServer);
	}
	P_NATIVE_END;
}
