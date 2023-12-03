// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "GBWFlowMoveObjects.h"
#include "GBWFlowMoveComponent.h"
#include "GBWFlowMoveFuncLib.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

bool FFlowMoveMoveVectorCondition::IsMet(const FGBWFlowMoveState& FlowMoveState)
{
	if (ConditionType == HasValue)
	{
		return FlowMoveState.MoveVector.Size() > 0;
	}

	if (ConditionType == HasNoValue)
	{
		return FlowMoveState.MoveVector.Size() == 0;
	}
	
	bool bIsMetSize = MoveVectorSizeRange.IsEmpty();
	bool bIsMetValue = MoveVectorRange.IsEmpty();
	
	if (!MoveVectorSizeRange.IsEmpty())
	{
		for (auto Item:MoveVectorSizeRange)
		{
			if (Item.InScope(FlowMoveState.MoveVector.Size()))
			{
				bIsMetSize = true;
				break;
			}
		}
	}

	if (!MoveVectorRange.IsEmpty())
	{
		for (auto Item:MoveVectorRange)
		{
			if (Item.InScope(FlowMoveState.MoveVector))
			{
				bIsMetValue = true;
				break;
			}
		}
	}

	return bIsMetSize && bIsMetValue;
}

bool FFlowMoveMoveVectorCondition::IsMet(const FVector& MoveVector)
{
	if (ConditionType == HasValue)
	{
		return MoveVector.Size() > 0;
	}

	if (ConditionType == HasNoValue)
	{
		return MoveVector.Size() == 0;
	}
	
	bool bIsMetSize = MoveVectorSizeRange.IsEmpty();
	bool bIsMetValue = MoveVectorRange.IsEmpty();
	
	if (!MoveVectorSizeRange.IsEmpty())
	{
		for (auto Item:MoveVectorSizeRange)
		{
			if (Item.InScope(MoveVector.Size()))
			{
				bIsMetSize = true;
				break;
			}
		}
	}

	if (!MoveVectorRange.IsEmpty())
	{
		for (auto Item:MoveVectorRange)
		{
			if (Item.InScope(MoveVector))
			{
				bIsMetValue = true;
				break;
			}
		}
	}

	return bIsMetSize && bIsMetValue;
}

bool FFlowMoveSceneDirectionCondition::IsMet(const FGBWFlowMoveScene& FlowMoveScene) const
{
	switch (ConditionType)
	{
	case EDirectionConditionType::ValueRange:
		for (auto Item:SceneDirectionRange)
		{
			if (Item.InScope(FlowMoveScene.TargetLocationScene.Direction))
			{
				return true;
			}
		}
		return false;
	case EDirectionConditionType::MarkInclude:
		return DirectionMark.Contains(FlowMoveScene.TargetLocationScene.SceneDirectionMark);
	case EDirectionConditionType::MarkNotInclude:
		return !DirectionMark.Contains(FlowMoveScene.TargetLocationScene.SceneDirectionMark);
	default:
		return false;
	}
}

void FGBWFlowMoveState::MakeVelocity()
{
	if (OwnerCharacter)
	{
		RealVelocity = (OwnerCharacter->GetActorLocation() - LastActorTransform.GetLocation())/FrameDeltaTime;

		LastActorTransform = OwnerCharacter->GetActorTransform();
	}
}

void FGBWFlowMoveState::ClearCurrentFlowMoveAction()
{
	CurrentFlowMoveActionTag = FGameplayTag();
	CurrentFlowMoveExecutedTime = 0.0f;
}

void FGBWFlowMoveState::ClearLastFlowMoveAction()
{
	LastFlowMoveActionTag = FGameplayTag();
}

void FGBWFlowMoveState::SetSupplementaryScene(FGameplayTag SceneSlot, FGBWFlowMoveScene NewScene)
{
	if (!SceneSlot.IsValid())
	{
		return;
	}
	if (const int Index = GetSupplementarySceneIndex(SceneSlot); Index>=0)
	{
		SupplementaryFlowMoveScene[Index].Scene = NewScene;
	}
	else
	{
		FGBWSupplementaryFlowMoveScene New = FGBWSupplementaryFlowMoveScene();
		New.Scene = NewScene;
		New.SceneSlot = SceneSlot;
		SupplementaryFlowMoveScene.Add(New);
	}
}

void FGBWFlowMoveState::RemoveSupplementaryScene(FGameplayTag SceneSlot)
{
	if (!SceneSlot.IsValid())
	{
		return;
	}
	if (const int Index = GetSupplementarySceneIndex(SceneSlot); Index>=0)
	{
		SupplementaryFlowMoveScene.RemoveAt(Index);
	}
}

bool FGBWFlowMoveState::GetSupplementaryScene(FGameplayTag SceneSlot, FGBWFlowMoveScene& Result)
{
	if (!SceneSlot.IsValid())
	{
		return false;
	}
	if (const int Index = GetSupplementarySceneIndex(SceneSlot); Index>=0)
	{
		Result = SupplementaryFlowMoveScene[Index].Scene;
		return true;
	}
	return false;
}

int FGBWFlowMoveState::GetSupplementarySceneIndex(FGameplayTag SceneSlot)
{
	if (!SceneSlot.IsValid())
	{
		return -1;
	}
	for (int i=0; i<SupplementaryFlowMoveScene.Num(); i++)
	{
		if (SupplementaryFlowMoveScene[i].SceneSlot == SceneSlot)
		{
			return i;
		}
	}
	return -1;
}

void FGBWFlowMoveState::ClearFlowMoveEvent()
{
	TArray<FFlowMoveEvent> FlowMoveEventNow_Temp = FlowMoveEventNow;
	int RemoveCount = 0;
	for (int i=0; i<FlowMoveEventNow_Temp.Num(); i++)
	{
		//Delay 2 frame to clear event
		if (FlowMoveEventNow_Temp[i].InFrameNum <= GFrameCounter-2)
		{
			FlowMoveEventNow.RemoveAt(i-RemoveCount);
			RemoveCount++;
		}
	}
}

void FGBWFlowMoveState::AddFlowMoveEvent(const FFlowMoveEvent& NewEvent)
{
	FFlowMoveEvent TheNewEvent = NewEvent;
	TheNewEvent.InFrameNum = GFrameCounter;
	FlowMoveEventNow.Add(TheNewEvent);
}

