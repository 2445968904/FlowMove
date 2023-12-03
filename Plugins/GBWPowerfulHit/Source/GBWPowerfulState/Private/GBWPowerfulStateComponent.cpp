// Copyright GanBowen 2022-2023. All Rights Reserved.

#include "GBWPowerfulStateComponent.h"

#include <string>

#include "GBWPowerfulToolsFuncLib.h"
#include "GBWPSFuncLib.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/Launch/Resources/Version.h"

float FPS_Value::GetFValue() const
{
	if (ValueType == EPS_ValueType::Float)
	{
		return FValue;
	}
	return 0.0;
}

FVector FPS_Value::GetVValue() const
{
	if (ValueType == EPS_ValueType::Vector)
	{
		return VValue;
	}
	return FVector::ZeroVector;
}

FTransform FPS_Value::GetTValue() const
{
	if (ValueType == EPS_ValueType::Transform)
	{
		return TValue;
	}
	return FTransform();
}

AActor* FPS_Value::GetActorValue() const
{
	if (ValueType == EPS_ValueType::Actor)
	{
		return ActorValue;
	}
	return nullptr;
}

FString FPS_Value::GetTextValue() const
{
	if (ValueType == EPS_ValueType::Text)
	{
		return TextValue;
	}
	return "";
}

FGameplayTag FPS_Value::GetGTValue() const
{
	if (ValueType == EPS_ValueType::GameplayTag)
	{
		return GTValue;
	}
	return FGameplayTag::EmptyTag;
}

bool FPS_Value::GetBoolValue() const
{
	if (ValueType == EPS_ValueType::Bool)
	{
		return BoolValue;
	}
	return false;
}

bool FPS_Value::IsTypeMatch(const EPS_ValueType InValueType) const
{
	return ValueType == InValueType;
}

void FPS_Value::SetFValue(const float NewValue)
{FValue = NewValue;ValueType = EPS_ValueType::Float;}

void FPS_Value::SetVValue(const FVector& NewValue)
{VValue = NewValue;ValueType = EPS_ValueType::Vector;}

void FPS_Value::SetTValue(const FTransform& NewValue)
{TValue = NewValue;ValueType = EPS_ValueType::Transform;}

void FPS_Value::SetActorValue(AActor* NewValue)
{ActorValue = NewValue;ValueType = EPS_ValueType::Actor;}

void FPS_Value::SetTextValue(FString NewValue)
{TextValue = NewValue;ValueType = EPS_ValueType::Text;}

void FPS_Value::SetGTValue(const FGameplayTag NewValue)
{GTValue = NewValue;ValueType = EPS_ValueType::GameplayTag;}

void FPS_Value::SetBoolValue(const bool NewValue)
{BoolValue = NewValue;ValueType = EPS_ValueType::Bool;}

FPS_Value::FPS_Value(const float NewValue)
{
	ValueType = EPS_ValueType::Float;
	FValue = NewValue;
}

FPS_Value::FPS_Value(const FVector& NewValue)
{
	ValueType = EPS_ValueType::Vector;
	VValue = NewValue;
}

FPS_Value::FPS_Value(const FTransform& NewValue)
{
	ValueType = EPS_ValueType::Transform;
	TValue = NewValue;
}

FPS_Value::FPS_Value(AActor* NewValue)
{
	ValueType = EPS_ValueType::Actor;
	ActorValue = NewValue;
}

FPS_Value::FPS_Value(FString NewValue)
{
	ValueType = EPS_ValueType::Text;
	TextValue = NewValue;
}

FPS_Value::FPS_Value(const FGameplayTag NewValue)
{
	ValueType = EPS_ValueType::GameplayTag;
	GTValue = NewValue;
}

FPS_Value::FPS_Value(bool NewValue)
{
	ValueType = EPS_ValueType::Bool;
	BoolValue = NewValue;
}

bool FPS_Value::EqualTo(const FPS_Value& Other) const
{
	if (ValueType == Other.ValueType)
	{
		switch (ValueType) { case EPS_ValueType::Float: return FValue==Other.FValue;
		case EPS_ValueType::Vector: return VValue==Other.VValue;
		case EPS_ValueType::Transform: return TValue.Equals(Other.TValue);
		case EPS_ValueType::Actor: return ActorValue==Other.ActorValue;
		case EPS_ValueType::Text: return TextValue==Other.TextValue;
		case EPS_ValueType::GameplayTag: return GTValue==Other.GTValue;
		case EPS_ValueType::Bool: return BoolValue==Other.BoolValue;
		default: ;
		}
	}
	return false;
}

FString FPS_Value::GetString() const
{
	switch (ValueType)
	{
	case EPS_ValueType::Float: return "<Float> "+FString::Printf(TEXT("%f"), FValue);
	case EPS_ValueType::Vector: return "<Vector> "+VValue.ToString();
	case EPS_ValueType::Transform: return "<Transform> "+TValue.ToString();
	case EPS_ValueType::Actor: return "<Actor> " + (ActorValue? ActorValue->GetName() : "None");
	case EPS_ValueType::Text: return "<Text> " + TextValue;
	case EPS_ValueType::GameplayTag: return "<GT> " + GTValue.ToString();
	case EPS_ValueType::Bool: return "<Bool> " + (BoolValue? FString("True") : FString("False"));
	default: return "";
	}
}

