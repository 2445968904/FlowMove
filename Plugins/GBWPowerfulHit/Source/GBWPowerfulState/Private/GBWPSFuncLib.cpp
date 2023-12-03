// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "GBWPSFuncLib.h"
#include "Animation/AnimInstance.h"

FPS_Value UGBWPSFuncLib::MakePSFloat(float Value)
{
	FPS_Value Res;
	Res.SetFValue(Value);
	return Res;
}

FPS_Value UGBWPSFuncLib::MakePSVector(FVector Value)
{
	FPS_Value Res;
	Res.SetVValue(Value);
	return Res;	
}

FPS_Value UGBWPSFuncLib::MakePSTransform(FTransform Value)
{
	FPS_Value Res;
	Res.SetTValue(Value);
	return Res;
}

FPS_Value UGBWPSFuncLib::MakePSActor(AActor* Value)
{
	FPS_Value Res;
	Res.SetActorValue(Value);
	return Res;
}

FPS_Value UGBWPSFuncLib::MakePSText(FString Value)
{
	FPS_Value Res;
	Res.SetTextValue(Value);
	return Res;
}

FPS_Value UGBWPSFuncLib::MakePSGT(FGameplayTag Value)
{
	FPS_Value Res;
	Res.SetGTValue(Value);
	return Res;
}

FPS_Value UGBWPSFuncLib::MakePSBool(bool Value)
{
	FPS_Value Res;
	Res.SetBoolValue(Value);
	return Res;
}

float UGBWPSFuncLib::GetFValue(const FPS_Value PSValue)
{return PSValue.GetFValue();}

FVector UGBWPSFuncLib::GetVValue(const FPS_Value PSValue)
{return PSValue.GetVValue();}

FTransform UGBWPSFuncLib::GetTValue(const FPS_Value PSValue)
{return PSValue.GetTValue();}

AActor* UGBWPSFuncLib::GetActorValue(const FPS_Value PSValue)
{return PSValue.GetActorValue();}

FString UGBWPSFuncLib::GetTextValue(const FPS_Value PSValue)
{return PSValue.GetTextValue();}

FGameplayTag UGBWPSFuncLib::GetGTValue(const FPS_Value PSValue)
{return PSValue.GetGTValue();}

bool UGBWPSFuncLib::GetBoolValue(const FPS_Value PSValue)
{return PSValue.GetBoolValue();}

FString UGBWPSFuncLib::GetValueDebugString(FPS_Value PSValue)
{return PSValue.GetString();}

FPS_State UGBWPSFuncLib::MakePowerfulState()
{
	FPS_State Result;
	return Result;
}

FPS_State UGBWPSFuncLib::AddFloatToPS(FPS_State InState, FGameplayTag Key, float Value)
{
	FPS_State Res = InState;
	Res.SetState(Key, FPS_Value(Value));
	return Res;
}

FPS_State UGBWPSFuncLib::AddVectorToPS(FPS_State InState, FGameplayTag Key, FVector Value)
{
	FPS_State Res = InState;
	Res.SetState(Key, FPS_Value(Value));
	return Res;
}

FPS_State UGBWPSFuncLib::AddTransformToPS(FPS_State InState, FGameplayTag Key, FTransform Value)
{
	FPS_State Res = InState;
	Res.SetState(Key, FPS_Value(Value));
	return Res;
}

FPS_State UGBWPSFuncLib::AddActorToPS(FPS_State InState, FGameplayTag Key, AActor* Value)
{
	FPS_State Res = InState;
	Res.SetState(Key, FPS_Value(Value));
	return Res;
}

FPS_State UGBWPSFuncLib::AddTextToPS(FPS_State InState, FGameplayTag Key, FString Value)
{
	FPS_State Res = InState;
	Res.SetState(Key, FPS_Value(Value));
	return Res;
}

FPS_State UGBWPSFuncLib::AddGTToPS(FPS_State InState, FGameplayTag Key, FGameplayTag Value)
{
	FPS_State Res = InState;
	Res.SetState(Key, FPS_Value(Value));
	return Res;
}