bool FGBWFlowMoveState::IsHasFlowMoveEvent(const FFlowMoveEvent& FlowMoveEvent)
{
	for (auto Event:FlowMoveEventNow)
	{
		if (Event.EqualTo(FlowMoveEvent))
		{
			return true;
		}
	}
	return false;
}

void FGBWFlowMoveState::SetMoveVector(const FVector& NewMoveVector)
{
	MoveVector = NewMoveVector;
	LastMoveVector = NewMoveVector;
	LastMoveVectorTime = OwnerCharacter->GetWorld()->GetTimeSeconds();
}

void FGBWFlowMoveState::AddPerceptronTag(const FGameplayTag PerceptronTag)
{
	if (PerceptronTag.IsValid())
	{
		PerceptronReady.AddTag(PerceptronTag);
	}
}

void FGBWFlowMoveState::ReadyPerceptronTag(FGameplayTag PerceptronTag)
{
	if (PerceptronTag.IsValid() && PerceptronReady.HasTag(PerceptronTag))
    {
        PerceptronReady.RemoveTag(PerceptronTag);
		PerceptronNow.AddTag(PerceptronTag);
    }
}

void FGBWFlowMoveState::RemovePerceptronTag(const FGameplayTag PerceptronTag)
{
	if (PerceptronTag.IsValid())
	{
		PerceptronToEnd.AddTag(PerceptronTag);
		PerceptronNow.RemoveTag(PerceptronTag);
	}
	else
	{
		PerceptronToEnd.AppendTags(PerceptronNow);
		PerceptronNow.Reset();
	}
}

void FGBWFlowMoveState::FinishPerceptronTag(FGameplayTag PerceptronTag)
{
	if (PerceptronTag.IsValid() && PerceptronToEnd.HasTag(PerceptronTag))
	{
		PerceptronToEnd.RemoveTag(PerceptronTag);
	}
}

void FGBWFlowMoveState::CheckPerceptronTag()
{
	if (FlowMoveComponent && FlowMoveComponent->FlowMoveBrain)
	{
		FGameplayTagContainer ThePerceptron = PerceptronReady;
		for (auto Element : ThePerceptron)
		{
			if (!FlowMoveComponent-> FlowMoveBrain->FlowMovePerceptronSet.Find(Element))
			{
				PerceptronReady.RemoveTag(Element);
				PerceptronInvalid.AddTag(Element);
			}
		}
		ThePerceptron = PerceptronNow;
		for (auto Element : ThePerceptron)
		{
			if (!FlowMoveComponent-> FlowMoveBrain->FlowMovePerceptronSet.Find(Element))
			{
				PerceptronNow.RemoveTag(Element);
				PerceptronInvalid.AddTag(Element);
			}
		}
		ThePerceptron = PerceptronToEnd;
		for (auto Element : ThePerceptron)
		{
			if (!FlowMoveComponent-> FlowMoveBrain->FlowMovePerceptronSet.Find(Element))
			{
				PerceptronToEnd.RemoveTag(Element);
				PerceptronInvalid.AddTag(Element);
			}
		}
	}
}

void FGBWFlowMoveState::AddScriptTag(FGameplayTag ScriptTag)
{
	if (ScriptTag.IsValid())
    {
        ScriptReady.AddTag(ScriptTag);
    }
}

void FGBWFlowMoveState::ReadyScriptTag(FGameplayTag ScriptTag)
{
	if (ScriptTag.IsValid() && ScriptReady.HasTag(ScriptTag))
	{
		ScriptReady.RemoveTag(ScriptTag);
		ScriptNow.AddTag(ScriptTag);
	}
}
void FGBWFlowMoveState::RemoveScriptTag(FGameplayTag ScriptTag)
{
	if (ScriptTag.IsValid())
	{
		ScriptToEnd.AddTag(ScriptTag);
		ScriptNow.RemoveTag(ScriptTag);
	}
	else
	{
		ScriptToEnd.AppendTags(ScriptNow);
		ScriptNow.Reset();
	}
}
void FGBWFlowMoveState::FinishScriptTag(FGameplayTag ScriptTag)
{
	if (ScriptTag.IsValid() && ScriptToEnd.HasTag(ScriptTag))
	{
		ScriptToEnd.RemoveTag(ScriptTag);
	}
}
void FGBWFlowMoveState::CheckScriptTag()
{
	if (FlowMoveComponent && FlowMoveComponent->FlowMoveBrain)
	{
		for (auto Element : ScriptReady)
		{
			if (!FlowMoveComponent-> FlowMoveBrain->FlowMoveScriptSet.Find(Element))
			{
				ScriptReady.RemoveTag(Element);
				ScriptInvalid.AddTag(Element);
			}
		}
		for (auto Element : ScriptNow)
		{
			if (!FlowMoveComponent-> FlowMoveBrain->FlowMoveScriptSet.Find(Element))
			{
				ScriptNow.RemoveTag(Element);
				ScriptInvalid.AddTag(Element);
			}
		}
		for (auto Element : ScriptToEnd)
		{
			if (!FlowMoveComponent-> FlowMoveBrain->FlowMoveScriptSet.Find(Element))
			{
				ScriptToEnd.RemoveTag(Element);
				ScriptInvalid.AddTag(Element);
			}
		}
	}
}

bool UGBWFlowMoveObject_Base::IsNameStableForNetworking() const
{
	return true;
}

bool UGBWFlowMoveObject_Base::IsSupportedForNetworking() const
{
	return true;
}

UGBWFlowMoveObject_Base::UGBWFlowMoveObject_Base()
{Guid = FGuid::NewGuid();}

void UGBWFlowMoveObject_Base::SetWorldContext(UObject* NewWorldContext)
{
	WorldContext = new FWorldContext();
	WorldContext->SetCurrentWorld(NewWorldContext->GetWorld());
}

UObject* UGBWFlowMoveObject_Base::GetWorldContextObject()
{
	if (WorldContext && WorldContext->World())
	{
		return WorldContext->World();
	}
	
	WorldContext = new FWorldContext();
	WorldContext->SetCurrentWorld(GWorld->GetWorld());
	return GWorld->GetWorld();
}