FPS_Element::FPS_Element(const FGameplayTag InKey, const FPS_Value& InValue, float InTime)
{
	Key = InKey;
	Value = InValue;
	Time = InTime;
}

FString FPS_Element::GetString(UWorld* InWorld, bool bIsGetOperationTime) const
{
	return FString::Printf(TEXT("%s : %s (%.2fs)"),
		*Key.ToString(),
		*Value.GetString(),
		bIsGetOperationTime? Time : (InWorld? InWorld->GetTimeSeconds() - Time : -1.0f)
		);
}

FPS_StateOperation::FPS_StateOperation(EPS_StateEventType InOperationType, FPS_Element InStateElement)
{
	OperationType = InOperationType;
	StateElement = InStateElement;
}

FString FPS_StateOperation::GetString(UWorld* InWorld) const
{
	return FString::Printf(TEXT("<%s> %s"),
		*(StaticEnum<EPS_StateEventType>()->GetNameStringByIndex( ( int32 ) OperationType)),
		*StateElement.GetString(InWorld, true)
		);
}

void FPS_State::SetState(const FGameplayTag Key, const FPS_Value& Value, float Time)
{
	for (int i=0; i<State.Num(); i++)
	{
		if (State[i].Key == Key)
		{
			if (State[i].Value.EqualTo(Value))
			{
				return;
			}
			State[i].Value = Value;
			State[i].Time = Time;
			AddStateOperation(FPS_StateOperation(EPS_StateEventType::Change,State[i]));
			return;
		}
	}
	
	State.Add(FPS_Element(Key, Value, Time));
	AddStateOperation(FPS_StateOperation(EPS_StateEventType::Add,FPS_Element(Key, Value, Time)));
}

bool FPS_State::RemoveState(const FGameplayTag Key, FPS_Value& OldValue, float Time)
{
	for (int i=0; i<State.Num(); i++)
	{
		if (State[i].Key == Key)
		{
			OldValue = State[i].Value;
			State[i].Time = Time;
			AddStateOperation(FPS_StateOperation(EPS_StateEventType::Remove,State[i]));
			State.RemoveAt(i);
			return true;
		}
	}
	return false;
}

bool FPS_State::GetFValue(const FGameplayTag Key, float& Result, float& Time)
{
	for (int i=0; i<State.Num(); i++)
	{
		if (State[i].Key == Key && State[i].Value.IsTypeMatch(EPS_ValueType::Float))
		{
			Result = UGBWPSFuncLib::GetFValue(State[i].Value);
			Time = State[i].Time;
			return true;
		}
	}
	return false;
}

bool FPS_State::GetVValue(const FGameplayTag Key, FVector& Result, float& Time)
{
	for (int i=0; i<State.Num(); i++)
	{
		if (State[i].Key == Key && State[i].Value.IsTypeMatch(EPS_ValueType::Vector))
		{
			Result = UGBWPSFuncLib::GetVValue(State[i].Value);
			Time = State[i].Time;
			return true;
		}
	}
	return false;
}

bool FPS_State::GetTValue(const FGameplayTag Key, FTransform& Result, float& Time)
{
	for (int i=0; i<State.Num(); i++)
	{
		if (State[i].Key == Key && State[i].Value.IsTypeMatch(EPS_ValueType::Transform))
		{
			Result = UGBWPSFuncLib::GetTValue(State[i].Value);
			Time = State[i].Time;
			return true;
		}
	}
	return false;
}

bool FPS_State::GetActorValue(const FGameplayTag Key, AActor*& Result, float& Time)
{
	for (int i=0; i<State.Num(); i++)
	{
		if (State[i].Key == Key && State[i].Value.IsTypeMatch(EPS_ValueType::Actor))
		{
			Result = UGBWPSFuncLib::GetActorValue(State[i].Value);
			Time = State[i].Time;
			return true;
		}
	}
	return false;
}

bool FPS_State::GetTextValue(const FGameplayTag Key, FString& Result, float& Time)
{
	for (int i=0; i<State.Num(); i++)
	{
		if (State[i].Key == Key && State[i].Value.IsTypeMatch(EPS_ValueType::Text))
		{
			Result = UGBWPSFuncLib::GetTextValue(State[i].Value);
			Time = State[i].Time;
			return true;
		}
	}
	return false;
}

bool FPS_State::GetGTValue(const FGameplayTag Key, FGameplayTag& Result, float& Time)
{
	for (int i=0; i<State.Num(); i++)
	{
		if (State[i].Key == Key && State[i].Value.IsTypeMatch(EPS_ValueType::GameplayTag))
		{
			Result = UGBWPSFuncLib::GetGTValue(State[i].Value);
			Time = State[i].Time;
			return true;
		}
	}
	return false;
}

bool FPS_State::GetBoolValue(FGameplayTag Key, bool& Result, float& Time)
{
	for (int i=0; i<State.Num(); i++)
	{
		if (State[i].Key == Key && State[i].Value.IsTypeMatch(EPS_ValueType::Bool))
		{
			Result = UGBWPSFuncLib::GetBoolValue(State[i].Value);
			Time = State[i].Time;
			return true;
		}
	}
	return false;
}