FPS_State UGBWPSFuncLib::AddBoolToPS(FPS_State InState, FGameplayTag Key, bool Value)
{
	FPS_State Res = InState;
	Res.SetState(Key, FPS_Value(Value));
	return Res;
}

bool UGBWPSFuncLib::GetStateFValue(FPS_State InState, FGameplayTag Key, float& Result)
{
	float Time = -1.0f;
	return InState.GetFValue(Key, Result, Time);
}

bool UGBWPSFuncLib::GetStateVValue(FPS_State InState, FGameplayTag Key, FVector& Result)
{
	float Time = -1.0f;
	return InState.GetVValue(Key, Result, Time);
}

bool UGBWPSFuncLib::GetStateTValue(FPS_State InState, FGameplayTag Key, FTransform& Result)
{
	float Time = -1.0f;
	return InState.GetTValue(Key, Result, Time);
}

bool UGBWPSFuncLib::GetStateActorValue(FPS_State InState, FGameplayTag Key, AActor*& Result)
{
	float Time = -1.0f;
	return InState.GetActorValue(Key, Result, Time);
}

bool UGBWPSFuncLib::GetStateTextValue(FPS_State InState, FGameplayTag Key, FString& Result)
{
	float Time = -1.0f;
	return InState.GetTextValue(Key, Result, Time);
}

bool UGBWPSFuncLib::GetStateGTValue(FPS_State InState, FGameplayTag Key, FGameplayTag& Result)
{
	float Time = -1.0f;
	return InState.GetGTValue(Key, Result, Time);
}

bool UGBWPSFuncLib::GetStateBoolValue(FPS_State InState, FGameplayTag Key, bool& Result)
{
	float Time = -1.0f;
	return InState.GetBoolValue(Key, Result, Time);
}

UGBWPowerfulStateComponent* UGBWPSFuncLib::GetPowerfulStateComponent(AActor* InActor)
{
	if (!InActor)
	{
		return nullptr;
	}
	UGBWPowerfulStateComponent* PSComp = nullptr;
	if (UActorComponent* Component = InActor->GetComponentByClass(UGBWPowerfulStateComponent::StaticClass()))
	{
		PSComp = Cast<UGBWPowerfulStateComponent>(Component);
	}

	if (!PSComp && InActor->HasAuthority())
	{
		PSComp = Cast<UGBWPowerfulStateComponent>(
			InActor->AddComponentByClass(UGBWPowerfulStateComponent::StaticClass(),
				false,
				FTransform(),
				false));
		PSComp->SetIsReplicated(true);
	}

	return PSComp? PSComp : nullptr;
}

void UGBWPSFuncLib::SetPowerfulState(AActor* InActor, bool bOnlyTriggerInServer, FGameplayTag Key, FPS_Value Value)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		if (bOnlyTriggerInServer && !InActor->HasAuthority())
		{
			return;
		}
		PSC->SetState(Key,Value);
	}
}

void UGBWPSFuncLib::PS_SetPowerfulState(
	bool InCondition, bool ExecuteWhenNotMeetCondition, bool& OutCondition,
	const UAnimInstance* AnimInstance, FGameplayTag Key, FPS_Value Value)
{
	OutCondition = InCondition;
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		if ((!ExecuteWhenNotMeetCondition && InCondition)
			|| (ExecuteWhenNotMeetCondition && !InCondition))
		{
			UGBWPSFuncLib::SetPowerfulState(AnimInstance->GetOwningActor(), true, Key, Value);
		}
	}
}

void UGBWPSFuncLib::RemovePowerfulState(AActor* InActor, bool bOnlyTriggerInServer, FGameplayTag Key)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		if (bOnlyTriggerInServer && !InActor->HasAuthority())
		{
			return;
		}
		PSC->RemoveState(Key);
	}
}

