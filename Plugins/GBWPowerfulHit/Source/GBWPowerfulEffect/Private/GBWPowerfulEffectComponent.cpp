// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "GBWPowerfulEffectComponent.h"
#include "GBWEffectActorBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UGBWPowerfulEffectComponent::UGBWPowerfulEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UGBWPowerfulEffectComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Parameters;
	Parameters.bIsPushBased = true;
	Parameters.Condition = COND_None;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Effects, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsEffectCompActive, Parameters)
}

// Called when the game starts
void UGBWPowerfulEffectComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UGBWPowerfulEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGBWPowerfulEffectComponent::SetEffectCompActive(bool bActive)
{
	SetEffectCompActive_Server(bActive);
}
void UGBWPowerfulEffectComponent::SetEffectCompActive_Server_Implementation(bool bActive)
{
	SetEffectCompActive_Imp(bActive);
}
void UGBWPowerfulEffectComponent::SetEffectCompActive_Imp(bool bActive)
{
	bIsEffectCompActive = bActive;
	if (!bIsEffectCompActive)
	{
		RemoveEffectByClass_Imp(AGBWEffectActorBase::StaticClass());
	}
}

void UGBWPowerfulEffectComponent::ExecuteEffect_WithStructParamStr(
	FGuid& EffectGuid,
	TSubclassOf<AGBWEffectActorBase> EffectClass,
	const FGBWSpawnRules& EffectSpawnSettings,
	const FString& EffectParamsStr,
	bool bIsReplicated,
	bool bOnlyTriggerFromServer)
{
	if (!bIsEffectCompActive || !EffectClass)
	{
		return;
	}

	if (bIsReplicated)
	{
		if ((bOnlyTriggerFromServer && !GetOwner()->HasAuthority())
			|| (!bOnlyTriggerFromServer && GetOwner()->GetLocalRole() < ROLE_AutonomousProxy))
		{
			return;
		}
		EffectGuid = FGuid::NewGuid();
		ExecuteEffect_Server(EffectGuid,EffectClass,EffectSpawnSettings,EffectParamsStr,bIsReplicated);
	}
	else
	{
		EffectGuid = FGuid::NewGuid();
		ExecuteEffect_Imp(EffectGuid,EffectClass,EffectSpawnSettings,EffectParamsStr,bIsReplicated);
	}
}