bool FPS_State::GetFValue_History(FGameplayTag Key, int DistanceFromNow, float& Result, float& Time)
{
	if (DistanceFromNow <= 0)
	{
		return GetFValue(Key,Result,Time);
	}

	FPS_StateOperation LastOperation;
	FPS_StateOperation TargetOperation;
	int DistanceCache = 0;
	bool bIsGet = false;
	for (int i=0; i<StateOperations.Num(); i++)
	{
		int Index = -1;
		if (StateOperationIndexNow - 1 - i >= 0)
		{
			Index = StateOperationIndexNow - i - 1;
		}
		else
		{
			Index = StateOperationIndexNow - i - 1 + StateOperations.Num();
		}
		if (StateOperations.Num() <= Index || Index < 0)
		{
			break;
		}

		if (StateOperations[Index].StateElement.Key == Key
			&& StateOperations[Index].StateElement.Value.IsTypeMatch(EPS_ValueType::Float))
		{
			DistanceCache++;
			if (DistanceCache == DistanceFromNow)
			{
				LastOperation = StateOperations[Index];
			}
			else if (DistanceCache == DistanceFromNow+1)
			{
				TargetOperation = StateOperations[Index];
				bIsGet = true;
				break;
			}
		}
	}

	if (bIsGet)
	{
		if (TargetOperation.OperationType == EPS_StateEventType::Add || TargetOperation.OperationType == EPS_StateEventType::Change)
		{
			Result = TargetOperation.StateElement.Value.GetFValue();
			Time = LastOperation.StateElement.Time - TargetOperation.StateElement.Time;
		}
		else
		{
			return false;
		}
	}

	return bIsGet;
}

bool FPS_State::GetVValue_History(FGameplayTag Key, int DistanceFromNow, FVector& Result, float& Time)
{
	if (DistanceFromNow <= 0)
	{
		return GetVValue(Key,Result,Time);
	}

	FPS_StateOperation LastOperation;
	FPS_StateOperation TargetOperation;
	int DistanceCache = 0;
	bool bIsGet = false;
	for (int i=0; i<StateOperations.Num(); i++)
	{
		int Index = -1;
		if (StateOperationIndexNow - 1 - i >= 0)
		{
			Index = StateOperationIndexNow - i - 1;
		}
		else
		{
			Index = StateOperationIndexNow - i - 1 + StateOperations.Num();
		}
		if (StateOperations.Num() <= Index || Index < 0)
		{
			break;
		}

		if (StateOperations[Index].StateElement.Key == Key
			&& StateOperations[Index].StateElement.Value.IsTypeMatch(EPS_ValueType::Vector))
		{
			DistanceCache++;
			if (DistanceCache == DistanceFromNow)
			{
				LastOperation = StateOperations[Index];
			}
			else if (DistanceCache == DistanceFromNow+1)
			{
				TargetOperation = StateOperations[Index];
				bIsGet = true;
				break;
			}
		}
	}

	if (bIsGet)
	{
		if (TargetOperation.OperationType == EPS_StateEventType::Add || TargetOperation.OperationType == EPS_StateEventType::Change)
		{
			Result = TargetOperation.StateElement.Value.GetVValue();
			Time = LastOperation.StateElement.Time - TargetOperation.StateElement.Time;
		}
		else
		{
			return false;
		}
	}

	return bIsGet;
}

bool FPS_State::GetTValue_History(FGameplayTag Key, int DistanceFromNow, FTransform& Result, float& Time)
{
	if (DistanceFromNow <= 0)
	{
		return GetTValue(Key,Result,Time);
	}

	FPS_StateOperation LastOperation;
	FPS_StateOperation TargetOperation;
	int DistanceCache = 0;
	bool bIsGet = false;
	for (int i=0; i<StateOperations.Num(); i++)
	{
		int Index = -1;
		if (StateOperationIndexNow - 1 - i >= 0)
		{
			Index = StateOperationIndexNow - i - 1;
		}
		else
		{
			Index = StateOperationIndexNow - i - 1 + StateOperations.Num();
		}
		if (StateOperations.Num() <= Index || Index < 0)
		{
			break;
		}

		if (StateOperations[Index].StateElement.Key == Key
			&& StateOperations[Index].StateElement.Value.IsTypeMatch(EPS_ValueType::Transform))
		{
			DistanceCache++;
			if (DistanceCache == DistanceFromNow)
			{
				LastOperation = StateOperations[Index];
			}
			else if (DistanceCache == DistanceFromNow+1)
			{
				TargetOperation = StateOperations[Index];
				bIsGet = true;
				break;
			}
		}
	}

	if (bIsGet)
	{
		if (TargetOperation.OperationType == EPS_StateEventType::Add || TargetOperation.OperationType == EPS_StateEventType::Change)
		{
			Result = TargetOperation.StateElement.Value.GetTValue();
			Time = LastOperation.StateElement.Time - TargetOperation.StateElement.Time;
		}
		else
		{
			return false;
		}
	}

	return bIsGet;
}