void UGBWPSFuncLib::PS_RemovePowerfulState(bool InCondition, bool ExecuteWhenNotMeetCondition, bool& OutCondition,
	const UAnimInstance* AnimInstance, FGameplayTag Key)
{
	OutCondition = InCondition;
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		if ((!ExecuteWhenNotMeetCondition && InCondition)
			|| (ExecuteWhenNotMeetCondition && !InCondition))
		{
			UGBWPSFuncLib::RemovePowerfulState(AnimInstance->GetOwningActor(), true, Key);
		}
	}
}

void UGBWPSFuncLib::SetPSFloat(AActor* InActor, FGameplayTag Key, float Value)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		PSC->SetStateFloat(Key,Value);
	}
}

void UGBWPSFuncLib::SetPSVector(AActor* InActor, FGameplayTag Key, FVector Value)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		PSC->SetStateVector(Key,Value);
	}
}

void UGBWPSFuncLib::SetPSTransform(AActor* InActor, FGameplayTag Key, FTransform Value)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		PSC->SetStateTransform(Key,Value);
	}
}

void UGBWPSFuncLib::SetPSActor(AActor* InActor, FGameplayTag Key, AActor* Value)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		PSC->SetStateActor(Key,Value);
	}
}

void UGBWPSFuncLib::SetPSText(AActor* InActor, FGameplayTag Key, const FString Value)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		PSC->SetStateText(Key,Value);
	}
}

void UGBWPSFuncLib::SetPSGT(AActor* InActor, FGameplayTag Key, FGameplayTag Value)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		PSC->SetStateGT(Key,Value);
	}
}

void UGBWPSFuncLib::SetPSBool(AActor* InActor, FGameplayTag Key, bool Value)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		PSC->SetStateBool(Key,Value);
	}
}

void UGBWPSFuncLib::GetPSFValue(AActor* InActor, FGameplayTag Key, bool& bIsGet, float& Result, float& Duration)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		bIsGet = PSC->GetFValue(Key,Result,Duration);
		return;
	}

	bIsGet = false;
}

void UGBWPSFuncLib::PS_GetPSFValue(const UAnimInstance* AnimInstance, FGameplayTag Key, bool& bIsGet,
	float& Result, float& Duration)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		UGBWPSFuncLib::GetPSFValue(
			AnimInstance->GetOwningActor(),Key,bIsGet,Result,Duration);
	}
}

void UGBWPSFuncLib::GetPSVValue(AActor* InActor, FGameplayTag Key, bool& bIsGet, FVector& Result, float& Duration)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		bIsGet = PSC->GetVValue(Key,Result,Duration);
		return;
	}

	bIsGet = false;
}

void UGBWPSFuncLib::PS_GetPSVValue(const UAnimInstance* AnimInstance, FGameplayTag Key, bool& bIsGet,
	FVector& Result, float& Duration)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		UGBWPSFuncLib::GetPSVValue(
			AnimInstance->GetOwningActor(),Key,bIsGet,Result,Duration);
	}
}

void UGBWPSFuncLib::GetPSTValue(AActor* InActor, FGameplayTag Key, bool& bIsGet, FTransform& Result, float& Duration)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		bIsGet = PSC->GetTValue(Key,Result,Duration);
		return;
	}

	bIsGet = false;
}

void UGBWPSFuncLib::PS_GetPSTValue(const UAnimInstance* AnimInstance, FGameplayTag Key, bool& bIsGet,
	FTransform& Result, float& Duration)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		UGBWPSFuncLib::GetPSTValue(
			AnimInstance->GetOwningActor(),Key,bIsGet,Result,Duration);
	}
}

void UGBWPSFuncLib::GetPSActorValue(AActor* InActor, FGameplayTag Key, bool& bIsGet, AActor*& Result, float& Duration)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		bIsGet = PSC->GetActorValue(Key,Result,Duration);
		return;
	}

	bIsGet = false;
}

void UGBWPSFuncLib::PS_GetPSActorValue(const UAnimInstance* AnimInstance, FGameplayTag Key, bool& bIsGet,
	AActor*& Result, float& Duration)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		UGBWPSFuncLib::GetPSActorValue(
			AnimInstance->GetOwningActor(),Key,bIsGet,Result,Duration);
	}
}