void UGBWPowerfulEffectComponent::ExecuteEffect(
	FGuid& EffectGuid,
	TSubclassOf<AGBWEffectActorBase> EffectClass,
	FGBWSpawnRules EffectSpawnSettings,
	bool bIsReplicated,
	bool bOnlyTriggerFromServer)
{
	if (!bIsEffectCompActive || !EffectClass)
	{
		return;
	}

	if (bIsReplicated)
	{
		if ((bOnlyTriggerFromServer && !GetOwner()->HasAuthority())
			|| (!bOnlyTriggerFromServer && GetOwner()->GetLocalRole() < ROLE_AutonomousProxy))
		{
			return;
		}
		EffectGuid = FGuid::NewGuid();
		ExecuteEffect_Server(EffectGuid,EffectClass,EffectSpawnSettings,"",bIsReplicated);
	}
	else
	{
		EffectGuid = FGuid::NewGuid();
		ExecuteEffect_Imp(EffectGuid,EffectClass,EffectSpawnSettings,"",bIsReplicated);
	}
}
void UGBWPowerfulEffectComponent::ExecuteEffect_Server_Implementation(
	const FGuid& EffectGuid,
	TSubclassOf<AGBWEffectActorBase> EffectClass,
	FGBWSpawnRules EffectSpawnSettings,
	const FString& EffectParamsStr,
	bool bIsReplicated)
{
	ExecuteEffect_Imp(EffectGuid,EffectClass,EffectSpawnSettings,EffectParamsStr,bIsReplicated);
}
void UGBWPowerfulEffectComponent::ExecuteEffect_Imp(
	const FGuid& EffectGuid,
	TSubclassOf<AGBWEffectActorBase> EffectClass,
	const FGBWSpawnRules& EffectSpawnSettings,
	FString EffectParamsStr,
	bool bIsReplicated)
{
	if (!EffectClass)
	{
		return;
	}

	bool bServerHas = false;
	bool bLocalHas = false;
	switch (EffectSpawnSettings.RepelType)
	{
	case None: break;
	case RemoveExistingSameClass:
		RemoveEffectByClass(EffectClass);
		break;
	case BlockedByExistingSameClass:
		HasSpecifiedClassEffect(EffectClass,bServerHas,bLocalHas);
		if (bServerHas || bLocalHas)
		{
			return;
		}
		break;
	case RemoveExistingSpecifiedClass:
		RemoveEffectByClassSet(EffectSpawnSettings.SpecifiedClassSet);
		break;
	case BlockedByExistingSpecifiedClass:
		HasSpecifiedClassesEffect(EffectSpawnSettings.SpecifiedClassSet,bServerHas,bLocalHas);
		if (bServerHas || bLocalHas)
		{
			return;
		}
		break;
	default: ;
	}
	
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.Owner = this->GetOwner();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (AGBWEffectActorBase* EffectActor =
		Cast<AGBWEffectActorBase>(
			this->GetWorld()->SpawnActorAbsolute(
				EffectClass,
				EffectSpawnSettings.bUseOwnerTransform? this->GetOwner()->GetTransform() : EffectSpawnSettings.Transform,
				SpawnParameters)
			)
		)
	{
		EffectActor->EffectGuid = EffectGuid;
		EffectActor->OwnerActorNetType = UGBWPowerfulToolsFuncLib::GetActorNetType(this->GetOwner());
		EffectActor->EffectParamsStr = EffectParamsStr;
		EffectActor->OwnerActor = this->GetOwner();

		if (EffectSpawnSettings.bAttachToOwner)
		{
			const FAttachmentTransformRules AttachmentTransformRules =
				FAttachmentTransformRules(EffectSpawnSettings.LocationAttachmentRule,
					EffectSpawnSettings.RotationAttachmentRule,
					EffectSpawnSettings.ScaleAttachmentRule,
					false);
			EffectActor->AttachToActor(this->GetOwner(),AttachmentTransformRules);
		}

		EffectActor->EffectStart(EffectActor->OwnerActorNetType);
		AddEffect(EffectActor,bIsReplicated);
	}
}

void UGBWPowerfulEffectComponent::AddEffect(AGBWEffectActorBase* NewEffect, bool bIsReplicated)
{
	if (NewEffect)
	{
		if (bIsReplicated)
		{
			Effects.AddUnique(NewEffect);
		}
		else
		{
			Effects_Local.AddUnique(NewEffect);
		}
	}
}

void UGBWPowerfulEffectComponent::RemoveEffect(FGuid EffectGuid)
{
	for (auto E : Effects)
	{
		if (E && E->EffectGuid==EffectGuid)
		{
			if (this->GetOwner()->HasAuthority())
			{
				RemoveEffect_Server(EffectGuid);
			}
			return;
		}
	}
	for (auto E : Effects_Local)
	{
		if (E && E->EffectGuid==EffectGuid)
		{
			RemoveEffect_Imp(EffectGuid);
			return;
		}
	}
}
void UGBWPowerfulEffectComponent::RemoveEffect_Server_Implementation(const FGuid& EffectGuid)
{
	RemoveEffect_Imp(EffectGuid);
}
void UGBWPowerfulEffectComponent::RemoveEffect_Imp(const FGuid& EffectGuid)
{
	for (auto E : Effects)
	{
		if (E && E->EffectGuid==EffectGuid)
		{
			Effects.Remove(E);
			E->DestroyEffect();
			return;
		}
	}
	for (auto E : Effects_Local)
	{
		if (E && E->EffectGuid==EffectGuid)
		{
			Effects_Local.Remove(E);
			E->DestroyEffect();
			return;
		}
	}
}

void UGBWPowerfulEffectComponent::HasSpecifiedClassEffect(TSubclassOf<AGBWEffectActorBase> EffectClass,
	bool& bServerHas, bool& bLocalHas)
{
	bServerHas = false;
	bLocalHas = false;
	for (auto E : Effects)
	{
		if (E && E->GetClass() == EffectClass)
		{
			bServerHas = true;
			break;
		}
	}
	for (auto E : Effects_Local)
	{
		if (E && E->GetClass() == EffectClass)
		{
			bLocalHas = true;
			break;
		}
	}
}