bool FPS_State::GetActorValue_History(FGameplayTag Key, int DistanceFromNow, AActor*& Result, float& Time)
{
	if (DistanceFromNow <= 0)
	{
		return GetActorValue(Key,Result,Time);
	}

	FPS_StateOperation LastOperation;
	FPS_StateOperation TargetOperation;
	int DistanceCache = 0;
	bool bIsGet = false;
	for (int i=0; i<StateOperations.Num(); i++)
	{
		int Index = -1;
		if (StateOperationIndexNow - 1 - i >= 0)
		{
			Index = StateOperationIndexNow - i - 1;
		}
		else
		{
			Index = StateOperationIndexNow - i - 1 + StateOperations.Num();
		}
		if (StateOperations.Num() <= Index || Index < 0)
		{
			break;
		}

		if (StateOperations[Index].StateElement.Key == Key
			&& StateOperations[Index].StateElement.Value.IsTypeMatch(EPS_ValueType::Actor))
		{
			DistanceCache++;
			if (DistanceCache == DistanceFromNow)
			{
				LastOperation = StateOperations[Index];
			}
			else if (DistanceCache == DistanceFromNow+1)
			{
				TargetOperation = StateOperations[Index];
				bIsGet = true;
				break;
			}
		}
	}

	if (bIsGet)
	{
		if (TargetOperation.OperationType == EPS_StateEventType::Add || TargetOperation.OperationType == EPS_StateEventType::Change)
		{
			Result = TargetOperation.StateElement.Value.GetActorValue();
			Time = LastOperation.StateElement.Time - TargetOperation.StateElement.Time;
		}
		else
		{
			return false;
		}
	}

	return bIsGet;
}

bool FPS_State::GetTextValue_History(FGameplayTag Key, int DistanceFromNow, FString& Result, float& Time)
{
	if (DistanceFromNow <= 0)
	{
		return GetTextValue(Key,Result,Time);
	}

	FPS_StateOperation LastOperation;
	FPS_StateOperation TargetOperation;
	int DistanceCache = 0;
	bool bIsGet = false;
	for (int i=0; i<StateOperations.Num(); i++)
	{
		int Index = -1;
		if (StateOperationIndexNow - 1 - i >= 0)
		{
			Index = StateOperationIndexNow - i - 1;
		}
		else
		{
			Index = StateOperationIndexNow - i - 1 + StateOperations.Num();
		}
		if (StateOperations.Num() <= Index || Index < 0)
		{
			break;
		}

		if (StateOperations[Index].StateElement.Key == Key
			&& StateOperations[Index].StateElement.Value.IsTypeMatch(EPS_ValueType::Text))
		{
			DistanceCache++;
			if (DistanceCache == DistanceFromNow)
			{
				LastOperation = StateOperations[Index];
			}
			else if (DistanceCache == DistanceFromNow+1)
			{
				TargetOperation = StateOperations[Index];
				bIsGet = true;
				break;
			}
		}
	}

	if (bIsGet)
	{
		if (TargetOperation.OperationType == EPS_StateEventType::Add || TargetOperation.OperationType == EPS_StateEventType::Change)
		{
			Result = TargetOperation.StateElement.Value.GetTextValue();
			Time = LastOperation.StateElement.Time - TargetOperation.StateElement.Time;
		}
		else
		{
			return false;
		}
	}

	return bIsGet;
}

bool FPS_State::GetGTValue_History(FGameplayTag Key, int DistanceFromNow, FGameplayTag& Result, float& Time)
{
	if (DistanceFromNow <= 0)
	{
		return GetGTValue(Key,Result,Time);
	}

	FPS_StateOperation LastOperation;
	FPS_StateOperation TargetOperation;
	int DistanceCache = 0;
	bool bIsGet = false;
	for (int i=0; i<StateOperations.Num(); i++)
	{
		int Index = -1;
		if (StateOperationIndexNow - 1 - i >= 0)
		{
			Index = StateOperationIndexNow - i - 1;
		}
		else
		{
			Index = StateOperationIndexNow - i - 1 + StateOperations.Num();
		}
		if (StateOperations.Num() <= Index || Index < 0)
		{
			break;
		}

		if (StateOperations[Index].StateElement.Key == Key
			&& StateOperations[Index].StateElement.Value.IsTypeMatch(EPS_ValueType::GameplayTag))
		{
			DistanceCache++;
			if (DistanceCache == DistanceFromNow)
			{
				LastOperation = StateOperations[Index];
			}
			else if (DistanceCache == DistanceFromNow+1)
			{
				TargetOperation = StateOperations[Index];
				bIsGet = true;
				break;
			}
		}
	}

	if (bIsGet)
	{
		if (TargetOperation.OperationType == EPS_StateEventType::Add || TargetOperation.OperationType == EPS_StateEventType::Change)
		{
			Result = TargetOperation.StateElement.Value.GetGTValue();
			Time = LastOperation.StateElement.Time - TargetOperation.StateElement.Time;
		}
		else
		{
			return false;
		}
	}

	return bIsGet;
}

bool FPS_State::GetBoolValue_History(FGameplayTag Key, int DistanceFromNow, bool& Result, float& Time)
{
	if (DistanceFromNow <= 0)
	{
		return GetBoolValue(Key,Result,Time);
	}

	FPS_StateOperation LastOperation;
	FPS_StateOperation TargetOperation;
	int DistanceCache = 0;
	bool bIsGet = false;
	for (int i=0; i<StateOperations.Num(); i++)
	{
		int Index = -1;
		if (StateOperationIndexNow - i >= 0)
		{
			Index = StateOperationIndexNow - i;
		}
		else
		{
			Index = StateOperationIndexNow - i + StateOperations.Num();
		}
		if (StateOperations.Num() <= Index || Index < 0)
		{
			break;
		}

		if (StateOperations[Index].StateElement.Key == Key
			&& StateOperations[Index].StateElement.Value.IsTypeMatch(EPS_ValueType::Bool))
		{
			DistanceCache++;
			if (DistanceCache == DistanceFromNow)
			{
				LastOperation = StateOperations[Index];
			}
			else if (DistanceCache == DistanceFromNow+1)
			{
				TargetOperation = StateOperations[Index];
				bIsGet = true;
				break;
			}
		}
	}

	if (bIsGet)
	{
		if (TargetOperation.OperationType == EPS_StateEventType::Add || TargetOperation.OperationType == EPS_StateEventType::Change)
		{
			Result = TargetOperation.StateElement.Value.GetBoolValue();
			Time = LastOperation.StateElement.Time - TargetOperation.StateElement.Time;
		}
		else
		{
			return false;
		}
	}

	return bIsGet;
}