UWorld* UGBWFlowMoveObject_Base::GetWorld() const
{
	if (UWorld* TheWorld = GWorld->GetWorld())
	{
		if (TheWorld->WorldType == EWorldType::Type::Game || TheWorld->WorldType == EWorldType::Type::PIE)
		{
			return TheWorld;
		}
	}
	return nullptr;
}

FGBWFlowMoveRMSControllerInitInfo::FGBWFlowMoveRMSControllerInitInfo(const FGBWFlowMoveState& FlowMoveState)
{
	BeginFlowMoveState = FlowMoveState;
	BeginTransform = FlowMoveState.OwnerCharacter->GetTransform();
	BeginVelocity = FlowMoveState.OwnerCharacter->GetVelocity();
}

void UGBWFlowMovePerceptron_Base::SetCache(FGBWFlowMoveScene TheFlowMoveSceneCache)
{
	bHasCache = true;
	FlowMoveSceneCache = TheFlowMoveSceneCache;
}

bool UGBWFlowMovePerceptron_Base::GetCache(FGBWFlowMoveScene& Result, const bool bConsume)
{
	const bool bIsGet = bHasCache;
	Result = FlowMoveSceneCache;
	if (bConsume)
	{
		bHasCache = false;
	}
	return bIsGet;
}

bool UGBWFlowMovePerceptron_Base::GetPerceptionResult_Implementation(FGBWFlowMoveScene& FlowMoveScene,
	EGBWFlowMoveEventType EventType, ACharacter* OwnerCharacter, UGBWFlowMoveComponent* FlowMoveComponent)
{
	return false;
}

bool UGBWFlowMovePerceptron_Base::GetThePerceptionResult(FGBWFlowMoveScene& FlowMoveScene,
                                                         const EGBWFlowMoveEventType EventType, ACharacter* OwnerCharacter, UGBWFlowMoveComponent* FlowMoveComponent)
{
	if (EventType == EGBWFlowMoveEventType::End)
	{
		bIsActive = false;
	}
	else
	{
		bIsActive = true;
	}

	const bool Result = GetPerceptionResult(FlowMoveScene, EventType, OwnerCharacter, FlowMoveComponent);
	SetCache(FlowMoveScene);
	return Result;
}

void UGBWFlowMovePerceptron_Base::CheckCopyCache()
{
	TArray<UObject*> Keys;
	CopyCache.GetKeys(Keys);
	for (const auto K:Keys)
	{
		if (!K)
		{
			CopyCache.Remove(K);
		}
	}
}

UGBWFlowMovePerceptron_Base* UGBWFlowMovePerceptron_Base::GetCopy(UObject* Outer)
{
	CheckCopyCache();
	UGBWFlowMovePerceptron_Base* Res;
	UObject* TheOuter = Outer;
	if (!TheOuter)
	{
		TheOuter = this->GetOuter();
	}
	if (CopyCache.Find(TheOuter))
	{
		Res = CopyCache.FindRef(TheOuter);
	}
	else
	{
		Res = Cast<UGBWFlowMovePerceptron_Base>(DuplicateObject<UObject>(this, TheOuter));
		Res->Guid = FGuid::NewGuid();
		CopyCache.Add(TheOuter,Res);
	}
	return Res;
}

void UGBWFlowMoveScript_Base::Update(EGBWFlowMoveEventType EventType,EFlowMoveNetworkLocationType NetworkLocation, bool bIsLocalOwn,
                                     UGBWFlowMoveComponent* FlowMoveComponent)
{
	OnUpdate(EventType, NetworkLocation, bIsLocalOwn, FlowMoveComponent);
}

void UGBWFlowMoveScript_Base::CheckCopyCache()
{
	TArray<UObject*> Keys;
	CopyCache.GetKeys(Keys);
	for (const auto K:Keys)
	{
		if (!K)
		{
			CopyCache.Remove(K);
		}
	}
}

UGBWFlowMoveScript_Base* UGBWFlowMoveScript_Base::GetCopy(UObject* Outer)
{
	CheckCopyCache();
	UGBWFlowMoveScript_Base* Res;
	UObject* TheOuter = Outer;
	if (!TheOuter)
	{
		TheOuter = this->GetOuter();
	}
	if (CopyCache.Find(TheOuter))
	{
		Res = CopyCache.FindRef(TheOuter);
	}
	else
	{
		Res = Cast<UGBWFlowMoveScript_Base>(DuplicateObject<UObject>(this, TheOuter));
		Res->Guid = FGuid::NewGuid();
		CopyCache.Add(TheOuter,Res);
	}
	return Res;
}

USceneComponent* UGBWFlowMoveGetFocusActor_Base::OnGetFocusActor_Implementation(
	UGBWFlowMoveComponent* FlowMoveComponent)
{
	return nullptr;
}

USceneComponent* UGBWFlowMoveGetFocusActor_Base::GetFocusActor(UGBWFlowMoveComponent* FlowMoveComponent)
{
	if (FlowMoveComponent && FlowMoveComponent->IsLocalOwn())
	{
		return OnGetFocusActor(FlowMoveComponent);
	}
	return nullptr;
}

bool UGBWFlowMoveGetFocusActor_Base::TestVisibility(AActor* StartActor, AActor* EndActor,
	FGBWFlowMoveTraceSetting TraceSetting, FVector& TouchPoint)
{
	if (!StartActor || !EndActor)
	{
		return false;
	}

	FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
	CollisionQueryParams.bTraceComplex = false;
	CollisionQueryParams.AddIgnoredActor(StartActor);
	
	TArray<FHitResult> OutHits;
	FHitResult HitRes = UGBWFlowMoveFuncLib::LineTrace(
		StartActor->GetWorld(),
		OutHits,
		StartActor->GetActorLocation(),
		EndActor->GetActorLocation(),
		TraceSetting,
		CollisionQueryParams);

	if (HitRes.IsValidBlockingHit())
	{
		if (HitRes.GetActor() == EndActor)
		{
			TouchPoint = HitRes.ImpactPoint;
			return true;
		}
	}

	return false;
}

bool UGBWFlowMoveActionTree::IsActionValid() const
{
	return ActionAnim.IsAnimValid();
}

