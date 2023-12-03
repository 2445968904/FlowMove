// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "GBWEffectActorBase.h"

#include "GBWPowerfulEffectComponent.h"
#include "GBWPowerfulEffectFuncLib.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AGBWEffectActorBase::AGBWEffectActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AGBWEffectActorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Parameters;
	Parameters.bIsPushBased = true;
	Parameters.Condition = COND_None;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, EffectGuid, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, OwnerActorNetType, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, OwnerActor, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, EffectParamsStr, Parameters)
}

// Called when the game starts or when spawned
void AGBWEffectActorBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGBWEffectActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	EffectStart(OwnerActorNetType);
	EffectUpdate(OwnerActorNetType,DeltaTime);
}

void AGBWEffectActorBase::EndEffect()
{
	if (UGBWPowerfulEffectComponent* PEComp = UGBWPowerfulEffectFuncLib::GetGBWEffectComponent(OwnerActor, false))
	{
		PEComp->RemoveEffect(EffectGuid);
	}
	else
	{
		DestroyEffect();
	}
}

AActor* AGBWEffectActorBase::GetOwnerActor() const
{
	return OwnerActor;
}

void AGBWEffectActorBase::DestroyEffect()
{
	EffectEnd(OwnerActorNetType);
	this->Destroy();
}

void AGBWEffectActorBase::EffectEnd(EGBWActorNetType OwnerNetType)
{
	EffectEnd_Server(OwnerNetType);
}
void AGBWEffectActorBase::EffectEnd_Server_Implementation(EGBWActorNetType OwnerNetType)
{
	EffectEnd_Multicast(OwnerNetType);
}
void AGBWEffectActorBase::EffectEnd_Multicast_Implementation(EGBWActorNetType OwnerNetType)
{
	EffectEnd_Imp(OwnerNetType);
}
void AGBWEffectActorBase::EffectEnd_Imp(EGBWActorNetType OwnerNetType)
{
	OnEffectEnd(OwnerNetType);
}

void AGBWEffectActorBase::execInitEffectParameters(UObject* Context, FFrame& Stack, void* const Z_Param__Result)
{
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentProperty = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* StructPtr = Stack.MostRecentPropertyAddress;
	P_FINISH;
	if (const AGBWEffectActorBase* EActor = Cast<AGBWEffectActorBase>(Stack.Object))
	{
		*static_cast<bool*>(Z_Param__Result) = UGBWPowerfulToolsFuncLib::TextToStruct_Imp(EActor->EffectParamsStr, StructPtr, Stack.MostRecentProperty);
	}
	else
	{
		*static_cast<bool*>(Z_Param__Result) = false;
	}
}