void FPS_State::AddStateOperation(const FPS_StateOperation& NewStateOperation)
{
	if (StateOperations.Num() <= MaxStateOperationNum)
	{
		StateOperationIndexNow = StateOperations.Num();
		StateOperations.Add(NewStateOperation);
	}
	else
	{
		if (StateOperationIndexNow >= MaxStateOperationNum)
		{
			StateOperationIndexNow = 0;
		}
		StateOperations[StateOperationIndexNow] = NewStateOperation;
		StateOperationIndexNow++;
	}
}

bool FPS_State::CheckStateChangeType(
	FGameplayTag Key,
	const FPS_Value& Value,
	EPS_StateEventType& ChangeType,
	FPS_Value& OldValue,
	FPS_Value& NewValue)
{
	NewValue = Value;
	for (int i=0; i<State.Num(); i++)
	{
		if (State[i].Key == Key)
		{
			if (!State[i].Value.EqualTo(Value))
			{
				ChangeType = EPS_StateEventType::Change;
				OldValue = State[i].Value;
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	
	ChangeType = EPS_StateEventType::Add;
	OldValue = Value;
	return true;
}

FString FPS_State::GetString(UWorld* InWorld, int StateOperationNum)
{
	FString Res = "";
	for (int i=State.Num()-1; i>=0; i--)
	{
		Res = Res + "\n" + State[i].GetString(InWorld);
	}

	if (StateOperationNum > 0)
	{
		Res =  Res + "\n\n ---------StateOperation---------";
		for (int i=0; i<StateOperationNum; i++)
		{
			int Index = -1;
			if (StateOperationIndexNow - i >= 0)
			{
				Index = StateOperationIndexNow - i;
			}
			else if (StateOperations.Num()>=StateOperationNum)
			{
				Index = StateOperationIndexNow - i + StateOperations.Num();
			}
			if (StateOperations.Num() <= Index || Index < 0)
			{
				break;
			}
			Res = Res + FString::Printf(TEXT("\n [%d] %s"), Index ,*StateOperations[Index].GetString(InWorld));
		}
		Res =  Res + "\n ---------StateOperation---------";
	}
	return Res;
}

FString FPE_Event::GetString(UWorld* InWorld)
{
	return ">>>>>>>>>PowerfulEvent Start ("+ Key.ToString() +")<<<<<<<<< : "
	+ EventState.GetString(InWorld,0)
	+ "\n>>>>>>>>>PowerfulEvent End ("+ Key.ToString() +")<<<<<<<<< : ";
}

FPS_Event::FPS_Event(EPS_StateEventType InStateEventType, FGameplayTag InStateKey, FPS_Value InOldValue,
	FPS_Value InNewValue)
{
	StateEventType = InStateEventType;
	StateKey = InStateKey;
	OldValue = InOldValue;
	NewValue = InNewValue;
}

FString FPS_Event::GetString()
{
	return ">>>>>>>>PS_Event Start<<<<<<<<<\n"
		+ (StaticEnum<EPS_StateEventType>()->GetNameStringByIndex( static_cast<int32>(StateEventType))) + "\n"
		+ "StateKey : " + StateKey.ToString() + "\n"
		+ "OldValue : " + OldValue.GetString() + "\n"
		+ "NewValue : " + NewValue.GetString() + "\n"
		+ ">>>>>>>>PS_Event End<<<<<<<<<";
}

UGBWPowerfulStateComponent::UGBWPowerfulStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UGBWPowerfulStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Parameters;
	Parameters.bIsPushBased = true;
	Parameters.Condition = COND_None;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MaxStateOperationNum, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PowerfulState, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PowerfulStateGuid, Parameters)

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsDebug, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, StateOperationNum, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, DebugTextColor, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bPrintToScreen, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bPrintToLog, Parameters)
}

void UGBWPowerfulStateComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		PowerfulStateGuid = FGuid::NewGuid();
		SetMaxStateOperationNum(MaxStateOperationNum);
	}
}

void UGBWPowerfulStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PSDebug();
}

bool UGBWPowerfulStateComponent::HasNetAuth() const
{
	const EGBWActorNetType ActorNetType = UGBWPowerfulToolsFuncLib::GetActorNetType(GetOwner());
	return !(ActorNetType == EGBWActorNetType::InClient_NotPlayer || ActorNetType == EGBWActorNetType::InClient_RemotePlayer);
}