void UGBWFlowMoveBrain_Base::OnFlowMoveEvent_Implementation(ACharacter* OwnerCharacter,
                                                            UGBWFlowMoveComponent* FlowMoveComponent, FGBWFlowMoveState FlowMoveState, const FFlowMoveEvent& FlowMoveEvent)
{}

void UGBWFlowMoveBrain_Base::OnFMEvent(ACharacter* OwnerCharacter, UGBWFlowMoveComponent* FlowMoveComponent,
	FGBWFlowMoveState FlowMoveState, const FFlowMoveEvent& FlowMoveEvent)
{
	OnFlowMoveEvent(OwnerCharacter, FlowMoveComponent, FlowMoveState, FlowMoveEvent);
}

void UGBWFlowMoveBrain_Base::GetFlowMoveIsActive_Implementation(
	ACharacter* OwnerCharacter,
	UGBWFlowMoveComponent* FlowMoveComponent,
	float DeltaTime,
	FGBWFlowMoveState FlowMoveState,
	bool& bIsActive,
	bool& WaitForCurrentActionFinished)
{
	bIsActive = false;
	WaitForCurrentActionFinished = true;
}
//FM 应该是Flow Move 的意思
void UGBWFlowMoveBrain_Base::GetFMIsActive(
	ACharacter* OwnerCharacter,
	UGBWFlowMoveComponent* FlowMoveComponent,
	float DeltaTime,
	FGBWFlowMoveState FlowMoveState,
	bool& bIsActive,
	bool& WaitForCurrentActionFinished)
{
	GetFlowMoveIsActive(
        OwnerCharacter,
        FlowMoveComponent,
        DeltaTime,
        FlowMoveState,
        bIsActive,
        WaitForCurrentActionFinished);
}

void UGBWFlowMoveBrain_Base::GetMoveVector_Implementation(ACharacter* OwnerCharacter,
	UGBWFlowMoveComponent* FlowMoveComponent, float DeltaTime, FGBWFlowMoveState FlowMoveState, bool& bIsGet,
	FVector& MoveVector)
{
	bIsGet = false;
    MoveVector = FVector::ZeroVector;
}

void UGBWFlowMoveBrain_Base::GetFMMoveVector(ACharacter* OwnerCharacter, UGBWFlowMoveComponent* FlowMoveComponent,
	float DeltaTime, FGBWFlowMoveState FlowMoveState, bool& bIsGet, FVector& MoveVector)
{
	GetMoveVector(
        OwnerCharacter,
        FlowMoveComponent,
        DeltaTime,
        FlowMoveState,
        bIsGet,
        MoveVector);
}

void UGBWFlowMoveBrain_Base::GetControlVector_Implementation(ACharacter* OwnerCharacter,
	UGBWFlowMoveComponent* FlowMoveComponent, float DeltaTime, FGBWFlowMoveState FlowMoveState, bool& bIsGet,
	FVector& ControlVector)
{
	bIsGet = false;
    ControlVector = FVector::ZeroVector;
}

void UGBWFlowMoveBrain_Base::GetFMControlVector(ACharacter* OwnerCharacter, UGBWFlowMoveComponent* FlowMoveComponent,
	float DeltaTime, FGBWFlowMoveState FlowMoveState, bool& bIsGet, FVector& ControlVector)
{
	GetControlVector(
        OwnerCharacter,
        FlowMoveComponent,
        DeltaTime,
        FlowMoveState,
        bIsGet,
        ControlVector);
}

bool FGBWFlowMoveTraceSetting::IsMetScreeningConditions(const AActor* InActor)
{
	if (!InActor)
	{
		return false;
	}
	if (!UseScreeningConditions
		||(ActorWithClass.IsEmpty() && ActorWithTag.IsEmpty()))
	{
		return true;
	}

	bool bIsMetActorClass = false;
	bool bIsMetActorTag = false;
	for (auto Class: ActorWithClass)
	{
		if (IsValid(Class) && InActor->GetClass()->IsChildOf(Class))
		{
			bIsMetActorClass = true;
			break;
		}
	}
	for (const auto Tag: ActorWithTag)
	{
		if (InActor->ActorHasTag(Tag))
		{
			bIsMetActorTag = true;
			break;
		}
	}

	return bIsMetActorClass || bIsMetActorTag;
}

bool FGBWPerceptionResultItem::EqualTo(const FGBWPerceptionResultItem& OtherPerceptionResultItem) const
{
	if (Type != OtherPerceptionResultItem.Type)
	{
		return false;
	}

	switch (Type)
	{
	case EGBWPerceptionResultItemType::Bool: return BValue==OtherPerceptionResultItem.BValue;
	case EGBWPerceptionResultItemType::Float: return FValue==OtherPerceptionResultItem.FValue;
	case EGBWPerceptionResultItemType::Vector: return VValue==OtherPerceptionResultItem.VValue;
	case EGBWPerceptionResultItemType::Transform: return TValue.Equals(OtherPerceptionResultItem.TValue);
	case EGBWPerceptionResultItemType::String: return SValue==OtherPerceptionResultItem.SValue;
	case EGBWPerceptionResultItemType::GamePlayTag: return GTValue==OtherPerceptionResultItem.GTValue;
	case EGBWPerceptionResultItemType::Actor: return AValue==OtherPerceptionResultItem.AValue;
	case EGBWPerceptionResultItemType::Object: return OValue==OtherPerceptionResultItem.OValue;
	case EGBWPerceptionResultItemType::MovementMode: return MovementModeValue==OtherPerceptionResultItem.MovementModeValue;
	default: ;
	}

	return false;
}

bool FGBWPerceptionResult::IsValid() const
{
	return Keys.Num()==Values.Num();
}

bool FGBWPerceptionResult::EqualTo(FGBWPerceptionResult OtherPerceptionResult)
{
	if (Keys.Num() != OtherPerceptionResult.Keys.Num())
	{
		return false;
	}

	for (int i=0; i<Keys.Num()-1; i++)
	{
		if (Keys[i] != OtherPerceptionResult.Keys[i]
			|| Values[i].EqualTo(OtherPerceptionResult.Values[i]))
		{
			return false;
		}
	}

	return true;
}