void UGBWPSFuncLib::GetPSTextValue(AActor* InActor, FGameplayTag Key, bool& bIsGet, FString& Result, float& Duration)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		bIsGet = PSC->GetTextValue(Key,Result,Duration);
		return;
	}

	bIsGet = false;
}

void UGBWPSFuncLib::PS_GetPSTextValue(const UAnimInstance* AnimInstance, FGameplayTag Key, bool& bIsGet,
	FString& Result, float& Duration)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		UGBWPSFuncLib::GetPSTextValue(
			AnimInstance->GetOwningActor(),Key,bIsGet,Result,Duration);
	}
}

void UGBWPSFuncLib::GetPSGTValue(AActor* InActor, FGameplayTag Key, bool& bIsGet, FGameplayTag& Result, float& Duration)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		bIsGet = PSC->GetGTValue(Key,Result,Duration);
		return;
	}

	bIsGet = false;
}

void UGBWPSFuncLib::PS_GetPSGTValue(const UAnimInstance* AnimInstance, FGameplayTag Key, bool& bIsGet,
	FGameplayTag& Result, float& Duration)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		UGBWPSFuncLib::GetPSGTValue(
			AnimInstance->GetOwningActor(),Key,bIsGet,Result,Duration);
	}
}

void UGBWPSFuncLib::GetPSBoolValue(AActor* InActor, FGameplayTag Key, bool& bIsGet, bool& Result, float& Duration)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		bIsGet = PSC->GetBoolValue(Key,Result,Duration);
		return;
	}

	bIsGet = false;
}

void UGBWPSFuncLib::PS_GetPSBoolValue(const UAnimInstance* AnimInstance, FGameplayTag Key, bool& bIsGet,
	bool& Result, float& Duration)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		UGBWPSFuncLib::GetPSBoolValue(
			AnimInstance->GetOwningActor(),Key,bIsGet,Result,Duration);
	}
}

void UGBWPSFuncLib::GetPSFValue_History(AActor* InActor, FGameplayTag Key, int DistanceFromNow, bool& bIsGet,
                                        float& Result, float& Duration)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		bIsGet = PSC->GetFValue_History(Key,DistanceFromNow,Result,Duration);
		return;
	}

	bIsGet = false;
}

void UGBWPSFuncLib::PS_GetPSFValue_History(const UAnimInstance* AnimInstance, FGameplayTag Key,
	int DistanceFromNow, bool& bIsGet, float& Result, float& Duration)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		UGBWPSFuncLib::GetPSFValue_History(
			AnimInstance->GetOwningActor(),Key,DistanceFromNow,bIsGet,Result,Duration);
	}
}

void UGBWPSFuncLib::GetPSVValue_History(AActor* InActor, FGameplayTag Key, int DistanceFromNow, bool& bIsGet,
                                        FVector& Result, float& Duration)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		bIsGet = PSC->GetVValue_History(Key,DistanceFromNow,Result,Duration);
		return;
	}

	bIsGet = false;
}

void UGBWPSFuncLib::PS_GetPSVValue_History(const UAnimInstance* AnimInstance, FGameplayTag Key,
	int DistanceFromNow, bool& bIsGet, FVector& Result, float& Duration)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		UGBWPSFuncLib::GetPSVValue_History(
			AnimInstance->GetOwningActor(),Key,DistanceFromNow,bIsGet,Result,Duration);
	}
}

void UGBWPSFuncLib::GetPSTValue_History(AActor* InActor, FGameplayTag Key, int DistanceFromNow, bool& bIsGet,
                                        FTransform& Result, float& Duration)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		bIsGet = PSC->GetTValue_History(Key,DistanceFromNow,Result,Duration);
		return;
	}

	bIsGet = false;
}

void UGBWPSFuncLib::PS_GetPSTValue_History(const UAnimInstance* AnimInstance, FGameplayTag Key,
	int DistanceFromNow, bool& bIsGet, FTransform& Result, float& Duration)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		UGBWPSFuncLib::GetPSTValue_History(
			AnimInstance->GetOwningActor(),Key,DistanceFromNow,bIsGet,Result,Duration);
	}
}