void UGBWPowerfulStateComponent::SetState_Imp(const FGameplayTag Key, const FPS_Value& Value)
{
	EPS_StateEventType ChangeType = EPS_StateEventType::Change;
	FPS_Value OldValue;
	FPS_Value NewValue;
	if (PowerfulState.CheckStateChangeType(Key, Value, ChangeType, OldValue, NewValue))
	{
		PowerfulState.SetState(Key, Value, this->GetWorld()->GetTimeSeconds());
		OnPowerfulState.Broadcast(Key, FPS_Event(ChangeType,Key,OldValue,NewValue));
		if (GetOwner()->GetClass()->ImplementsInterface(UGBWPSBPI::StaticClass()))
		{
			IGBWPSBPI::Execute_OnPowerfulState(GetOwner(), Key, FPS_Event(ChangeType,Key,OldValue,NewValue));
		}
	}
}

void UGBWPowerfulStateComponent::SetMaxStateOperationNum(int InMaxStateOperationNum)
{
	if (!HasNetAuth())
	{
		return;
	}
	SetMaxStateOperationNum_Server(InMaxStateOperationNum);
}
void UGBWPowerfulStateComponent::SetMaxStateOperationNum_Server_Implementation(int InMaxStateOperationNum)
{
	if (PowerfulState.StateOperations.Num() < InMaxStateOperationNum)
	{
		MaxStateOperationNum = InMaxStateOperationNum;
		PowerfulState.MaxStateOperationNum = InMaxStateOperationNum;
	}
}

void UGBWPowerfulStateComponent::SetState(const FGameplayTag Key, const FPS_Value Value)
{
	if (!HasNetAuth())
	{
		return;
	}
	SetState_Server(Key,Value);
}
void UGBWPowerfulStateComponent::SetState_Server_Implementation(const FGameplayTag Key, const FPS_Value Value)
{
	SetState_Imp(Key, Value);
}

void UGBWPowerfulStateComponent::RemoveState(const FGameplayTag Key)
{
	if (!HasNetAuth())
	{
		return;
	}
	RemoveState_Server(Key);
}

void UGBWPowerfulStateComponent::SetStateFloat(FGameplayTag Key, float Value)
{
	if (!HasNetAuth())
	{
		return;
	}
	SetStateFloat_Server(Key,Value);
}
void UGBWPowerfulStateComponent::SetStateFloat_Server_Implementation(FGameplayTag Key, float Value)
{
	SetState_Imp(Key, FPS_Value(Value));
}

void UGBWPowerfulStateComponent::SetStateVector(FGameplayTag Key, FVector Value)
{
	if (!HasNetAuth())
	{
		return;
	}
	SetStateVector_Server(Key,Value);
}
void UGBWPowerfulStateComponent::SetStateVector_Server_Implementation(FGameplayTag Key, FVector Value)
{
	SetState_Imp(Key, FPS_Value(Value));
}

void UGBWPowerfulStateComponent::SetStateTransform(FGameplayTag Key, FTransform Value)
{
	if (!HasNetAuth())
	{
		return;
	}
	SetStateTransform_Server(Key, Value);
}
void UGBWPowerfulStateComponent::SetStateTransform_Server_Implementation(FGameplayTag Key, FTransform Value)
{
	SetState_Imp(Key, FPS_Value(Value));
}

void UGBWPowerfulStateComponent::SetStateActor(FGameplayTag Key, AActor* Value)
{
	if (!HasNetAuth())
	{
		return;
	}
	SetStateActor_Server(Key, Value);
}
void UGBWPowerfulStateComponent::SetStateActor_Server_Implementation(FGameplayTag Key, AActor* Value)
{
	SetState_Imp(Key, FPS_Value(Value));
}

void UGBWPowerfulStateComponent::SetStateText(FGameplayTag Key, FString Value)
{
	if (!HasNetAuth())
	{
		return;
	}
	SetStateText_Server(Key, Value);
}
void UGBWPowerfulStateComponent::SetStateText_Server_Implementation(FGameplayTag Key, const FString& Value)
{
	SetState_Imp(Key, FPS_Value(Value));
}

void UGBWPowerfulStateComponent::SetStateGT(FGameplayTag Key, FGameplayTag Value)
{
	if (!HasNetAuth())
	{
		return;
	}
	FGameplayTag CurrentValue = FGameplayTag();
	float Time = -1.0f;
	if (GetGTValue(Key,CurrentValue,Time) && CurrentValue == Value)
	{
		return;
	}
	SetStateGT_Server(Key, Value);
}

void UGBWPowerfulStateComponent::SetStateGT_Server_Implementation(FGameplayTag Key, FGameplayTag Value)
{
	SetState_Imp(Key, FPS_Value(Value));
}

void UGBWPowerfulStateComponent::SetStateBool(FGameplayTag Key, bool Value)
{
	if (!HasNetAuth())
	{
		return;
	}
	bool CurrentValue = false;
	float Time = -1.0f;
	if (GetBoolValue(Key,CurrentValue,Time) && CurrentValue == Value)
	{
		return;
	}
	SetStateBool_Server(Key, Value);
}

void UGBWPowerfulStateComponent::SetStateBool_Server_Implementation(FGameplayTag Key, bool Value)
{
	SetState_Imp(Key, FPS_Value(Value));
}