void FGBWPerceptionResult::AddBool(FName Key, bool Value)
{
	const int Index = Keys.Find(Key);
	if (Index != INDEX_NONE)
	{
		Values[Index] = {Value};
	}
	else
	{
		Keys.Add(Key); Values.Add({Value});
	}
}

void FGBWPerceptionResult::AddFloat(FName Key, float Value)
{
	const int Index = Keys.Find(Key);
	if (Index != INDEX_NONE)
	{
		Values[Index] = {Value};
	}
	else
	{
		Keys.Add(Key); Values.Add({Value});
	}
}

void FGBWPerceptionResult::AddVector(FName Key, FVector Value)
{
	const int Index = Keys.Find(Key);
	if (Index != INDEX_NONE)
	{
		Values[Index] = {Value};
	}
	else
	{
		Keys.Add(Key); Values.Add({Value});
	}
}

void FGBWPerceptionResult::AddTransform(FName Key, FTransform Value)
{
	const int Index = Keys.Find(Key);
	if (Index != INDEX_NONE)
	{
		Values[Index] = {Value};
	}
	else
	{
		Keys.Add(Key); Values.Add({Value});
	}
}

void FGBWPerceptionResult::AddString(FName Key, FString Value)
{
	const int Index = Keys.Find(Key);
	if (Index != INDEX_NONE)
	{
		Values[Index] = {Value};
	}
	else
	{
		Keys.Add(Key); Values.Add({Value});
	}
}

void FGBWPerceptionResult::AddGameplayTag(FName Key, FGameplayTag Value)
{
	const int Index = Keys.Find(Key);
	if (Index != INDEX_NONE)
	{
		Values[Index] = {Value};
	}
	else
	{
		Keys.Add(Key); Values.Add({Value});
	}
}

void FGBWPerceptionResult::AddActor(FName Key, AActor* Value)
{
	const int Index = Keys.Find(Key);
	if (Index != INDEX_NONE)
	{
		Values[Index] = {Value};
	}
	else
	{
		Keys.Add(Key); Values.Add({Value});
	}
}

void FGBWPerceptionResult::AddObject(FName Key, UObject* Value)
{
	const int Index = Keys.Find(Key);
	if (Index != INDEX_NONE)
	{
		Values[Index] = {Value};
	}
	else
	{
		Keys.Add(Key); Values.Add({Value});
	}
}

void FGBWPerceptionResult::AddMovementMode(FName Key, EMovementMode Value)
{
	const int Index = Keys.Find(Key);
	if (Index != INDEX_NONE)
	{
		Values[Index] = {Value};
	}
	else
	{
		Keys.Add(Key); Values.Add({Value});
	}
}

void FGBWPerceptionResult::GetBool(FName Key, bool& IsGet, bool& Value) const
{
	const int Index = Keys.Find(Key);
	IsGet = (Index != INDEX_NONE);
	if (IsGet)
	{
		if (const FGBWPerceptionResultItem Item = Values[Index];
			Item.Type == EGBWPerceptionResultItemType::Bool)
		{
			Value = Item.BValue;
		}
	}
}

void FGBWPerceptionResult::GetFloat(FName Key, bool& IsGet, float& Value) const
{
	const int Index = Keys.Find(Key);
	IsGet = (Index != INDEX_NONE);
	if (IsGet)
	{
		if (const FGBWPerceptionResultItem Item = Values[Index];
			Item.Type == EGBWPerceptionResultItemType::Float)
		{
			Value = Item.FValue;
		}
	}
}

void FGBWPerceptionResult::GetVector(FName Key, bool& IsGet, FVector& Value) const
{
	const int Index = Keys.Find(Key);
	IsGet = (Index != INDEX_NONE);
	if (IsGet)
	{
		if (const FGBWPerceptionResultItem Item = Values[Index];
			Item.Type == EGBWPerceptionResultItemType::Vector)
		{
			Value = Item.VValue;
		}
	}
}

void FGBWPerceptionResult::GetTransform(FName Key, bool& IsGet, FTransform& Value) const
{
	const int Index = Keys.Find(Key);
	IsGet = (Index != INDEX_NONE);
	if (IsGet)
	{
		if (const FGBWPerceptionResultItem Item = Values[Index];
			Item.Type == EGBWPerceptionResultItemType::Transform)
		{
			Value = Item.TValue;
		}
	}
}

void FGBWPerceptionResult::GetString(FName Key, bool& IsGet, FString& Value) const
{
	const int Index = Keys.Find(Key);
	IsGet = (Index != INDEX_NONE);
	if (IsGet)
	{
		if (const FGBWPerceptionResultItem Item = Values[Index];
			Item.Type == EGBWPerceptionResultItemType::String)
		{
			Value = Item.SValue;
		}
	}
}

void FGBWPerceptionResult::GetGameplayTag(FName Key, bool& IsGet, FGameplayTag& Value) const
{
	const int Index = Keys.Find(Key);
	IsGet = (Index != INDEX_NONE);
	if (IsGet)
	{
		if (const FGBWPerceptionResultItem Item = Values[Index];
			Item.Type == EGBWPerceptionResultItemType::GamePlayTag)
		{
			Value = Item.GTValue;
		}
	}
}

void FGBWPerceptionResult::GetActor(FName Key, bool& IsGet, AActor*& Value) const
{
	const int Index = Keys.Find(Key);
	IsGet = (Index != INDEX_NONE);
	if (IsGet)
	{
		if (const FGBWPerceptionResultItem Item = Values[Index];
			Item.Type == EGBWPerceptionResultItemType::Actor)
		{
			Value = Item.AValue;
		}
	}
}

void FGBWPerceptionResult::GetObject(FName Key, bool& IsGet, UObject*& Value) const
{
	const int Index = Keys.Find(Key);
	IsGet = (Index != INDEX_NONE);
	if (IsGet)
	{
		if (const FGBWPerceptionResultItem Item = Values[Index];
			Item.Type == EGBWPerceptionResultItemType::Object)
		{
			Value = Item.OValue;
		}
	}
}