void UGBWPSFuncLib::GetPSActorValue_History(AActor* InActor, FGameplayTag Key, int DistanceFromNow, bool& bIsGet,
                                            AActor*& Result, float& Duration)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		bIsGet = PSC->GetActorValue_History(Key,DistanceFromNow,Result,Duration);
		return;
	}

	bIsGet = false;
}

void UGBWPSFuncLib::PS_GetPSActorValue_History(const UAnimInstance* AnimInstance, FGameplayTag Key, int DistanceFromNow, bool& bIsGet, AActor*& Result, float& Duration)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		UGBWPSFuncLib::GetPSActorValue_History(
			AnimInstance->GetOwningActor(),Key,DistanceFromNow,bIsGet,Result,Duration);
	}
}

void UGBWPSFuncLib::GetPSTextValue_History(AActor* InActor, FGameplayTag Key, int DistanceFromNow, bool& bIsGet,
                                           FString& Result, float& Duration)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		bIsGet = PSC->GetTextValue_History(Key,DistanceFromNow,Result,Duration);
		return;
	}

	bIsGet = false;
}

void UGBWPSFuncLib::PS_GetPSTextValue_History(const UAnimInstance* AnimInstance, FGameplayTag Key,
	int DistanceFromNow, bool& bIsGet, FString& Result, float& Duration)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		UGBWPSFuncLib::GetPSTextValue_History(
			AnimInstance->GetOwningActor(),Key,DistanceFromNow,bIsGet,Result,Duration);
	}
}

void UGBWPSFuncLib::GetPSGTValue_History(AActor* InActor, FGameplayTag Key, int DistanceFromNow, bool& bIsGet,
                                         FGameplayTag& Result, float& Duration)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		bIsGet = PSC->GetGTValue_History(Key,DistanceFromNow,Result,Duration);
		return;
	}

	bIsGet = false;
}

void UGBWPSFuncLib::PS_GetPSGTValue_History(const UAnimInstance* AnimInstance, FGameplayTag Key,
	int DistanceFromNow, bool& bIsGet, FGameplayTag& Result, float& Duration)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		UGBWPSFuncLib::GetPSGTValue_History(
			AnimInstance->GetOwningActor(),Key,DistanceFromNow,bIsGet,Result,Duration);
	}
}

void UGBWPSFuncLib::GetPSBoolValue_History(AActor* InActor, FGameplayTag Key, int DistanceFromNow, bool& bIsGet,
                                           bool& Result, float& Duration)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		bIsGet = PSC->GetBoolValue_History(Key,DistanceFromNow,Result,Duration);
		return;
	}

	bIsGet = false;
}

void UGBWPSFuncLib::PS_GetPSBoolValue_History(const UAnimInstance* AnimInstance, FGameplayTag Key,
	int DistanceFromNow, bool& bIsGet, bool& Result, float& Duration)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		UGBWPSFuncLib::GetPSBoolValue_History(
			AnimInstance->GetOwningActor(),Key,DistanceFromNow,bIsGet,Result,Duration);
	}
}

void UGBWPSFuncLib::ActivePSDebug(AActor* InActor, FColor InDebugTextColor, bool InPrintToScreen, bool InPrintToLog)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		return PSC->ActivePSDebug(InDebugTextColor,InPrintToScreen,InPrintToLog);
	}
}

void UGBWPSFuncLib::DeactivePSDebug(AActor* InActor)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		return PSC->DeactivePSDebug();
	}
}

bool UGBWPSFuncLib::GetEventFValue(FPE_Event InEvent, FGameplayTag Key, float& Result)
{
	float Time = -1.0f;
	return InEvent.EventState.GetFValue(Key,Result, Time);
}

bool UGBWPSFuncLib::GetEventVValue(FPE_Event InEvent, FGameplayTag Key, FVector& Result)
{
	float Time = -1.0f;
	return InEvent.EventState.GetVValue(Key,Result, Time);
}