void UGBWPowerfulStateComponent::RemoveState_Server_Implementation(const FGameplayTag Key)
{
	FPS_Value OldValue;
	if (PowerfulState.RemoveState(Key,OldValue,this->GetWorld()->GetTimeSeconds()))
	{
		OnPowerfulState.Broadcast( Key, FPS_Event(EPS_StateEventType::Remove,Key,OldValue,FPS_Value()));
		if (GetOwner()->GetClass()->ImplementsInterface(UGBWPSBPI::StaticClass()))
		{
			IGBWPSBPI::Execute_OnPowerfulState(GetOwner(), Key, FPS_Event(EPS_StateEventType::Remove,Key,OldValue,FPS_Value()));
		}
	}
}

bool UGBWPowerfulStateComponent::GetFValue(FGameplayTag Key, float& Result, float& Duration)
{
	float Time = -1.0f;
	if(PowerfulState.GetFValue(Key,Result,Time))
	{
		Duration = this->GetWorld()->GetTimeSeconds() - Time;
		return true;
	}
	return false;
}

bool UGBWPowerfulStateComponent::GetVValue(FGameplayTag Key, FVector& Result, float& Duration)
{
	float Time = -1.0f;
	if(PowerfulState.GetVValue(Key,Result,Time))
	{
		Duration = this->GetWorld()->GetTimeSeconds() - Time;
		return true;
	}
	return false;
}

bool UGBWPowerfulStateComponent::GetTValue(FGameplayTag Key, FTransform& Result, float& Duration)
{
	float Time = -1.0f;
	if(PowerfulState.GetTValue(Key,Result,Time))
	{
		Duration = this->GetWorld()->GetTimeSeconds() - Time;
		return true;
	}
	return false;
}

bool UGBWPowerfulStateComponent::GetActorValue(FGameplayTag Key, AActor*& Result, float& Duration)
{
	float Time = -1.0f;
	if(PowerfulState.GetActorValue(Key,Result,Time))
	{
		Duration = this->GetWorld()->GetTimeSeconds() - Time;
		return true;
	}
	return false;
}

bool UGBWPowerfulStateComponent::GetTextValue(FGameplayTag Key, FString& Result, float& Duration)
{
	float Time = -1.0f;
	if(PowerfulState.GetTextValue(Key,Result,Time))
	{
		Duration = this->GetWorld()->GetTimeSeconds() - Time;
		return true;
	}
	return false;
}

bool UGBWPowerfulStateComponent::GetGTValue(FGameplayTag Key, FGameplayTag& Result, float& Duration)
{
	float Time = -1.0f;
	if(PowerfulState.GetGTValue(Key,Result,Time))
	{
		Duration = this->GetWorld()->GetTimeSeconds() - Time;
		return true;
	}
	return false;
}

bool UGBWPowerfulStateComponent::GetBoolValue(FGameplayTag Key, bool& Result, float& Duration)
{
	float Time = -1.0f;
	if(PowerfulState.GetBoolValue(Key,Result,Time))
	{
		Duration = this->GetWorld()->GetTimeSeconds() - Time;
		return true;
	}
	return false;
}

bool UGBWPowerfulStateComponent::GetFValue_History(FGameplayTag Key, int DistanceFromNow, float& Result,
	float& Duration)
{
	return PowerfulState.GetFValue_History(Key,DistanceFromNow,Result,Duration);
}

bool UGBWPowerfulStateComponent::GetVValue_History(FGameplayTag Key, int DistanceFromNow, FVector& Result,
	float& Duration)
{
	return PowerfulState.GetVValue_History(Key,DistanceFromNow,Result,Duration);
}

bool UGBWPowerfulStateComponent::GetTValue_History(FGameplayTag Key, int DistanceFromNow, FTransform& Result,
	float& Duration)
{
	return PowerfulState.GetTValue_History(Key,DistanceFromNow,Result,Duration);
}

bool UGBWPowerfulStateComponent::GetActorValue_History(FGameplayTag Key, int DistanceFromNow, AActor*& Result,
	float& Duration)
{
	return PowerfulState.GetActorValue_History(Key,DistanceFromNow,Result,Duration);
}

bool UGBWPowerfulStateComponent::GetTextValue_History(FGameplayTag Key, int DistanceFromNow, FString& Result,
	float& Duration)
{
	return PowerfulState.GetTextValue_History(Key,DistanceFromNow,Result,Duration);
}

bool UGBWPowerfulStateComponent::GetGTValue_History(FGameplayTag Key, int DistanceFromNow, FGameplayTag& Result,
	float& Duration)
{
	return PowerfulState.GetGTValue_History(Key,DistanceFromNow,Result,Duration);
}

bool UGBWPowerfulStateComponent::GetBoolValue_History(FGameplayTag Key, int DistanceFromNow, bool& Result,
	float& Duration)
{
	return PowerfulState.GetBoolValue_History(Key,DistanceFromNow,Result,Duration);
}

void UGBWPowerfulStateComponent::SendEvent(const FPE_Event PE_Event)
{
	if (!HasNetAuth())
	{
		return;
	}
	SendEvent_Server(PE_Event);
}
void UGBWPowerfulStateComponent::SendEvent_Server_Implementation(const FPE_Event PE_Event)
{
	SendEvent_Multicast(PE_Event);
}
void UGBWPowerfulStateComponent::SendEvent_Multicast_Implementation(const FPE_Event PE_Event)
{
	SendEvent_Imp(PE_Event);
}
void UGBWPowerfulStateComponent::SendEvent_Imp(const FPE_Event& PE_Event) const
{
	OnPowerfulEvent.Broadcast(PE_Event.Key,PE_Event);
	PEDebug(PE_Event);

	if (GetOwner()->GetClass()->ImplementsInterface(UGBWPSBPI::StaticClass()))
	{
		IGBWPSBPI::Execute_OnPowerfulEvent(GetOwner(), PE_Event.Key, PE_Event);
	}
}