void FGBWPerceptionResult::GetMovementMode(FName Key, bool& IsGet, TEnumAsByte<EMovementMode>& Value) const
{
	const int Index = Keys.Find(Key);
	IsGet = (Index != INDEX_NONE);
	if (IsGet)
	{
		if (const FGBWPerceptionResultItem Item = Values[Index];
			Item.Type == EGBWPerceptionResultItemType::MovementMode)
		{
			Value = Item.MovementModeValue;
		}
	}
}

bool FGBWFlowMoveAnimSetting::EqualTo(const FGBWFlowMoveAnimSetting& Other) const
{
	if (AnimType != Other.AnimType
		|| ExcludedMeshTag != Other.ExcludedMeshTag
		|| MeshTag != Other.MeshTag)
	{
		return false;
	}
	if (AnimType == EFlowMoveAnimType::Montage)
	{
		return Montage == Other.Montage
			&& bIsLoop == Other.bIsLoop
			&& RandomSections == Other.RandomSections;
	}
	else if (AnimType == EFlowMoveAnimType::BlendSpace)
	{
		return AnimBlendSpace == Other.AnimBlendSpace;
	}
	return false;
}

bool FGBWFlowMoveAnimSetting::IsAnimValid() const
{
	if (AnimType == EFlowMoveAnimType::Montage)
    {
        return Montage!= nullptr;
    }
    else if (AnimType == EFlowMoveAnimType::BlendSpace)
    {
        return AnimBlendSpace!= nullptr;
    }
    return false;
}

void FGBWFlowMoveAnimSetting::Reset()
{
	bIsPlayed = false;
	AnimType = EFlowMoveAnimType::Montage;
	MeshTag = NAME_None;
	Montage = nullptr;
	AnimBlendSpace = nullptr;
	bIsLoop = false;
	RandomSections = {};
	ExcludedMeshTag = {};
}

FString FGBWFlowMoveAnimSetting::GetDebugStr() const
{
	FString AnimNowStr = "IsPlayed : (" + (bIsPlayed? FString("True"):FString("False")) + ")  ";
	AnimNowStr = AnimNowStr + "AnimType : (" + StaticEnum<EFlowMoveAnimType>()->GetNameStringByIndex( ( int32 ) AnimType) + ")  ";
	AnimNowStr = AnimNowStr + "MeshTag : (" + MeshTag.ToString() + ")  ";
	AnimNowStr = AnimNowStr + "ExcludedMeshTag : (";
	for (auto Item : ExcludedMeshTag)
	{
		AnimNowStr = AnimNowStr + Item.ToString() + " & ";
	}
	AnimNowStr = AnimNowStr + ")  ";
	if (AnimType == EFlowMoveAnimType::Montage)
	{
		AnimNowStr = AnimNowStr + "Montage : (" + (Montage? Montage->GetName() : "None") + ")  ";
		AnimNowStr = AnimNowStr + "bIsLoop : (" + (bIsLoop? FString("True"):FString("False")) + ")  ";
		AnimNowStr = AnimNowStr + "RandomSections : (";
		for (auto Item : RandomSections)
		{
			AnimNowStr = AnimNowStr + Item.ToString() + " & ";
		}
		AnimNowStr = AnimNowStr + ")  ";
	}
	else
	{
		AnimNowStr = AnimNowStr + "AnimBlendSpace : (" + (AnimBlendSpace? AnimBlendSpace->GetName() : "None")+ ")  ";
	}

	return AnimNowStr;
}

FFlowMoveEvent::FFlowMoveEvent(EFlowMoveEventType TheEventType)
{
	EventType = TheEventType;
}

FFlowMoveEvent::FFlowMoveEvent(EFlowMoveEventType TheEventType, FGameplayTag InActionTag)
{
	EventType = TheEventType;
	if (EventType == EFlowMoveEventType::OnActionStart
		||EventType == EFlowMoveEventType::OnActionUpdate
		||EventType == EFlowMoveEventType::OnActionEnd)
	{
		ActionTag = InActionTag;
	}
}

FFlowMoveEvent::FFlowMoveEvent(EMovementMode TheNewMovementMode)
{
	EventType = EFlowMoveEventType::OnMovementModeChange;
	NewMovementMode = TheNewMovementMode;
}

FFlowMoveEvent::FFlowMoveEvent(EFlowMoveCharacterViewMode TheNewViewMode)
{
	EventType = EFlowMoveEventType::OnViewModeChange;
	NewViewMode = TheNewViewMode;
}

FFlowMoveEvent::FFlowMoveEvent(USceneComponent* TheNewFocusActorComponent)
{
	EventType = EFlowMoveEventType::OnFocusActorChange;
	NewFocusActorComponent = TheNewFocusActorComponent;
}

bool FFlowMoveEvent::EqualTo(const FFlowMoveEvent& OtherEvent) const
{
	if (EventType != OtherEvent.EventType)
	{
		return false;
	}
		
	if (EventType == EFlowMoveEventType::OnActionStart || EventType == EFlowMoveEventType::OnActionEnd || EventType == EFlowMoveEventType::OnActionUpdate)
	{
		return ActionTag==OtherEvent.ActionTag
		|| ActionTag.MatchesTag(OtherEvent.ActionTag)
		|| OtherEvent.ActionTag.MatchesTag(ActionTag);
	}

	if (EventType == EFlowMoveEventType::OnMovementModeChange)
	{
		return NewMovementMode == OtherEvent.NewMovementMode;
	}

	if (EventType == EFlowMoveEventType::OnViewModeChange)
	{
		return NewViewMode == OtherEvent.NewViewMode;
	}

	if (EventType == EFlowMoveEventType::OnFocusActorChange)
	{
		return NewFocusActorComponent == OtherEvent.NewFocusActorComponent;
	}

	return true;
}

FString FFlowMoveEvent::GetDebugString() const
{
	return FString::Printf(TEXT("%s %s %s"),
	                       *(StaticEnum<EFlowMoveEventType>()->GetNameStringByIndex( ( int32 ) EventType)),
	                       *ActionTag.ToString(),
	                       *(StaticEnum<EMovementMode>()->GetNameStringByIndex( ( int32 ) NewMovementMode)));
}

bool FGBWFlowMoveActionLockState::IsLock(FGameplayTag ForActionTag) const
{
	return ForActionTag.IsValid() && ActionToLock.Matches(ForActionTag.GetSingleTagContainer());
}