bool UGBWPSFuncLib::GetEventTValue(FPE_Event InEvent, FGameplayTag Key, FTransform& Result)
{
	float Time = -1.0f;
	return InEvent.EventState.GetTValue(Key,Result, Time);
}

bool UGBWPSFuncLib::GetEventActorValue(FPE_Event InEvent, FGameplayTag Key, AActor*& Result)
{
	float Time = -1.0f;
	return InEvent.EventState.GetActorValue(Key,Result, Time);
}

bool UGBWPSFuncLib::GetEventTextValue(FPE_Event InEvent, FGameplayTag Key, FString& Result)
{
	float Time = -1.0f;
	return InEvent.EventState.GetTextValue(Key,Result, Time);
}

bool UGBWPSFuncLib::GetEventGTValue(FPE_Event InEvent, FGameplayTag Key, FGameplayTag& Result)
{
	float Time = -1.0f;
	return InEvent.EventState.GetGTValue(Key,Result, Time);
}

bool UGBWPSFuncLib::GetEventBoolValue(FPE_Event InEvent, FGameplayTag Key, bool& Result)
{
	float Time = -1.0f;
	return InEvent.EventState.GetBoolValue(Key,Result, Time);
}

FPE_Event UGBWPSFuncLib::MakePE_Event(FGameplayTag EventKey)
{
	FPE_Event Result;
	Result.Key = EventKey;
	return Result;
}

FPE_Event UGBWPSFuncLib::AddStateToPE(FPE_Event InEvent, FPS_State EventState)
{
	FPE_Event Res = InEvent;
	for(auto S:EventState.State)
	{
		Res.EventState.SetState(S.Key, S.Value);
	}
	return Res;
}

FPE_Event UGBWPSFuncLib::AddFloatToPE(FPE_Event InEvent, FGameplayTag Key, float Value)
{
	FPE_Event Res = InEvent;
	Res.EventState.SetState(Key, FPS_Value(Value));
	return Res;
}

FPE_Event UGBWPSFuncLib::AddVectorToPE(FPE_Event InEvent, FGameplayTag Key, FVector Value)
{
	FPE_Event Res = InEvent;
	Res.EventState.SetState(Key, FPS_Value(Value));
	return Res;
}

FPE_Event UGBWPSFuncLib::AddTransformToPE(FPE_Event InEvent, FGameplayTag Key, FTransform Value)
{
	FPE_Event Res = InEvent;
	Res.EventState.SetState(Key, FPS_Value(Value));
	return Res;
}

FPE_Event UGBWPSFuncLib::AddActorToPE(FPE_Event InEvent, FGameplayTag Key, AActor* Value)
{
	FPE_Event Res = InEvent;
	Res.EventState.SetState(Key, FPS_Value(Value));
	return Res;
}

FPE_Event UGBWPSFuncLib::AddTextToPE(FPE_Event InEvent, FGameplayTag Key, FString Value)
{
	FPE_Event Res = InEvent;
	Res.EventState.SetState(Key, FPS_Value(Value));
	return Res;
}

FPE_Event UGBWPSFuncLib::AddGTToPE(FPE_Event InEvent, FGameplayTag Key, FGameplayTag Value)
{
	FPE_Event Res = InEvent;
	Res.EventState.SetState(Key, FPS_Value(Value));
	return Res;
}

FPE_Event UGBWPSFuncLib::AddBoolToPE(FPE_Event InEvent, FGameplayTag Key, bool Value)
{
	FPE_Event Res = InEvent;
	Res.EventState.SetState(Key, FPS_Value(Value));
	return Res;
}

void UGBWPSFuncLib::SendPowerfulEvent(AActor* InActor, FPE_Event PE_Event)
{
	if (UGBWPowerfulStateComponent* PSC = GetPowerfulStateComponent(InActor))
	{
		PSC->SendEvent(PE_Event);
	}
}

FString UGBWPSFuncLib::GetPSEventDebugString(FPS_Event InEvent)
{
	return InEvent.GetString();
}

FString UGBWPSFuncLib::GetPEEventDebugString(FPE_Event InEvent)
{
	return InEvent.GetString();
}