void UGBWPowerfulStateComponent::ActivePSDebug(const FColor InDebugTextColor, const bool InPrintToScreen, const bool InPrintToLog)
{
	if (!HasNetAuth())
	{
		return;
	}
	ActivePSDebug_Server(InDebugTextColor, InPrintToScreen,InPrintToLog);
}

void UGBWPowerfulStateComponent::ActivePSDebug_Server_Implementation(FColor InDebugTextColor, bool InPrintToScreen,
	bool InPrintToLog)
{
	bIsDebug = true;
	DebugTextColor = InDebugTextColor;
	bPrintToScreen = InPrintToScreen;
	bPrintToLog = InPrintToLog;
}

void UGBWPowerfulStateComponent::DeactivePSDebug()
{
	if (!HasNetAuth())
	{
		return;
	}
	DeactivePSDebug_Server();
}

void UGBWPowerfulStateComponent::DeactivePSDebug_Server_Implementation()
{
	bIsDebug = false;
}

void UGBWPowerfulStateComponent::PSDebug()
{
	if (bIsDebug)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(GetOwner(), EGetWorldErrorMode::ReturnNull);
		FString Prefix;
		if (World)
		{
			if (World->WorldType == EWorldType::PIE)
			{
				switch(World->GetNetMode())
				{
				case NM_Client:
					Prefix = FString::Printf(TEXT("Client %d: "), GPlayInEditorID);
					break;
				case NM_DedicatedServer:
				case NM_ListenServer:
					Prefix = FString::Printf(TEXT("Server: "));
					break;
				case NM_Standalone:
					break;
				default: ;
				}
			}
		}
		
		UKismetSystemLibrary::PrintString(GetOwner()->GetWorld(),
			">>>>>>>>>>>>Powerful State Debug End<<<<<<<<<<<<<<<\n",
			bPrintToScreen,bPrintToLog,DebugTextColor,0.0f,FName(Prefix + "PSState" + FString::Printf(TEXT("%d"), PowerfulState.State.Num())));

		UKismetSystemLibrary::PrintString(GetOwner()->GetWorld(),
			PowerfulState.GetString(GetOwner()->GetWorld(),StateOperationNum),
			bPrintToScreen,bPrintToLog,DebugTextColor,0.0f,FName(Prefix + "PSState" + FString::Printf(TEXT("%d"), 0)));
		
		UKismetSystemLibrary::PrintString(GetOwner()->GetWorld(),
        			FString::Printf(TEXT("(LocalRole-%s   RemoteRole-%s)"),
        				*UEnum::GetValueAsName(GetOwner()->GetLocalRole()).ToString(),
        				*UEnum::GetValueAsName(GetOwner()->GetRemoteRole()).ToString()),
        			bPrintToScreen,bPrintToLog,DebugTextColor,0.0f,FName(Prefix + "PSState" + FString::Printf(TEXT("%d"), -1)));
		UKismetSystemLibrary::PrintString(GetOwner()->GetWorld(),
        			"(OwnerActor : " + GetOwner()->GetName() + ")",
        			bPrintToScreen,bPrintToLog,DebugTextColor,0.0f,FName(Prefix + "PSState" + FString::Printf(TEXT("%d"), -2)));
		UKismetSystemLibrary::PrintString(GetOwner()->GetWorld(),
					"(StateGuid : " + PowerfulStateGuid.ToString() + ")",
					bPrintToScreen,bPrintToLog,DebugTextColor,0.0f,FName(Prefix + "PSState" + FString::Printf(TEXT("%d"), -3)));
		UKismetSystemLibrary::PrintString(GetOwner()->GetWorld(),
			">>>>>>>>>>>>Powerful State Debug Start<<<<<<<<<<<<<<<",
			bPrintToScreen,bPrintToLog,DebugTextColor,0.0f,FName(Prefix + "PSState" + FString::Printf(TEXT("%d"), -4)));
	}
}

void UGBWPowerfulStateComponent::PEDebug(const FPE_Event& PE_Event) const
{
	if (bIsDebug)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(GetOwner(), EGetWorldErrorMode::ReturnNull);
		FString Prefix;
		if (World)
		{
			if (World->WorldType == EWorldType::PIE)
			{
				switch(World->GetNetMode())
				{
				case NM_Client:
					Prefix = FString::Printf(TEXT("Client %d: "), GPlayInEditorID);
					break;
				case NM_DedicatedServer:
				case NM_ListenServer:
					Prefix = FString::Printf(TEXT("Server: "));
					break;
				case NM_Standalone:
					break;
				default: ;
				}
			}
		}
		
		UKismetSystemLibrary::PrintString(GetOwner()->GetWorld(),
			UGBWPSFuncLib::GetPEEventDebugString(PE_Event),
			bPrintToScreen, bPrintToLog,DebugTextColor2,0.0f,FName(Prefix + "PE_" + FString::Printf(TEXT("%d"), 0)));
	}
}