void UGBWPowerfulEffectComponent::HasSpecifiedClassesEffect(TArray<TSubclassOf<AGBWEffectActorBase>> EffectClassSet,
	bool& bServerHas, bool& bLocalHas)
{
	bServerHas = false;
	bLocalHas = false;
	for (auto E : Effects)
	{
		if (E && EffectClassSet.Contains(E->GetClass()))
		{
			bServerHas = true;
			break;
		}
	}
	for (auto E : Effects_Local)
	{
		if (E && EffectClassSet.Contains(E->GetClass()))
		{
			bLocalHas = true;
			break;
		}
	}
}

void UGBWPowerfulEffectComponent::RemoveEffectByClass(TSubclassOf<AGBWEffectActorBase> EffectClass)
{
	bool bServerHas = false;
	bool bLocalHas = false;
	HasSpecifiedClassEffect(EffectClass,bServerHas,bLocalHas);
	if (bServerHas)
	{
		RemoveEffectByClass_Server(EffectClass);
	}
	if (bLocalHas)
	{
		RemoveEffectByClass_Imp(EffectClass);
	}
}
void UGBWPowerfulEffectComponent::RemoveEffectByClass_Server_Implementation(TSubclassOf<AGBWEffectActorBase> EffectClass)
{
	RemoveEffectByClass_Imp(EffectClass);
}
void UGBWPowerfulEffectComponent::RemoveEffectByClass_Imp(TSubclassOf<AGBWEffectActorBase> EffectClass)
{
	TArray<AGBWEffectActorBase*> EffectsArr = Effects;
	for (auto E : EffectsArr)
	{
		if (E && E->GetClass() == EffectClass)
		{
			Effects.Remove(E);
			E->DestroyEffect();
			return;
		}
	}
	EffectsArr = Effects_Local;
	for (auto E : EffectsArr)
	{
		if (E && E->GetClass() == EffectClass)
		{
			Effects_Local.Remove(E);
			E->DestroyEffect();
			return;
		}
	}
}

void UGBWPowerfulEffectComponent::RemoveEffectByClassSet(TArray<TSubclassOf<AGBWEffectActorBase>> EffectClassSet)
{
	bool bServerHas = false;
	bool bLocalHas = false;
	HasSpecifiedClassesEffect(EffectClassSet,bServerHas,bLocalHas);
	if (bServerHas)
	{
		RemoveEffectByClassSet_Server(EffectClassSet);
	}
	if (bLocalHas)
	{
		RemoveEffectByClassSet_Imp(EffectClassSet);
	}
}
void UGBWPowerfulEffectComponent::RemoveEffectByClassSet_Server_Implementation(
	const TArray<TSubclassOf<AGBWEffectActorBase>>& EffectClassSet)
{
	RemoveEffectByClassSet_Imp(EffectClassSet);
}
void UGBWPowerfulEffectComponent::RemoveEffectByClassSet_Imp(const TArray<TSubclassOf<AGBWEffectActorBase>>& EffectClassSet)
{
	TArray<AGBWEffectActorBase*> EffectsArr = Effects;
	for (auto E : EffectsArr)
	{
		if (E && EffectClassSet.Contains(E->GetClass()))
		{
			Effects.Remove(E);
			E->DestroyEffect();
			return;
		}
	}
	EffectsArr = Effects_Local;
	for (auto E : EffectsArr)
	{
		if (E && EffectClassSet.Contains(E->GetClass()))
		{
			Effects_Local.Remove(E);
			E->DestroyEffect();
			return;
		}
	}
}

bool UGBWPowerfulEffectComponent::GetPostureAdjustData(FGBWEffect_PostureAdjustData& OutPostureAdjustData) const
{
	OutPostureAdjustData = PostureAdjustData;
	return bIsPostureAdjust;
}
bool UGBWPowerfulEffectComponent::GetBodyShakeData(bool ClearData, FGBWEffect_BodyShakeData& OutBodyShakeData)
{
	OutBodyShakeData = BodyShakeData;
	if (ClearData)
	{
		DeactiveBodyShake();
	}
	return bIsBodyShake;
}
bool UGBWPowerfulEffectComponent::GetBodyTwistData(bool ClearData, FGBWEffect_BodyTwistData& OutBodyTwistData)
{
	OutBodyTwistData = BodyTwistData;
	if (ClearData)
	{
		DeactiveBodyTwist();
	}
	return bIsBodyTwist;
}