bool FGBWFlowMoveControlParam::HasActiveAnimCurveState(FName InName) const
{
	for (auto Element : AnimPlayStateSet.CurveValueState)
	{
		if (Element.CurveName == InName)
		{
			return true;
		}
	}
	return false;
}

bool FGBWFlowMoveFloorRoofScene::EqualTo(const FGBWFlowMoveFloorRoofScene& Other) const
{
	return Floor == Other.Floor
		&& FloorLedge_Forward == Other.FloorLedge_Forward
		&& FloorLedge_Backward == Other.FloorLedge_Backward
		&& Roof == Other.Roof
		&& RoofLedge_Forward == Other.RoofLedge_Forward
		&& RoofLedge_Backward == Other.RoofLedge_Backward
		&& HeightFromGround == Other.HeightFromGround
		&& HeightFromRoof == Other.HeightFromRoof;
}

bool FGBWFlowMoveBlockWallScene::EqualTo(const FGBWFlowMoveBlockWallScene& Other) const
{
	return WallPoint == Other.WallPoint
		&& WallLedge_Up == Other.WallLedge_Up
		&& WallLedge_Down == Other.WallLedge_Down
		&& WallLedge_Left == Other.WallLedge_Left
		&& WallLedge_Right == Other.WallLedge_Right
		&& DistanceFromActor == Other.DistanceFromActor;
}

bool FGBWFlowMoveBlockWallScene::IsWall() const
{
	return !WallPoint.IsZero();
}

bool FGBWFlowMoveBlockWallScene::IsLedge_Up() const
{
	return !WallLedge_Up.IsZero();
}

bool FGBWFlowMoveBlockWallScene::IsLedge_Down() const
{
	return !WallLedge_Down.IsZero();
}

bool FGBWFlowMoveBlockWallScene::IsLedge_Left() const
{
	return !WallLedge_Left.IsZero();
}

bool FGBWFlowMoveBlockWallScene::IsLedge_Right() const
{
	return !WallLedge_Right.IsZero();
}