void UGBWPowerfulEffectComponent::ActivePostureAdjust(FGBWEffect_PostureAdjustData InPostureAdjustData)
{
	ActivePostureAdjust_Server(InPostureAdjustData);
}
void UGBWPowerfulEffectComponent::ActivePostureAdjust_Server_Implementation(
	const FGBWEffect_PostureAdjustData& InPostureAdjustData)
{
	ActivePostureAdjust_Imp(InPostureAdjustData);
}
void UGBWPowerfulEffectComponent::ActivePostureAdjust_Imp(const FGBWEffect_PostureAdjustData& InPostureAdjustData)
{
	bIsPostureAdjust = true;
	PostureAdjustData = InPostureAdjustData;
}

void UGBWPowerfulEffectComponent::UpdatePostureAdjustTargetPoint(FVector TargetPoint)
{
	UpdatePostureAdjustTargetPoint_Server(TargetPoint);
}
void UGBWPowerfulEffectComponent::UpdatePostureAdjustTargetPoint_Server_Implementation(FVector TargetPoint)
{
	UpdatePostureAdjustTargetPoint_Imp(TargetPoint);
}
void UGBWPowerfulEffectComponent::UpdatePostureAdjustTargetPoint_Imp(const FVector& TargetPoint)
{
	PostureAdjustData.TargetPoint = TargetPoint;
}

void UGBWPowerfulEffectComponent::DeactivePostureAdjust()
{
	DeactivePostureAdjust_Server();
}
void UGBWPowerfulEffectComponent::DeactivePostureAdjust_Server_Implementation()
{
	DeactivePostureAdjust_Imp();
}
void UGBWPowerfulEffectComponent::DeactivePostureAdjust_Imp()
{
	bIsPostureAdjust = false;
}

void UGBWPowerfulEffectComponent::ActiveBodyShake(FGBWEffect_BodyShakeData InBodyShakeData)
{
	ActiveBodyShake_Server(InBodyShakeData);
}
void UGBWPowerfulEffectComponent::ActiveBodyShake_Server_Implementation(const FGBWEffect_BodyShakeData& InBodyShakeData)
{
	ActiveBodyShake_Imp(InBodyShakeData);
}
void UGBWPowerfulEffectComponent::ActiveBodyShake_Imp(const FGBWEffect_BodyShakeData& InBodyShakeData)
{
	bIsBodyShake = true;
	BodyShakeData = InBodyShakeData;
}

void UGBWPowerfulEffectComponent::DeactiveBodyShake()
{
	DeactiveBodyShake_Server();
}
void UGBWPowerfulEffectComponent::DeactiveBodyShake_Server_Implementation()
{
	DeactiveBodyShake_Imp();
}
void UGBWPowerfulEffectComponent::DeactiveBodyShake_Imp()
{
	bIsBodyShake = false;
}

void UGBWPowerfulEffectComponent::ActiveBodyTwist(FGBWEffect_BodyTwistData InBodyTwistData)
{
	ActiveBodyTwist_Server(InBodyTwistData);
}
void UGBWPowerfulEffectComponent::ActiveBodyTwist_Server_Implementation(const FGBWEffect_BodyTwistData& InBodyTwistData)
{
	ActiveBodyTwist_Imp(InBodyTwistData);
}
void UGBWPowerfulEffectComponent::ActiveBodyTwist_Imp(const FGBWEffect_BodyTwistData& InBodyTwistData)
{
	bIsBodyTwist = true;
	BodyTwistData = InBodyTwistData;
}

void UGBWPowerfulEffectComponent::DeactiveBodyTwist()
{
	DeactiveBodyTwist_Server();
}
void UGBWPowerfulEffectComponent::DeactiveBodyTwist_Server_Implementation()
{
	DeactiveBodyTwist_Imp();
}
void UGBWPowerfulEffectComponent::DeactiveBodyTwist_Imp()
{
	bIsBodyTwist = false;
}