FGBWFlowMovePointScene::FGBWFlowMovePointScene(ACharacter* InCharacter, const FVector& InPoint, bool bFindLedge, float LedgeTraceDistance, FGBWFlowMoveTraceSetting TraceSetting)
{
	Point = InPoint;

	if (InCharacter)
	{
		/*
		FVector Floor = FVector::ZeroVector;
		FVector Roof = FVector::ZeroVector;
		float HeightFromGround = 0.0f;
		float HeightFromRoof = 0.0f;*/

		const FVector ActorLocation = InCharacter->GetActorLocation();
		const FVector RefInPoint = UKismetMathLibrary::InverseTransformLocation(InCharacter->GetTransform(),InPoint);

		//-----------------------------------------
		Height = RefInPoint.Z;
		Right = RefInPoint.Y;
		Forward = RefInPoint.X;

		//-----------------------------------------
		Angle = (InPoint - InCharacter->GetActorLocation()).Rotation().Pitch;

		//-----------------------------------------
		FVector A = UKismetMathLibrary::ProjectVectorOnToPlane(
		InPoint-ActorLocation,InCharacter->GetActorForwardVector());
		A.Normalize();
		FVector B = UKismetMathLibrary::ProjectVectorOnToPlane(InCharacter->GetActorUpVector(),InCharacter->GetActorForwardVector());
		B.Normalize();
		const float DirectionAngle = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(A,B));

		const FTransform T = InCharacter->GetActorTransform();
		const FVector A2 = UKismetMathLibrary::InverseTransformDirection(T,A);
		const FVector B2 = UKismetMathLibrary::InverseTransformDirection(T,B);
		const bool bIsRight = UKismetMathLibrary::CrossProduct2D(
			FVector2D(A2.Y, A2.Z),
			FVector2D(B2.Y, B2.Z)
		)>=0;

		Direction = DirectionAngle * (bIsRight? 1 : -1);
		if (UKismetMathLibrary::InRange_FloatFloat(Direction, -22.5f, 22.5f))
		{
			SceneDirectionMark = EGBWFlowMoveDirectionType::MU;
		}
		else if (UKismetMathLibrary::InRange_FloatFloat(Direction, 22.5f, 67.5f))
		{
			SceneDirectionMark = EGBWFlowMoveDirectionType::RU;
		}
		else if (UKismetMathLibrary::InRange_FloatFloat(Direction, 67.5f, 112.5f))
		{
			SceneDirectionMark = EGBWFlowMoveDirectionType::RM;
		}
		else if (UKismetMathLibrary::InRange_FloatFloat(Direction, 112.5f, 157.5f))
		{
			SceneDirectionMark = EGBWFlowMoveDirectionType::RD;
		}
		else if (UKismetMathLibrary::InRange_FloatFloat(Direction, 157.5f, 180.0f)
			|| UKismetMathLibrary::InRange_FloatFloat(Direction, -180.0f, -157.5f))
		{
			SceneDirectionMark = EGBWFlowMoveDirectionType::MD;
		}
		else if (UKismetMathLibrary::InRange_FloatFloat(Direction, -157.5f, -112.5f))
		{
			SceneDirectionMark = EGBWFlowMoveDirectionType::LD;
		}
		else if (UKismetMathLibrary::InRange_FloatFloat(Direction, -112.5f, -67.5f))
		{
			SceneDirectionMark = EGBWFlowMoveDirectionType::LM;
		}
		else if (UKismetMathLibrary::InRange_FloatFloat(Direction, -67.5f, -22.5f))
		{
			SceneDirectionMark = EGBWFlowMoveDirectionType::LU;
		}
		else
		{
			SceneDirectionMark = EGBWFlowMoveDirectionType::MM;
		}

		//-----------------------------------------
		bool bIsFloor; FHitResult OutHit_Floor; bool bIsRoof; FHitResult OutHit_Roof;
		FGBWFlowMoveFloorRoofScene FloorAndRoofRes;
		UGBWFlowMoveFuncLib::FindFloorAndRoof(InCharacter, Point, 10000.0f, 10000.0f, bFindLedge, LedgeTraceDistance, TraceSetting,
											  bIsFloor, OutHit_Floor, bIsRoof, OutHit_Roof,FloorAndRoofRes);

		FloorAndRoof = FloorAndRoofRes;

		if (ActorLocation == InPoint)
		{
			FVector FootLocation = InCharacter->GetCapsuleComponent()->GetComponentLocation() - FVector(0,0,InCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
			FVector HeadLocation = InCharacter->GetCapsuleComponent()->GetComponentLocation() + FVector(0,0,InCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
    	
			FloorAndRoof.HeightFromGround = bIsFloor? (FloorAndRoof.Floor-FootLocation).Length() : 999999.0f;
			FloorAndRoof.HeightFromRoof = bIsRoof? (FloorAndRoof.Roof-HeadLocation).Length() : 999999.0f;
		}
		else
		{
			FloorAndRoof.HeightFromGround = bIsFloor? (FloorAndRoof.Floor-InPoint).Length() : 999999.0f;
			FloorAndRoof.HeightFromRoof = bIsRoof? (FloorAndRoof.Roof-InPoint).Length() : 999999.0f;
		}
	}
}

bool FGBWFlowMovePointScene::EqualTo(const FGBWFlowMovePointScene& Other) const
{
	return Point == Other.Point &&
		Height == Other.Height &&
		Right == Other.Right &&
		Forward == Other.Forward &&
		Angle == Other.Angle &&
		Direction == Other.Direction &&
		SceneDirectionMark == Other.SceneDirectionMark &&
		FloorAndRoof.EqualTo(Other.FloorAndRoof);
}

FGBWFlowMoveScene::FGBWFlowMoveScene(ACharacter* InCharacter,
		FName InSceneType,
		AActor* InTargetActor,
		float InSlope,
		const FVector& InTargetPoint,
		const FVector& LeftTargetPoint,
		const FVector& RightTargetPoint,
		const FGBWFlowMoveTraceSetting& TraceSetting,
		float LedgeTraceDistance)
{
	SceneType = InSceneType;
	Slope = InSlope;
	TargetActor = InTargetActor;

	ActorLocationScene = FGBWFlowMovePointScene(InCharacter, InCharacter->GetActorLocation(), true, LedgeTraceDistance, TraceSetting);
	TargetLocationScene = FGBWFlowMovePointScene(InCharacter, InTargetPoint, false, LedgeTraceDistance, TraceSetting);
	LeftLocationScene = FGBWFlowMovePointScene(InCharacter, LeftTargetPoint, false, LedgeTraceDistance, TraceSetting);
	RightLocationScene = FGBWFlowMovePointScene(InCharacter, RightTargetPoint, false, LedgeTraceDistance, TraceSetting);
}

bool FGBWFlowMoveScene::IsValid() const
{
	return PerceptionResult.IsValid();
}

bool FGBWFlowMoveScene::EqualTo(const FGBWFlowMoveScene& OtherScene)
{
	return SceneType == OtherScene.SceneType
		&& TargetActor == OtherScene.TargetActor
		&& Slope == OtherScene.Slope
		&& PerceptionResult.EqualTo(OtherScene.PerceptionResult)
		&& ActorLocationScene.EqualTo(OtherScene.ActorLocationScene)
		&& TargetLocationScene.EqualTo(OtherScene.TargetLocationScene)
		&& LeftLocationScene.EqualTo(OtherScene.LeftLocationScene)
		&& RightLocationScene.EqualTo(OtherScene.RightLocationScene);
}

bool FGBWFlowMoveScene::CheckSceneChange(const FGBWFlowMoveScene& OtherScene, FGBWFlowMoveSceneChangeInfo& ChangeInfo)
{
	const bool bIsChange = !EqualTo(OtherScene);
	ChangeInfo.SceneType = SceneType != OtherScene.SceneType;
	ChangeInfo.ActorLocationScene = !ActorLocationScene.EqualTo(OtherScene.ActorLocationScene);
	ChangeInfo.TargetLocationScene = !TargetLocationScene.EqualTo(OtherScene.TargetLocationScene);
	ChangeInfo.LeftLocationScene = !LeftLocationScene.EqualTo(OtherScene.LeftLocationScene);
	ChangeInfo.RightLocationScene = !RightLocationScene.EqualTo(OtherScene.RightLocationScene);
	ChangeInfo.Slope = Slope != OtherScene.Slope;
	ChangeInfo.PerceptionResult = !PerceptionResult.EqualTo(OtherScene.PerceptionResult);

	return bIsChange;
}

bool FGBWFlowMoveScene::GetPlaneConstraintSetting(FVector& PlaneNormal, FVector& PlaneOrigin) const
{
	if (!IsValid())
	{
		return false;
	}
	FVector NormalVector = TargetLocationScene.Point - ActorLocationScene.Point;
	NormalVector.Z = 0.0f;
	NormalVector.Normalize();
	NormalVector = FVector(NormalVector.Y, -NormalVector.X, 0.0f);

	PlaneNormal = NormalVector;
	PlaneOrigin = ActorLocationScene.Point;

	return true;
}

bool FGBWFlowMoveFloatScope::InScope(const float InValue) const
{
	if (Min.Type==EGBWFlowMoveValueScopePointType::Contain && !(InValue>=Min.Value))
	{
		return false;
	}
	if (Min.Type==EGBWFlowMoveValueScopePointType::NotContain && !(InValue>Min.Value))
	{
		return false;
	}
	if (Max.Type==EGBWFlowMoveValueScopePointType::Contain && !(InValue<=Max.Value))
	{
		return false;
	}
	if (Max.Type==EGBWFlowMoveValueScopePointType::NotContain && !(InValue<Max.Value))
	{
		return false;
	}
	return true;
}

bool FGBWFlowMoveVectorScope::InScope(const FVector InValue) const
{
	return X_Scope.InScope(InValue.X) && Y_Scope.InScope(InValue.Y) && Z_Scope.InScope(InValue.Z);
}

bool FGBWFlowMoveTransformScope::InScope(const FTransform InValue) const
{
	return Location_Scope.InScope(InValue.GetLocation())
		&& Rotation_Scope.InScope({
			InValue.GetRotation().X,InValue.GetRotation().Y,InValue.GetRotation().Z
		})
		&& Scale_Scope.InScope(InValue.GetScale3D());
}
