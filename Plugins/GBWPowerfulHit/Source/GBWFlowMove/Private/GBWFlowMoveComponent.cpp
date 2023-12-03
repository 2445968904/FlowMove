// Copyright GanBowen 2022-2023. All Rights Reserved.

#include "GBWFlowMoveComponent.h"

#include "GBWAnimFuncLib.h"
#include "GBWDebugComponent.h"
#include "GBWFlowMoveFuncLib.h"
#include "GBWFlowMoveInterface.h"
#include "GBWPowerfulAnimInstance.h"
#include "GBWPowerfulToolsFuncLib.h"
#include "GBWPSFuncLib.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "SkeletalMeshCompiler.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimNode_AssetPlayerBase.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS/GBWFlowMoveANS_ActionLock.h"
#include "ANS/GBWFlowMoveANS_MoveControl.h"
#include "RMS/GBWRMSBPFuncLib.h"

// Sets default values for this component's properties
UGBWFlowMoveComponent::UGBWFlowMoveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UGBWFlowMoveComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Parameters;
	Parameters.bIsPushBased = true;
	Parameters.Condition = COND_None;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, TaskState, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, NetWorkActorGuids, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, NetWorkActors, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsAIPlayer, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsAIPlayerInit, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PSStateComponent, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, DefaultViewMode, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CurrentActionTargetSceneInfo, Parameters)
}

void UGBWFlowMoveComponent::BeginPlay()
{
	Super::BeginPlay();
	TaskState.OwnerCharacter = Cast<ACharacter>(GetOwner());
	TaskState.Timer = 0.0f;
	TaskState.bIsActive = false;
	TaskState.bIsStopping = false;
	TaskState.FlowMoveComponent = this;

	if (TaskState.OwnerCharacter)
	{
		TArray<UActorComponent*> SKCArr;
		TaskState.OwnerCharacter->GetComponents(USkeletalMeshComponent::StaticClass(),SKCArr);

		MovementCompMaxAccelerationCache = TaskState.OwnerCharacter->GetCharacterMovement()->GetMaxAcceleration();

		if (FlowMoveBrain)
		{
			SetFlowMoveViewMode(FlowMoveBrain->DefaultViewMode,true);
		}

		bIsConstrainToPlaneCache = TaskState.OwnerCharacter->GetCharacterMovement()->bConstrainToPlane;
		PlaneConstraintNormalCache = TaskState.OwnerCharacter->GetCharacterMovement()->GetPlaneConstraintNormal();
		PlaneConstraintOriginCache = TaskState.OwnerCharacter->GetCharacterMovement()->GetPlaneConstraintOrigin();

		bUseControllerRotationYawCache = TaskState.OwnerCharacter->bUseControllerRotationYaw;
	}

	if (IsInServer())
	{
		TaskState.FlowMoveComponentGuid = FGuid::NewGuid();
		IsAIPlayer();

		PSStateComponent = UGBWPSFuncLib::GetPowerfulStateComponent(TaskState.OwnerCharacter);
		PSStateComponent->bIsDebug = bPS_IsDebug;
		PSStateComponent->StateOperationNum = PS_StateOperationNum;
		PSStateComponent->DebugTextColor = PS_DebugTextColor;
		PSStateComponent->DebugTextColor2 = PS_DebugTextColor2;
		PSStateComponent->bPrintToScreen = bPS_PrintToScreen;
		PSStateComponent->bPrintToLog = bPS_PrintToLog;
		PSStateComponent->SetMaxStateOperationNum(MaxStateOperationNum);

		if (FlowMoveBrain)
		{
			UGBWAnimFuncLib::SetIncludeMainInstance(TaskState.OwnerCharacter,FlowMoveBrain->AnimStateGetType.IncludeMainAnimInstance);
			UGBWAnimFuncLib::SetIncludeLinkedInstance(TaskState.OwnerCharacter,FlowMoveBrain->AnimStateGetType.IncludeLinkedAnimInstance);
			UGBWAnimFuncLib::AddIncludeLinkedInstanceClass(TaskState.OwnerCharacter,FlowMoveBrain->AnimStateGetType.IncludeLinkedAnimInstanceClassSet);
		}
	}
}
void UGBWFlowMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const float RealDeltaTime = DeltaTime;
	
	if (FrameNumber == GFrameCounter )
		return;

	FlowMoveTick(RealDeltaTime);

	TaskState.Timer = TaskState.Timer + RealDeltaTime;
	TaskState.FrameDeltaTime = RealDeltaTime;
	FrameNumber = GFrameCounter;
}

bool UGBWFlowMoveComponent::GetRootMotionParamDirect(
	FTransform& Result,
	float SimulationTime,
	float MovementTickTime,
	const ACharacter& Character,
	const UCharacterMovementComponent& MoveComponent)
{
	Result = TaskState.RMSParamNow;
	return true;
}

void UGBWFlowMoveComponent::OnEvent(const FFlowMoveEvent& FlowMoveEvent)
{
	TaskState.AddFlowMoveEvent(FlowMoveEvent);
	if (FlowMoveBrain)
	{
		FlowMoveBrain->OnFMEvent(TaskState.OwnerCharacter,this,TaskState,FlowMoveEvent);

		if (FlowMoveEvent.EventType == EFlowMoveEventType::OnActionStart
			&& FlowMoveBrain->ActionEventKey_Start.IsValid()
			&& FlowMoveBrain->ActionEventValueKey.IsValid())
		{
			UGBWPSFuncLib::SendPowerfulEvent(
				TaskState.OwnerCharacter,
				UGBWPSFuncLib::AddGTToPE(
				UGBWPSFuncLib::MakePE_Event(FlowMoveBrain->ActionEventKey_Start),
				FlowMoveBrain->ActionEventValueKey,
				FlowMoveEvent.ActionTag));
		}
		else if (FlowMoveEvent.EventType == EFlowMoveEventType::OnActionEnd
			&& FlowMoveBrain->ActionEventKey_End.IsValid()
			&& FlowMoveBrain->ActionEventValueKey.IsValid())
		{
			UGBWPSFuncLib::SendPowerfulEvent(
				TaskState.OwnerCharacter,
				UGBWPSFuncLib::AddGTToPE(
				UGBWPSFuncLib::MakePE_Event(FlowMoveBrain->ActionEventKey_End),
				FlowMoveBrain->ActionEventValueKey,
				FlowMoveEvent.ActionTag));
		}
		else if (FlowMoveEvent.EventType == EFlowMoveEventType::OnActionUpdate
			&& FlowMoveBrain->ActionStateKey.IsValid() && IsInServer())
		{
			UGBWPSFuncLib::SetPSGT(TaskState.OwnerCharacter, FlowMoveBrain->ActionStateKey, FlowMoveEvent.ActionTag);
		}
	}
}

void UGBWFlowMoveComponent::SetInputValueByTargetPoint(const FVector TargetPoint)
{
	SetInputValueByTargetPoint_Server(TargetPoint);
}

void UGBWFlowMoveComponent::SetInputValueByTargetPoint_Server_Implementation(const FVector TargetPoint)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_SetInputValueByTargetPoint");
	if (TaskState.OwnerCharacter)
	{
		FVector ControlVector = TargetPoint - TaskState.OwnerCharacter->GetActorLocation();
		ControlVector.Normalize();
		SetControlVector(ControlVector);
		SetMoveVector(FVector(1,0,0),FlowMoveBrain? FlowMoveBrain->MoveVectorZeroFaultToleranceDuration : 0.1f);
		SetPerceptionVector(FVector(0,0,0));
	}
}

void UGBWFlowMoveComponent::SetMoveVector(FVector NewMoveVector, float ZeroFaultToleranceDuration)
{
	if (IsLocalOwn())
	{
		if (!NewMoveVector.IsNearlyZero() ||
			(NewMoveVector.IsNearlyZero() && TaskState.OwnerCharacter->GetWorld()->GetTimeSeconds() - TaskState.LastMoveVectorTime >= ZeroFaultToleranceDuration))
		{
			TaskState.LastMoveVectorTime = TaskState.OwnerCharacter->GetWorld()->GetTimeSeconds();
			TaskState.LastMoveVector = NewMoveVector;
			
			if (NewMoveVector == TaskState.MoveVector)
			{
				return;
			}
			
			TaskState.SetMoveVector(NewMoveVector);
			SetMoveVector_Server(NewMoveVector);
		}
	}
}
void UGBWFlowMoveComponent::SetMoveVector_Server_Implementation(FVector NewMoveVector)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_SetMoveVector");
	TaskState.SetMoveVector(NewMoveVector);
}

FVector UGBWFlowMoveComponent::GetMoveVector(const bool Consumed)
{
	const FVector r = TaskState.MoveVector;
	return r;
}

void UGBWFlowMoveComponent::SetControlVector(FVector ControlVector)
{
	if (IsLocalOwn())
	{
		if (TaskState.ControlVector == ControlVector)
		{
			return;
		}
		TaskState.ControlVector = ControlVector;
		SetControlVector_Server(ControlVector);
	}
}
void UGBWFlowMoveComponent::SetControlVector_Server_Implementation(FVector ControlVector)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_SetControlVector");
	TaskState.ControlVector = ControlVector;
}

FVector UGBWFlowMoveComponent::GetControlVector()
{
	return TaskState.ControlVector;
}

void UGBWFlowMoveComponent::SetPerceptionVector(FVector NewPerceptionVector)
{
	if (IsLocalOwn())
	{
		if (TaskState.PerceptionVector == NewPerceptionVector)
		{
			return;
		}
		TaskState.PerceptionVector = NewPerceptionVector;
		SetPerceptionVector_Server(NewPerceptionVector);
	}
}
void UGBWFlowMoveComponent::SetPerceptionVector_Server_Implementation(FVector NewPerceptionVector)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_SetPerceptionVector");
	TaskState.PerceptionVector = NewPerceptionVector;
}
FVector UGBWFlowMoveComponent::GetPerceptionVector()
{
	return TaskState.PerceptionVector;
}

void UGBWFlowMoveComponent::GetMoveParamForBlendSpace(UBlendSpace* ForBS, float& Res_Direction, float& Res_Speed, float& PlayRate) const
{
	PlayRate = 1.0f;
	if (!ForBS || !TaskState.OwnerCharacter)
	{
		return;
	}
	
	FVector Velocity = TaskState.OwnerCharacter->GetVelocity();
	Velocity.Z = 0.0f;
	const float Speed = Velocity.Length();
	
	const float D = UGBWFlowMoveFuncLib::GetMoveDirection(TaskState.OwnerCharacter, Velocity, false);
	const FBlendParameter BPX = ForBS->GetBlendParameter(0);
	const FBlendParameter BPY = ForBS->GetBlendParameter(1);
	
	Res_Direction = (D+180.0f)/360.0f * BPX.GetRange() + BPX.Min;
	Res_Speed = Speed/GetMoveControlParam().MoveSpeed / GetActorScale() * BPY.GetRange() + BPY.Min;

	PlayRate = TaskState.MoveControlParam.AnimPlayRateNow;
}

void UGBWFlowMoveComponent::StopFlowMove(bool WaitForCurrentActionFinished)
{
	if (IsFlowMoveActive() && (IsLocalOwn() || IsInServer()))
	{
		if (TaskState.bIsStopping)
		{
			return;
		}
		StopFlowMove_Server(WaitForCurrentActionFinished);
	}
}
void UGBWFlowMoveComponent::StopFlowMove_Server_Implementation(bool WaitForCurrentActionFinished)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_StopFlowMove");
	StopFlowMove_Multicast(WaitForCurrentActionFinished);
}
void UGBWFlowMoveComponent::StopFlowMove_Multicast_Implementation(bool WaitForCurrentActionFinished)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Multicast_StopFlowMove");
	if (WaitForCurrentActionFinished)
	{
		TaskState.bIsStopping = true;
	}
	else
	{
		OnStopEvent();
	}
}

void UGBWFlowMoveComponent::Stop()
{
	Stop_Server();
}
void UGBWFlowMoveComponent::Stop_Server_Implementation()
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_Stop");
	Stop_Multicast();
}
void UGBWFlowMoveComponent::Stop_Multicast_Implementation()
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Multicast_Stop");
	OnStopEvent();
}

void UGBWFlowMoveComponent::ActiveFlowMove()
{
	if (IsInServer() || IsLocalOwn())
	{
		Active_Server();
	}
}
void UGBWFlowMoveComponent::Active_Server_Implementation()
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_Active");
	Active_Multicast();
}
void UGBWFlowMoveComponent::Active_Multicast_Implementation()
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Multicast_Active");
	if (TaskState.OwnerCharacter)
	{
		Active_Imp();
	}
}

void UGBWFlowMoveComponent::Active_Imp()
{
	if (IsFlowMoveActive())
	{
		if (IsFlowMoveStopping())
		{
			if (IsInServer())
			{
				TaskState.bIsStopping = false;
			}
			return;
		}
		return;	
	}
	if (!FlowMoveBrain)
	{
		return;
	}
	
	if (IsInServer())
	{
		if (!TaskState.FlowMoveComponentGuid.IsValid())
		{
			TaskState.FlowMoveComponentGuid = FGuid::NewGuid();
		}
	}
	TaskState.OwnerCharacter = Cast<ACharacter>(GetOwner());
	TaskState.Timer = 0.0f;
	TaskState.FlowMoveComponent = this;
	
	CapsuleHalfHeightCache = TaskState.OwnerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	MeshRelativeLocationCache = TaskState.OwnerCharacter->GetMesh()->GetRelativeLocation();
	CapsuleRadiusCache = TaskState.OwnerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius();

	bUseControllerRotationYawCache = TaskState.OwnerCharacter->bUseControllerRotationYaw;

	if (TaskState.OwnerCharacter && FlowMoveBrain)
	{
		TaskState.OwnerCharacter->bUseControllerRotationYaw = false;
		
		EnsureRMSActivation();
		
		MovementCompMaxAccelerationCache = TaskState.OwnerCharacter->GetCharacterMovement()->GetMaxAcceleration();
		TaskState.OwnerCharacter->GetCharacterMovement()->MaxAcceleration = 0.0f;
		
		TaskState.bIsActive = true;
		TaskState.bIsStopping = false;

		if (IsInServer())
		{
			for (const auto Item : FlowMoveBrain->FlowMovePerceptronSet)
            {
            	if (Item.Value && Item.Value->bIsDefaultActive)
            	{
            		ActivePerceptron(Item.Key);
            	}
            }

			for (const auto Item :  FlowMoveBrain->FlowMoveScriptSet)
			{
				if (Item.Value && Item.Value->bIsDefaultActive)
				{
					ActiveScript(Item.Key);
				}
			}
		}
		
		UpdateMoveControlParam(TaskState.FrameDeltaTime);
		
		OnActiveEvent();
	}
	else
	{
		OnActiveFailed.Broadcast(TaskState);
		OnEvent(FFlowMoveEvent(EFlowMoveEventType::OnActiveFailed));
		StopFlowMove(false);
	}
	bIsNetInit = true;
}

void UGBWFlowMoveComponent::SetActive_Server_Implementation(bool NewActive)
{
	SetActive_Multicast(NewActive);
}

void UGBWFlowMoveComponent::SetActive_Multicast_Implementation(bool NewActive)
{
	TaskState.bIsActive = NewActive;
}
//是否收到了平面的约束 （这里看的是movement这个组件里面的内容）
void UGBWFlowMoveComponent::SetMovementConstrain(const FVector& PlaneNormal, const FVector& PlaneOrigin)
{
	if (IsInServer() || IsLocalOwn())
	{
		if (TaskState.OwnerCharacter->GetMovementComponent()->bConstrainToPlane
		&& TaskState.OwnerCharacter->GetMovementComponent()->GetPlaneConstraintNormal() == PlaneNormal
		&& TaskState.OwnerCharacter->GetMovementComponent()->GetPlaneConstraintOrigin() == PlaneOrigin)
		{
			return;
		}
		SetMovementConstrain_Server(PlaneNormal, PlaneOrigin);
	}
}
void UGBWFlowMoveComponent::SetMovementConstrain_Server_Implementation(FVector PlaneNormal, FVector PlaneOrigin)
{
	SetMovementConstrain_Multicast(PlaneNormal, PlaneOrigin);
}
void UGBWFlowMoveComponent::SetMovementConstrain_Multicast_Implementation(FVector PlaneNormal, FVector PlaneOrigin)
{
	if (TaskState.OwnerCharacter && TaskState.OwnerCharacter->GetMovementComponent())
	{
		TaskState.OwnerCharacter->GetMovementComponent()->SetPlaneConstraintEnabled(true);
		TaskState.OwnerCharacter->GetMovementComponent()->SetPlaneConstraintNormal(PlaneNormal);
		TaskState.OwnerCharacter->GetMovementComponent()->SetPlaneConstraintOrigin(PlaneOrigin);
	}
}

void UGBWFlowMoveComponent::ResetMovementConstrain()
{
	if (IsInServer() || IsLocalOwn())
	{
		if (TaskState.OwnerCharacter->GetMovementComponent()->bConstrainToPlane == bIsConstrainToPlaneCache
		&& TaskState.OwnerCharacter->GetMovementComponent()->GetPlaneConstraintNormal() == PlaneConstraintNormalCache
		&& TaskState.OwnerCharacter->GetMovementComponent()->GetPlaneConstraintOrigin() == PlaneConstraintOriginCache)
		{
			return;
		}
		ResetMovementConstrain_Server();
	}
}
void UGBWFlowMoveComponent::ResetMovementConstrain_Server_Implementation()
{
	ResetMovementConstrain_Multicast();
}
void UGBWFlowMoveComponent::ResetMovementConstrain_Multicast_Implementation()
{
	TaskState.OwnerCharacter->GetMovementComponent()->SetPlaneConstraintEnabled(bIsConstrainToPlaneCache);
	TaskState.OwnerCharacter->GetMovementComponent()->SetPlaneConstraintNormal(PlaneConstraintNormalCache);
	TaskState.OwnerCharacter->GetMovementComponent()->SetPlaneConstraintOrigin(PlaneConstraintOriginCache);
}

void UGBWFlowMoveComponent::ResetUseControllerRotationYaw()
{
	if (IsInServer() || IsLocalOwn())
	{
		ResetUseControllerRotationYaw_Server();
	}
}
void UGBWFlowMoveComponent::ResetUseControllerRotationYaw_Server_Implementation()
{
	ResetUseControllerRotationYaw_Multicast();
}
void UGBWFlowMoveComponent::ResetUseControllerRotationYaw_Multicast_Implementation()
{
	if (TaskState.OwnerCharacter)
	{
		TaskState.OwnerCharacter->bUseControllerRotationYaw = bUseControllerRotationYawCache;
	}
}

void UGBWFlowMoveComponent::SetFlowMoveScene(
	const FGBWFlowMoveScene& NewScene,
	FGameplayTag TargetScene,
	const bool bIsRemoveSupplementaryScene)
{
	if (IsLocalOwn() || IsInServer())
	{
		if (!TargetScene.IsValid())
		{
			return;
		}

		const int SupplementarySceneIndex = TaskState.GetSupplementarySceneIndex(TargetScene);
		if (!bIsRemoveSupplementaryScene && SupplementarySceneIndex < 0)
		{
			SetFlowMoveScene_Imp(NewScene, TargetScene, bIsRemoveSupplementaryScene);
			SetFlowMoveScene_Server(NewScene, TargetScene, bIsRemoveSupplementaryScene);
			return;
		}
		else if (bIsRemoveSupplementaryScene && SupplementarySceneIndex >= 0)
		{
			SetFlowMoveScene_Imp(NewScene, TargetScene, bIsRemoveSupplementaryScene);
			SetFlowMoveScene_Server(NewScene, TargetScene, bIsRemoveSupplementaryScene);
			return;
		}
		
		FGBWFlowMoveSceneChangeInfo ChangeInfo = FGBWFlowMoveSceneChangeInfo();
		const bool bChanged = CheckSceneChange(NewScene,TargetScene,ChangeInfo);
		if (bChanged)
		{
			if (ChangeInfo.SceneType)
			{
				SetFlowMoveScene_SceneType_Imp(NewScene.SceneType, TargetScene);
				SetFlowMoveScene_SceneType_Server(NewScene.SceneType,TargetScene);
			}
			if (ChangeInfo.TargetActor)
			{
				SetFlowMoveScene_TargetActor_Imp(NewScene.TargetActor, TargetScene);
				SetFlowMoveScene_TargetActor_Server(NewScene.TargetActor,TargetScene);
			}
			if (ChangeInfo.Slope)
			{
				SetFlowMoveScene_Slope_Imp(NewScene.Slope, TargetScene);
				SetFlowMoveScene_Slope_Server(NewScene.Slope,TargetScene);
			}
			if (ChangeInfo.PerceptionResult)
			{
				SetFlowMoveScene_PerceptionResult_Imp(NewScene.PerceptionResult, TargetScene);
				SetFlowMoveScene_PerceptionResult_Server(NewScene.PerceptionResult,TargetScene);
			}
			if (ChangeInfo.TargetLocationScene)
			{
				SetFlowMoveScene_TargetLocation_Imp(NewScene.TargetLocationScene, TargetScene);
				SetFlowMoveScene_TargetLocation_Server(NewScene.TargetLocationScene,TargetScene);
			}
			if (ChangeInfo.ActorLocationScene)
			{
				SetFlowMoveScene_ActorLocation_Imp(NewScene.ActorLocationScene, TargetScene);
				SetFlowMoveScene_ActorLocation_Server(NewScene.ActorLocationScene,TargetScene);
			}
			if (ChangeInfo.LeftLocationScene)
			{
				SetFlowMoveScene_LeftLocation_Imp(NewScene.LeftLocationScene, TargetScene);
				SetFlowMoveScene_LeftLocation_Server(NewScene.LeftLocationScene,TargetScene);
			}
			if (ChangeInfo.RightLocationScene)
			{
				SetFlowMoveScene_RightLocation_Imp(NewScene.RightLocationScene, TargetScene);
				SetFlowMoveScene_RightLocation_Server(NewScene.RightLocationScene,TargetScene);
			}
		}
	}
}
void UGBWFlowMoveComponent::SetFlowMoveScene_Server_Implementation(
		FGBWFlowMoveScene NewScene,
		FGameplayTag TargetScene,
		bool bIsRemoveSupplementaryScene)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_SetFlowMoveScene");
	SetFlowMoveScene_Imp(NewScene, TargetScene, bIsRemoveSupplementaryScene);
}
void UGBWFlowMoveComponent::SetFlowMoveScene_Imp(
	const FGBWFlowMoveScene& NewScene,
	FGameplayTag TargetScene,
	const bool bIsRemoveSupplementaryScene)
{
	if (bIsRemoveSupplementaryScene)
	{
		TaskState.RemoveSupplementaryScene(TargetScene);
	}
	else
	{
		TaskState.SetSupplementaryScene(TargetScene,NewScene);
	}
}

void UGBWFlowMoveComponent::ApplyActionLockStateSetting(bool bHasActionLock,
	const FGBWFlowMoveActionLockStateSettings ActionLockStateSetting)
{
	if (IsLocalOwn() || IsInServer())
	{
		ApplyActionLockStateSetting_Server(bHasActionLock,ActionLockStateSetting);
	}
}
void UGBWFlowMoveComponent::ApplyActionLockStateSetting_Server_Implementation(bool bHasActionLock,
	const FGBWFlowMoveActionLockStateSettings ActionLockStateSetting)
{
	ApplyActionLockStateSetting_Imp(bHasActionLock,ActionLockStateSetting);
}
void UGBWFlowMoveComponent::ApplyActionLockStateSetting_Imp(bool bHasActionLock,
	const FGBWFlowMoveActionLockStateSettings& ActionLockStateSetting)
{
	if (IsInServer())
	{
		SetActionLockState(bHasActionLock,ActionLockStateSetting.ActionToLock);
	}
}

void UGBWFlowMoveComponent::SetActionLockState(
		bool bHasActionLock,FGameplayTagQuery ActionToLock)
{
	if (IsLocalOwn() || IsInServer())
	{
		SetActionLockState_Server(bHasActionLock,ActionToLock);
	}
}
void UGBWFlowMoveComponent::SetActionLockState_Server_Implementation(
		bool bHasActionLock,FGameplayTagQuery ActionToLock)
{
	SetActionLockState_Imp(bHasActionLock,ActionToLock);
}
void UGBWFlowMoveComponent::SetActionLockState_Imp(
		bool bHasActionLock,const FGameplayTagQuery& ActionToLock)
{
	TaskState.ActionLockState.bHasLock = bHasActionLock;
	TaskState.ActionLockState.ActionToLock = ActionToLock;
}

void UGBWFlowMoveComponent::SetFlowMoveScene_SceneType_Imp(
	const FName SceneType,
	FGameplayTag TargetScene)
{
	if (!TargetScene.IsValid())
	{
		return;
	}
	if (const int Index = TaskState.GetSupplementarySceneIndex(TargetScene); Index>=0)
	{
		TaskState.SupplementaryFlowMoveScene[Index].Scene.SceneType = SceneType;
	}
	else
	{
		FGBWFlowMoveScene NewScene = FGBWFlowMoveScene();
		NewScene.SceneType = SceneType;
		TaskState.SetSupplementaryScene(TargetScene,NewScene);
	}
}
void UGBWFlowMoveComponent::SetFlowMoveScene_SceneType_Server_Implementation(
	FName SceneType,
	FGameplayTag TargetScene)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_SetFlowMoveScene_SceneType");
	SetFlowMoveScene_SceneType_Imp(SceneType, TargetScene);
}

void UGBWFlowMoveComponent::SetFlowMoveScene_TargetActor_Imp(AActor* TargetActor,
	FGameplayTag TargetScene)
{
	if (!TargetScene.IsValid())
	{
		return;
	}
	if (const int Index = TaskState.GetSupplementarySceneIndex(TargetScene); Index>=0)
	{
		TaskState.SupplementaryFlowMoveScene[Index].Scene.TargetActor = TargetActor;
	}
	else
	{
		FGBWFlowMoveScene NewScene = FGBWFlowMoveScene();
		NewScene.TargetActor = TargetActor;
		TaskState.SetSupplementaryScene(TargetScene,NewScene);
	}
}
void UGBWFlowMoveComponent::SetFlowMoveScene_TargetActor_Server_Implementation(AActor* TargetActor,
                                                                               FGameplayTag TargetScene)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_SetFlowMoveScene_TargetActor");
	SetFlowMoveScene_TargetActor_Imp(TargetActor,TargetScene);
}

void UGBWFlowMoveComponent::SetFlowMoveScene_ActorLocation_Server_Implementation(FGBWFlowMovePointScene ActorLocation,
                                                                                 FGameplayTag TargetScene)
{
	SetFlowMoveScene_ActorLocation_Imp(ActorLocation, TargetScene);
}
void UGBWFlowMoveComponent::SetFlowMoveScene_ActorLocation_Imp(const FGBWFlowMovePointScene& ActorLocation, FGameplayTag TargetScene)
{
	if (!TargetScene.IsValid())
	{
		return;
	}
	if (const int Index = TaskState.GetSupplementarySceneIndex(TargetScene); Index>=0)
	{
		TaskState.SupplementaryFlowMoveScene[Index].Scene.ActorLocationScene = ActorLocation;
	}
	else
	{
		FGBWFlowMoveScene NewScene = FGBWFlowMoveScene();
		NewScene.ActorLocationScene = ActorLocation;
		TaskState.SetSupplementaryScene(TargetScene,NewScene);
	}
}

void UGBWFlowMoveComponent::SetFlowMoveScene_LeftLocation_Server_Implementation(FGBWFlowMovePointScene Location,
                                                                                FGameplayTag TargetScene)
{
	SetFlowMoveScene_LeftLocation_Imp(Location, TargetScene);
}
void UGBWFlowMoveComponent::SetFlowMoveScene_LeftLocation_Imp(const FGBWFlowMovePointScene& Location,
	FGameplayTag TargetScene)
{
	if (!TargetScene.IsValid())
	{
		return;
	}
	if (const int Index = TaskState.GetSupplementarySceneIndex(TargetScene); Index>=0)
	{
		TaskState.SupplementaryFlowMoveScene[Index].Scene.LeftLocationScene = Location;
	}
	else
	{
		FGBWFlowMoveScene NewScene = FGBWFlowMoveScene();
		NewScene.LeftLocationScene = Location;
		TaskState.SetSupplementaryScene(TargetScene,NewScene);
	}
}

void UGBWFlowMoveComponent::SetFlowMoveScene_RightLocation_Server_Implementation(FGBWFlowMovePointScene Location,
                                                                                 FGameplayTag TargetScene)
{
	SetFlowMoveScene_RightLocation_Imp(Location, TargetScene);
}
void UGBWFlowMoveComponent::SetFlowMoveScene_RightLocation_Imp(const FGBWFlowMovePointScene& Location,
	FGameplayTag TargetScene)
{
	if (!TargetScene.IsValid())
	{
		return;
	}
	if (const int Index = TaskState.GetSupplementarySceneIndex(TargetScene); Index>=0)
	{
		TaskState.SupplementaryFlowMoveScene[Index].Scene.RightLocationScene = Location;
	}
	else
	{
		FGBWFlowMoveScene NewScene = FGBWFlowMoveScene();
		NewScene.RightLocationScene = Location;
		TaskState.SetSupplementaryScene(TargetScene,NewScene);
	}
}

void UGBWFlowMoveComponent::SetFlowMoveScene_TargetLocation_Server_Implementation(FGBWFlowMovePointScene TargetLocation,
                                                                                  FGameplayTag TargetScene)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_SetFlowMoveScene_TargetLocation");
	SetFlowMoveScene_TargetLocation_Imp(TargetLocation, TargetScene);
}
void UGBWFlowMoveComponent::SetFlowMoveScene_TargetLocation_Imp(const FGBWFlowMovePointScene& TargetLocation,
	FGameplayTag TargetScene)
{
	if (!TargetScene.IsValid())
	{
		return;
	}
	if (const int Index = TaskState.GetSupplementarySceneIndex(TargetScene); Index>=0)
	{
		TaskState.SupplementaryFlowMoveScene[Index].Scene.TargetLocationScene = TargetLocation;
	}
	else
	{
		FGBWFlowMoveScene NewScene = FGBWFlowMoveScene();
		NewScene.TargetLocationScene = TargetLocation;
		TaskState.SetSupplementaryScene(TargetScene,NewScene);
	}
}

void UGBWFlowMoveComponent::SetFlowMoveScene_Slope_Imp(const float Slope,
	FGameplayTag TargetScene)
{
	if (!TargetScene.IsValid())
	{
		return;
	}
	if (const int Index = TaskState.GetSupplementarySceneIndex(TargetScene); Index>=0)
	{
		TaskState.SupplementaryFlowMoveScene[Index].Scene.Slope = Slope;
	}
	else
	{
		FGBWFlowMoveScene NewScene = FGBWFlowMoveScene();
		NewScene.Slope = Slope;
		TaskState.SetSupplementaryScene(TargetScene,NewScene);
	}
}
void UGBWFlowMoveComponent::SetFlowMoveScene_Slope_Server_Implementation(float Slope,
                                                                         FGameplayTag TargetScene)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_SetFlowMoveScene_Slope");
	SetFlowMoveScene_Slope_Imp(Slope,TargetScene);
}

void UGBWFlowMoveComponent::SetFlowMoveScene_PerceptionResult_Imp(const FGBWPerceptionResult PerceptionResult,
	FGameplayTag TargetScene)
{
	if (!TargetScene.IsValid())
	{
		return;
	}
	if (const int Index = TaskState.GetSupplementarySceneIndex(TargetScene); Index>=0)
	{
		TaskState.SupplementaryFlowMoveScene[Index].Scene.PerceptionResult = PerceptionResult;
	}
	else
	{
		FGBWFlowMoveScene NewScene = FGBWFlowMoveScene();
		NewScene.PerceptionResult = PerceptionResult;
		TaskState.SetSupplementaryScene(TargetScene,NewScene);
	}
}
void UGBWFlowMoveComponent::SetFlowMoveScene_PerceptionResult_Server_Implementation(
	FGBWPerceptionResult PerceptionResult, FGameplayTag TargetScene)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_SetFlowMoveScene_PerceptionResult");
	SetFlowMoveScene_PerceptionResult_Imp(PerceptionResult,TargetScene);
}

void UGBWFlowMoveComponent::SetFlowMoveLastAction(
		FGameplayTag NewLastActionTag)
{
	if (IsInServer())
	{
		SetFlowMoveLastAction_Server(NewLastActionTag);
	}
}

void UGBWFlowMoveComponent::SetFlowMoveLastAction_Server_Implementation(
		FGameplayTag NewLastActionTag)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_SetFlowMoveLastAction");
	SetFlowMoveLastAction_Imp(NewLastActionTag);
}

void UGBWFlowMoveComponent::SetFlowMoveLastAction_Imp(
		FGameplayTag NewLastActionTag)
{
	TaskState.LastFlowMoveActionTag = NewLastActionTag;
}

void UGBWFlowMoveComponent::ActivePerceptron(const FGameplayTag PerceptronTag)
{
	if (IsInServer() || IsLocalOwn())
	{
		ActivePerceptron_Server(PerceptronTag);
	}
}
void UGBWFlowMoveComponent::ActivePerceptron_Server_Implementation(const FGameplayTag PerceptronTag)
{
	ActivePerceptron_Imp(PerceptronTag);
}
void UGBWFlowMoveComponent::ActivePerceptron_Imp(const FGameplayTag PerceptronTag)
{
	TaskState.AddPerceptronTag(PerceptronTag);
}

void UGBWFlowMoveComponent::DeactivePerceptron(const FGameplayTag PerceptronTag)
{
	if (IsInServer() || IsLocalOwn())
	{
		DeactivePerceptron_Server(PerceptronTag);
	}
}
void UGBWFlowMoveComponent::DeactivePerceptron_Server_Implementation(const FGameplayTag PerceptronTag)
{
	DeactivePerceptron_Imp(PerceptronTag);
}
void UGBWFlowMoveComponent::DeactivePerceptron_Imp(const FGameplayTag PerceptronTag)
{
	TaskState.RemovePerceptronTag(PerceptronTag);
}

void UGBWFlowMoveComponent::ActiveScript(const FGameplayTag ScriptTag)
{
	if (IsInServer() || IsLocalOwn())
	{
		ActiveScript_Server(ScriptTag);
	}
}
void UGBWFlowMoveComponent::ActiveScript_Server_Implementation(const FGameplayTag ScriptTag)
{
	ActiveScript_Imp(ScriptTag);
}
void UGBWFlowMoveComponent::ActiveScript_Imp(const FGameplayTag ScriptTag)
{
	TaskState.AddScriptTag(ScriptTag);
}

void UGBWFlowMoveComponent::DeactiveScript(const FGameplayTag ScriptTag)
{
	if (IsInServer() || IsLocalOwn())
	{
		DeactiveScript_Server(ScriptTag);
	}
}
void UGBWFlowMoveComponent::DeactiveScript_Server_Implementation(const FGameplayTag ScriptTag)
{
	DeactiveScript_Imp(ScriptTag);
}
void UGBWFlowMoveComponent::DeactiveScript_Imp(const FGameplayTag ScriptTag)
{
	TaskState.RemoveScriptTag(ScriptTag);
}

void UGBWFlowMoveComponent::SetMoveControl(const FGBWFlowMoveControlSettings MoveControlSettings)
{
	if (IsInServer() || IsLocalOwn())
	{
		SetMoveControl_Server(MoveControlSettings);
	}
}
void UGBWFlowMoveComponent::SetMoveControl_Server_Implementation(const FGBWFlowMoveControlSettings MoveControlSettings)
{
	SetMoveControl_Imp(MoveControlSettings);
}
void UGBWFlowMoveComponent::SetMoveControl_Imp(const FGBWFlowMoveControlSettings& MoveControlSettings)
{
	TaskState.MoveControlParam.bIsActive = true;
	TaskState.MoveControlParam.ActionGuid = MoveControlSettings.ActionGuid;
	TaskState.MoveControlParam.ActionTag = MoveControlSettings.ActionTag;
	TaskState.MoveControlParam.ActionTargetScene = MoveControlSettings.ActionTargetScene;
	TaskState.MoveControlParam.ActionTargetPoint = MoveControlSettings.ActionTargetPoint;

	if (MoveControlSettings.bIsUsePostureAdjust)
	{
		TaskState.MoveControlParam.PostureAdjustAmplitude = MoveControlSettings.PostureAdjustAmplitude;
		TaskState.MoveControlParam.PostureAdjustReferenceSpeed = MoveControlSettings.PostureAdjustReferenceSpeed;
		TaskState.MoveControlParam.bIsLock_UsePostureAdjust = true;
	}
	else
	{
		TaskState.MoveControlParam.PostureAdjustAmplitude = 30.0f;
		TaskState.MoveControlParam.PostureAdjustReferenceSpeed = 1000.0f;
		TaskState.MoveControlParam.bIsLock_UsePostureAdjust = false;
	}
	
	if (MoveControlSettings.bIsConstrainMoveToTargetPlane)
	{
		TaskState.MoveControlParam.ConstrainMoveTime = MoveControlSettings.ConstrainMoveTime;
		TaskState.MoveControlParam.bIsLock_ConstrainMoveToTargetPlane = true;
	}
	else
	{
		TaskState.MoveControlParam.bIsLock_ConstrainMoveToTargetPlane = false;
		TaskState.MoveControlParam.ConstrainMoveTime = 0.2f;
	}

	if (MoveControlSettings.bIsUseInertia)
	{
		TaskState.MoveControlParam.InertiaTime = MoveControlSettings.InertiaTime;
		TaskState.MoveControlParam.bIsLock_UseInertia = true;
	}
	else
	{
		TaskState.MoveControlParam.InertiaTime = 0.3f;
		TaskState.MoveControlParam.bIsLock_UseInertia = false;
	}
	
	if (MoveControlSettings.bIsMaxSpeed)
	{
		TaskState.MoveControlParam.MaxMoveSpeed = MoveControlSettings.MaxSpeed;
		TaskState.MoveControlParam.bIsLock_MaxSpeed = true;
	}
	else
	{
		TaskState.MoveControlParam.bIsLock_MaxSpeed = false;
		TaskState.MoveControlParam.MaxMoveSpeed = 5000.0f;
	}
	
	if (MoveControlSettings.bIsGravity)
	{
		TaskState.MoveControlParam.TargetGravity = MoveControlSettings.Gravity;
		TaskState.MoveControlParam.bIsLock_Gravity = true;
		TaskState.MoveControlParam.GravitySmoothSpeed = MoveControlSettings.GravitySmoothSpeed;
	}
	else
	{
		TaskState.MoveControlParam.bIsLock_Gravity = false;
		TaskState.MoveControlParam.GravitySmoothSpeed = 6.0f;
	}

	if (MoveControlSettings.bIsMoveSpeed)
	{
		TaskState.MoveControlParam.TargetMoveSpeed = MoveControlSettings.MoveSpeed;
        TaskState.MoveControlParam.bIsLock_MoveSpeed = true;
		TaskState.MoveControlParam.MoveSpeedSmoothSpeed = MoveControlSettings.MoveSpeedSmoothSpeed;
		TaskState.MoveControlParam.CompensateLostSpeedToCurrentSpeedDirection = MoveControlSettings.CompensateLostSpeedToCurrentSpeedDirection;
	}
	else
	{
		TaskState.MoveControlParam.bIsLock_MoveSpeed = false;
		TaskState.MoveControlParam.MoveSpeedSmoothSpeed = 6.0f;
		TaskState.MoveControlParam.CompensateLostSpeedToCurrentSpeedDirection = false;
	}

	if (MoveControlSettings.bIsMoveToDirectionSmoothSpeed)
	{
		TaskState.MoveControlParam.MoveToDirectionSmoothSpeed = MoveControlSettings.MoveToDirectionSmoothSpeed;
		TaskState.MoveControlParam.bIsLock_MoveToDirectionSmoothSpeed = true;
	}
	else
	{
		TaskState.MoveControlParam.bIsLock_MoveToDirectionSmoothSpeed = false;
	}

	if (MoveControlSettings.bIsRotationLockToMoveDirectionYaw)
	{
		TaskState.MoveControlParam.RotationLockToMoveDirectionYaw = MoveControlSettings.RotationLockToMoveDirectionYaw;
		TaskState.MoveControlParam.RotationLockTo = MoveControlSettings.RotationLockTo;
		TaskState.MoveControlParam.RotationLockToSmoothSpeed = MoveControlSettings.RotationLockToSmoothSpeed;
        TaskState.MoveControlParam.bIsLock_RotationLockToMoveDirectionYaw = true;
	}
	else
	{
		TaskState.MoveControlParam.RotationLockToMoveDirectionYaw = 0.0f;
		TaskState.MoveControlParam.RotationLockTo = EFlowMoveDirectionType::ActorForward;
		TaskState.MoveControlParam.RotationLockToSmoothSpeed = 12.0f;
		TaskState.MoveControlParam.bIsLock_RotationLockToMoveDirectionYaw = false;
	}

	if (MoveControlSettings.bIsUseSpeedAdjustAnimPlayRate)
	{
		TaskState.MoveControlParam.AnimPlayRateAdjust = MoveControlSettings.AnimPlayRateAdjust;
	}
	else
	{
		TaskState.MoveControlParam.AnimPlayRateAdjust.bUseSpeedAdjustAnimPlayRate = false;
	}
}

void UGBWFlowMoveComponent::DeactiveMoveControl()
{
	if (IsInServer() || IsLocalOwn())
	{
		DeactiveMoveControl_Server();
	}
}
void UGBWFlowMoveComponent::DeactiveMoveControl_Server_Implementation()
{
	DeactiveMoveControl_Imp();
}
void UGBWFlowMoveComponent::DeactiveMoveControl_Imp()
{
	TaskState.MoveControlParam = FGBWFlowMoveControlParam();
	TaskState.MoveControlParam.bIsActive = false;
}

void UGBWFlowMoveComponent::ResetMoveControl()
{
	if (IsInServer() || IsLocalOwn())
	{
		ResetMoveControl_Server();
	}
}
void UGBWFlowMoveComponent::ResetMoveControl_Server_Implementation()
{
	ResetMoveControl_Imp();
}
void UGBWFlowMoveComponent::ResetMoveControl_Imp()
{
	TaskState.MoveControlParam.bIsLock_Gravity = false;
	TaskState.MoveControlParam.GravitySmoothSpeed = 0.0f;
	TaskState.MoveControlParam.bIsLock_MoveSpeed = false;
	TaskState.MoveControlParam.MoveSpeedSmoothSpeed = 0.0f;
	TaskState.MoveControlParam.bIsLock_MoveToDirectionSmoothSpeed = false;
	TaskState.MoveControlParam.bIsLock_RotationLockToMoveDirectionYaw = false;
	TaskState.MoveControlParam.AnimPlayRateAdjust.bUseSpeedAdjustAnimPlayRate = false;
}

FGBWFlowMoveControlParam UGBWFlowMoveComponent::GetMoveControlParam() const
{
	return TaskState.MoveControlParam;
}

FVector UGBWFlowMoveComponent::GetMoveToVector(float DeltaTime, bool bUseDefaultMoveVectorWhenZero, const FVector& DefaultMoveVector)
{
	if (DeltaTime > SMALL_NUMBER)
	{
		FVector InputVector = GetMoveVector(true);

		if (InputVector.IsNearlyZero())
		{
			if (bUseDefaultMoveVectorWhenZero)
			{
				InputVector = DefaultMoveVector;
			}
			else
			{
				InputVector = TaskState.OwnerCharacter->GetActorForwardVector();
				InputVector.Z = 0.0f;
			}
		}
		InputVector.Normalize();
		
		FVector ForwardVector = GetControlVector();
		ForwardVector.Z = 0.0f;
		if (!InputVector.IsNearlyZero())
		{
			ForwardVector = ForwardVector.Rotation().RotateVector(InputVector);
		}
		ForwardVector.Normalize();
		return ForwardVector;
	}

	return TaskState.OwnerCharacter? TaskState.OwnerCharacter->GetActorForwardVector() : FVector::ZeroVector;
}

bool UGBWFlowMoveComponent::IsActionNow() const
{
	return TaskState.MoveControlParam.bIsActive;
}

UGBWFlowMoveBrain_Base* UGBWFlowMoveComponent::GetFlowMoveBrain()
{
	return FlowMoveBrain;
}

FVector UGBWFlowMoveComponent::GetDirectionVector(EFlowMoveDirectionType Type)
{
	FVector Res = FVector::ZeroVector;
	switch (Type) { case ActorForward:
		Res = TaskState.OwnerCharacter->GetActorForwardVector();
		break;
	case Velocity:
		Res = TaskState.OwnerCharacter->GetVelocity();
		break;
	case Controller:
		Res = GetControlVector();
		break;
	case CurrentMoveToDirection:
		Res = GetMoveControlParam().MoveToDirection;
		break;
	case TargetMoveToDirection:
		Res = GetControlVector();
		Res.Z = 0.0f;
		Res.Normalize();
		Res = Res.Rotation().RotateVector(GetMoveVector());
		break;
	case ActorToTarget:
		Res = GetActionTargetPoint(GetMoveControlParam().ActionTargetPoint) - TaskState.OwnerCharacter->GetActorLocation();
		break;
	case StartPointToTarget:
		Res = GetActionTargetPoint(GetMoveControlParam().ActionTargetPoint) - CurrentActionTargetSceneInfo.BeginTransform.GetLocation();
		break;
	default: ;
	}

	Res.Z = 0.0f;
	Res.Normalize();

	if (Res.IsNearlyZero())
	{
		Res = GetMoveControlParam().MoveToDirection;
		Res.Z = 0.0f;
		Res.Normalize();
	}

	return Res;
}

bool UGBWFlowMoveComponent::GetScene(FGBWFlowMoveScene& Scene, FGameplayTag SceneSlot, bool bIsConsume)
{
	bool Res = TaskState.GetSupplementaryScene(SceneSlot, Scene);
	if (bIsConsume)
	{
		SetFlowMoveScene(Scene, SceneSlot, true);
	}
	return Res;
}

bool UGBWFlowMoveComponent::IsActionLock(FGameplayTagContainer ActionToLock) const
{
	if (TaskState.ActionLockState.ActionToLock.IsEmpty())
	{
		return false;
	}
	return TaskState.ActionLockState.bHasLock
	&& ActionToLock.IsValid()
	&& TaskState.ActionLockState.ActionToLock.Matches(ActionToLock);
}

bool UGBWFlowMoveComponent::IsAnyActionLock() const
{
	return TaskState.ActionLockState.bHasLock;
}

void UGBWFlowMoveComponent::SetCurrentTargetScene(FGameplayTag ActionTargetScene)
{
	if (IsInServer())
	{
		SetCurrentTargetScene_Server(ActionTargetScene);
	}
}

void UGBWFlowMoveComponent::SetCurrentTargetScene_Server_Implementation(FGameplayTag ActionTargetScene)
{
	if (TaskState.OwnerCharacter)
	{
		FGBWFlowMoveRMSControllerInitInfo ActionTargetSceneInfo = FGBWFlowMoveRMSControllerInitInfo();
		ActionTargetSceneInfo.ActionTargetScene = ActionTargetScene;
		ActionTargetSceneInfo.BeginFlowMoveState = TaskState;
		ActionTargetSceneInfo.BeginTransform = TaskState.OwnerCharacter->GetTransform();
		ActionTargetSceneInfo.BeginVelocity = TaskState.OwnerCharacter->GetVelocity();
		ActionTargetSceneInfo.bIsValid = TaskState.GetSupplementaryScene(ActionTargetScene, ActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene);
		CurrentActionTargetSceneInfo = ActionTargetSceneInfo;
	}
}

void UGBWFlowMoveComponent::SetCurrentActionTag(FGameplayTag ActionTag)
{
	if (IsInServer())
	{
		SetCurrentActionTag_Server(ActionTag);
	}
}
void UGBWFlowMoveComponent::SetCurrentActionTag_Server_Implementation(FGameplayTag ActionTag)
{
	if (TaskState.MoveControlParam.ActionTag.IsValid())
	{
		TaskState.LastFlowMoveActionTag = TaskState.MoveControlParam.ActionTag;
	}
	TaskState.CurrentFlowMoveActionTag = ActionTag;
}

FVector UGBWFlowMoveComponent::GetActionTargetPoint(EGBWFlowMoveScenePointType ActionTargetPoint)
{
	float Dis_F;
	float Dis_B;
	switch (ActionTargetPoint) {
		case TargetPoint: return CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.TargetLocationScene.Point;
		case ActorPoint: return CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.Point;
		case LeftSidePoint: return CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.LeftLocationScene.Point;
		case RightSidePoint: return CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.RightLocationScene.Point;
		case FloorPoint: return CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.Floor;
		case RoofPoint: return CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.Roof;
		case FloorForwardLedgePoint: return CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Forward;
		case FloorBackwardLedgePoint: return CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Backward;
		case RoofForwardLedgePoint: return CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Forward;
		case RoofBackwardLedgePoint: return CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Backward;
		case NearestFloorLedgePoint:
			Dis_F = (CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Forward
				- CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.Floor).Length();
			Dis_B = (CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Backward
					- CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.Floor).Length();
			if (Dis_F < Dis_B)
			{
				return CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Forward;
			}
			else
			{
				return CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Backward;
			}
		case NearestRoofLedgePoint: 
			Dis_F = (CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Forward
							- CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.Roof).Length();
			Dis_B = (CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Backward
					- CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.Roof).Length();
			if (Dis_F < Dis_B)
			{
				return CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Forward;
			}
			else
			{
				return CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Backward;
			}
	default: ;
	}

	return FVector::ZeroVector;
}

bool UGBWFlowMoveComponent::IsLocalPlayer(bool bIsReset)
{
	if (!TaskState.OwnerCharacter)
	{
		return false;
	}

	if (!bIsLocalPlayerInit || bIsReset)
	{
		const EGBWActorNetType ActorNetType = UGBWPowerfulToolsFuncLib::GetActorNetType(TaskState.OwnerCharacter);
		bIsLocalPlayer =
			ActorNetType == EGBWActorNetType::InClient_LocalPlayer
			|| ActorNetType == EGBWActorNetType::InServer_LocalPlayer;
		bIsLocalPlayerInit = true;
	}
	return bIsLocalPlayer;
}

bool UGBWFlowMoveComponent::IsAIPlayer(bool bIsReset)
{
	if (!TaskState.OwnerCharacter)
	{
		return false;
	}

	if (bIsReset)
	{
		if (IsInServer())
		{
			bIsAIPlayer = UGBWFlowMoveFuncLib::IsAIPlayer(TaskState.OwnerCharacter);
			bIsAIPlayerInit = true;
		}
	}
	
	return bIsAIPlayer;
}

bool UGBWFlowMoveComponent::IsLocalOwn(bool bIsReset)
{
	if (!TaskState.OwnerCharacter)
	{
		return false;
	}

	if (!bIsLocalOwnInit || bIsReset)
	{
		bIsLocalOwn = UGBWFlowMoveFuncLib::IsLocalOwn(TaskState.OwnerCharacter);
		bIsLocalOwnInit = true;
	}
	
	return bIsLocalOwn;
}

bool UGBWFlowMoveComponent::IsInServer(bool bIsReset)
{
	if (!bIsInServerInit || bIsReset)
	{
		bIsInServer = IsValid(TaskState.OwnerCharacter) && TaskState.OwnerCharacter->HasAuthority();
		bIsInServerInit = true;
	}
	return bIsInServer;
}

bool UGBWFlowMoveComponent::IsFlowMoveActive() const
{
	return TaskState.OwnerCharacter && TaskState.bIsActive;
}

bool UGBWFlowMoveComponent::IsFlowMoveStopping() const
{
	return TaskState.bIsStopping;
}

AActor* UGBWFlowMoveComponent::GetNetWorkActor(const FGuid ActorGuid)
{
	if (ActorGuid.IsValid())
	{
		const int Index = NetWorkActorGuids.Find(ActorGuid);
		if (Index != INDEX_NONE)
		{
			return NetWorkActors[Index];
		}
	}

	return nullptr;
}

void UGBWFlowMoveComponent::SpawnNetWorkActor(const FGuid ActorGuid, const TSubclassOf<AActor> ActorClass, const FTransform Transform)
{
	if (IsInServer() || IsLocalOwn())
	{
		SpawnNetWorkActor_Server(ActorGuid, ActorClass, Transform);
	}
}
void UGBWFlowMoveComponent::SpawnNetWorkActor_Server_Implementation(const FGuid ActorGuid, const TSubclassOf<AActor> ActorClass, const FTransform Transform)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_SpawnNetWorkActor");
	SpawnNetWorkActor_Imp(ActorGuid, ActorClass, Transform);
}
void UGBWFlowMoveComponent::SpawnNetWorkActor_Imp(FGuid ActorGuid, const TSubclassOf<AActor> ActorClass, const FTransform Transform)
{
	if (ActorGuid.IsValid())
	{
		const int Index = NetWorkActorGuids.Find(ActorGuid);
		if (Index != INDEX_NONE)
		{
			if (NetWorkActors[Index])
			{
				NetWorkActors[Index]->Destroy();
			}
			
			NetWorkActorGuids.RemoveAt(Index);
			NetWorkActors.RemoveAt(Index);
		}
		
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = TaskState.OwnerCharacter;
		SpawnInfo.Instigator = TaskState.OwnerCharacter;
		SpawnInfo.Name = FName(ActorGuid.ToString());
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (AActor* Actor = TaskState.OwnerCharacter->GetWorld()->SpawnActor<AActor>(ActorClass, Transform, SpawnInfo))
		{
			Actor->SetReplicates(true);
			Actor->SetReplicateMovement(true);
			NetWorkActorGuids.Add(ActorGuid);
			NetWorkActors.Add(Actor);
		}
	}
}

void UGBWFlowMoveComponent::DestroyNetWorkActor(const FGuid ActorGuid)
{
	if (IsInServer() || IsLocalOwn())
	{
		DestroyNetWorkActor_Server(ActorGuid);
	}
}
void UGBWFlowMoveComponent::DestroyNetWorkActor_Server_Implementation(const FGuid ActorGuid)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_DestroyNetWorkActor");
	DestroyNetWorkActor_Imp(ActorGuid);
}
void UGBWFlowMoveComponent::DestroyNetWorkActor_Imp(const FGuid ActorGuid)
{
	if (ActorGuid.IsValid())
	{
		const int Index = NetWorkActorGuids.Find(ActorGuid);
		if (Index != INDEX_NONE)
		{
			if (NetWorkActors[Index])
			{
				NetWorkActors[Index]->Destroy();
			}
			
			NetWorkActorGuids.RemoveAt(Index);
			NetWorkActors.RemoveAt(Index);
		}
	}
}

void UGBWFlowMoveComponent::SetNetWorkActorLocation(const FGuid ActorGuid, const FVector NewLocation)
{
	if (IsLocalOwn() || IsInServer())
	{
		SetNetWorkActorLocation_Server(ActorGuid, NewLocation);
	}
}
void UGBWFlowMoveComponent::SetNetWorkActorLocation_Server_Implementation(const FGuid ActorGuid, const FVector NewLocation)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_SetNetWorkActorLocation");
	SetNetWorkActorLocation_Imp(ActorGuid, NewLocation);
}
void UGBWFlowMoveComponent::SetNetWorkActorLocation_Imp(const FGuid ActorGuid, const FVector NewLocation)
{
	if (ActorGuid.IsValid())
	{
		const int Index = NetWorkActorGuids.Find(ActorGuid);
		if (Index != INDEX_NONE)
		{
			if (NetWorkActors[Index])
			{
				NetWorkActors[Index]->SetActorLocation(NewLocation);
			}
		}
	}
}

void UGBWFlowMoveComponent::ResetCapsuleAndMesh()
{
	if (IsInServer())
	{
		ResetCapsuleAndMesh_Server();
	}
}
void UGBWFlowMoveComponent::ResetCapsuleAndMesh_Server_Implementation()
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_ResetCapsuleAndMesh");
	ResetCapsuleAndMesh_Multicast();
}
void UGBWFlowMoveComponent::ResetCapsuleAndMesh_Multicast_Implementation()
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Multicast_ResetCapsuleAndMesh");
	ResetCapsuleAndMesh_Imp();
}
void UGBWFlowMoveComponent::ResetCapsuleAndMesh_Imp() const
{
	TaskState.OwnerCharacter->GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleHalfHeightCache);
	TaskState.OwnerCharacter->GetCapsuleComponent()->SetCapsuleRadius(CapsuleRadiusCache);
	TaskState.OwnerCharacter->GetMesh()->SetRelativeLocation(MeshRelativeLocationCache);
}

void UGBWFlowMoveComponent::SetCapsuleAndKeepMesh(float NewHeight)
{
	if (IsInServer())
	{
		const FVector CapScale = TaskState.OwnerCharacter->GetCapsuleComponent()->GetComponentScale();
		const float TargetHalfHeight = NewHeight / 2.0f / CapScale.Z;
		float TargetRadius = CapsuleRadiusCache;
		if (NewHeight / 2.0f < TaskState.OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius())
		{
			TargetRadius = TargetHalfHeight / 2.0f ;
		}

		const float TargetMeshRlz = MeshRelativeLocationCache.Z + (CapsuleHalfHeightCache * CapScale.Z - NewHeight / 2.0f);

		if (TaskState.OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() == TargetRadius
			&& TaskState.OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() == TargetHalfHeight
			&& TaskState.OwnerCharacter->GetMesh()->GetRelativeLocation().Z == TargetMeshRlz)
		{
			return;
		}

		SetCapsuleAndKeepMesh_Server(TargetRadius,TargetHalfHeight,TargetMeshRlz);
	}
}
void UGBWFlowMoveComponent::SetCapsuleAndKeepMesh_Server_Implementation(float TargetRadius, float TargetHalfHeight, float TargetMeshRlz)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Server_SetCapsuleAndKeepMesh");
	SetCapsuleAndKeepMesh_Multicast(TargetRadius,TargetHalfHeight,TargetMeshRlz);
}
void UGBWFlowMoveComponent::SetCapsuleAndKeepMesh_Multicast_Implementation(float TargetRadius, float TargetHalfHeight, float TargetMeshRlz)
{
	//UGBWDebugComponent::GBWDebug_CounterPrint(TaskState.OwnerCharacter,"FM_RPC_Multicast_SetCapsuleAndKeepMesh");
	SetCapsuleAndKeepMesh_Imp(TargetRadius,TargetHalfHeight,TargetMeshRlz);
}
void UGBWFlowMoveComponent::SetCapsuleAndKeepMesh_Imp(float TargetRadius, float TargetHalfHeight, float TargetMeshRlz) const
{
	TaskState.OwnerCharacter->GetCapsuleComponent()->SetCapsuleRadius(TargetRadius);
	TaskState.OwnerCharacter->GetCapsuleComponent()->SetCapsuleHalfHeight(TargetHalfHeight);
	TaskState.OwnerCharacter->GetMesh()->SetRelativeLocation(FVector(
		MeshRelativeLocationCache.X,
		MeshRelativeLocationCache.Y,
		TargetMeshRlz
	));
}

void UGBWFlowMoveComponent::AdjustCapsuleAndKeepMeshByTargetLocation(FGBWFlowMoveScene FlowMoveScene)
{
	if (FlowMoveScene.TargetLocationScene.FloorAndRoof.Floor.IsNearlyZero() || FlowMoveScene.TargetLocationScene.FloorAndRoof.Roof.IsNearlyZero())
	{
		ResetCapsuleAndMesh();
	}
	else
	{
		const float TargetHeight = FMath::Abs((FlowMoveScene.TargetLocationScene.FloorAndRoof.Floor - FlowMoveScene.TargetLocationScene.FloorAndRoof.Roof).Z);
		if (TargetHeight / 2.0f > CapsuleHalfHeightCache * TaskState.OwnerCharacter->GetCapsuleComponent()->GetComponentScale().Z)
		{
			ResetCapsuleAndMesh();
		}
		else
		{
			SetCapsuleAndKeepMesh(TargetHeight * 0.8f);
		}
	}
}

void UGBWFlowMoveComponent::AdjustCapsuleAndKeepMeshByCurrentLocation(FGBWFlowMoveScene FlowMoveScene)
{
	if (FlowMoveScene.ActorLocationScene.FloorAndRoof.Floor.IsNearlyZero() || FlowMoveScene.ActorLocationScene.FloorAndRoof.Roof.IsNearlyZero())
	{
		ResetCapsuleAndMesh();
	}
	else
	{
		const float TargetHeight = FMath::Abs((FlowMoveScene.ActorLocationScene.FloorAndRoof.Floor - FlowMoveScene.ActorLocationScene.FloorAndRoof.Roof).Z);
		if (TargetHeight / 2.0f > CapsuleHalfHeightCache * TaskState.OwnerCharacter->GetCapsuleComponent()->GetComponentScale().Z)
		{
			ResetCapsuleAndMesh();
		}
		else
		{
			SetCapsuleAndKeepMesh(TargetHeight * 0.9f);
		}
	}
}

float UGBWFlowMoveComponent::GetActorScale() const
{
	float Res = 1.0f;
	if (TaskState.OwnerCharacter)
	{
		const FVector Scale = TaskState.OwnerCharacter->GetActorScale3D();
		Res =  (Scale.X + Scale.Y + Scale.Z) / 3.0f;
	}

	return Res==0.0f? 1.0f : Res;
}

bool UGBWFlowMoveComponent::CheckUpdateValid()
{
	if (!TaskState.OwnerCharacter)
	{
		TaskState.OwnerCharacter = Cast<ACharacter>(GetOwner());
	}
	if (!TaskState.OwnerCharacter)
	{
		return false;
	}
	
	if (!IsValid(FlowMoveBrain))
	{
		return false;
	}

	if (!bIsNetInit)
	{
		NetInit();
	}
	
	return bIsNetInit;
}

void UGBWFlowMoveComponent::EnsureRMSActivation()
{
	if (!UGBWRMSBPFuncLib::GBWRMS_IsRMSCommonActive(TaskState.OwnerCharacter->GetCharacterMovement()))
	{
		FName RMSInsName;
		UGBWRMSBPFuncLib::GBWRMS_Common(
						RMSInsName,
						this,
						TaskState.OwnerCharacter->GetCharacterMovement(),
						true);
	}

	FRootMotionSourceGroup RmsArr = TaskState.OwnerCharacter->GetCharacterMovement()->CurrentRootMotion;
	for (const auto Rms:RmsArr.RootMotionSources)
	{
		if (Rms->InstanceName != UGBWRMSBPFuncLib::GBWRMS_GetRMSCommonInstanceName())
		{
			TaskState.OwnerCharacter->GetCharacterMovement()->RemoveRootMotionSource(Rms->InstanceName);
		}
	}
}

void UGBWFlowMoveComponent::UpdateCharacterState(float DeltaTime)
{
	if (TaskState.OwnerCharacter->GetCharacterMovement()->MovementMode != TaskState.MovementMode)
	{
		OnEvent(FFlowMoveEvent(TaskState.OwnerCharacter->GetCharacterMovement()->MovementMode));
		if (IsInServer())
		{
			TaskState.MovementMode = TaskState.OwnerCharacter->GetCharacterMovement()->MovementMode;
		}
	}
}

void UGBWFlowMoveComponent::UpdateScript(float DeltaTime)
{
	if (!FlowMoveBrain)
	{
		return;
	}
	if (IsInServer())
	{
		TaskState.CheckScriptTag();
		for (auto Item :  FlowMoveBrain->FlowMoveScriptSet)
		{
			if (Item.Value)
			{
				if (TaskState.ScriptReady.HasTag(Item.Key))
				{
					UpdateScript_Server(DeltaTime, Item.Key, EGBWFlowMoveEventType::Init);
					TaskState.ReadyScriptTag(Item.Key);
				}
				else if (TaskState.ScriptToEnd.HasTag(Item.Key))
				{
					UpdateScript_Server(DeltaTime, Item.Key, EGBWFlowMoveEventType::End);
					TaskState.FinishScriptTag(Item.Key);
				}
			}
		}
	}
	
	for (auto Item :  FlowMoveBrain->FlowMoveScriptSet)
	{
		if (Item.Value)
		{
			if (TaskState.ScriptNow.HasTag(Item.Key))
			{
				Item.Value->Update(
					EGBWFlowMoveEventType::Update,
					TaskState.OwnerCharacter->HasAuthority()? EFlowMoveNetworkLocationType::Server : EFlowMoveNetworkLocationType::Client,
					UGBWFlowMoveFuncLib::IsLocalOwn(TaskState.OwnerCharacter),
					this
				);
			}
		}
	}
}

void UGBWFlowMoveComponent::UpdateScript_Server_Implementation(float DeltaTime, FGameplayTag ScriptKey, EGBWFlowMoveEventType EventType)
{
	UpdateScript_Multicast(DeltaTime, ScriptKey, EventType);
}

void UGBWFlowMoveComponent::UpdateScript_Multicast_Implementation(float DeltaTime, FGameplayTag ScriptKey, EGBWFlowMoveEventType EventType)
{
	for (auto Item :  FlowMoveBrain->FlowMoveScriptSet)
	{
		if (Item.Key == ScriptKey && Item.Value)
		{
			Item.Value->Update(
					EventType,
					TaskState.OwnerCharacter->HasAuthority()? EFlowMoveNetworkLocationType::Server : EFlowMoveNetworkLocationType::Client,
					UGBWFlowMoveFuncLib::IsLocalOwn(TaskState.OwnerCharacter),
					this
				);
		}
	}
}

void UGBWFlowMoveComponent::UpdateScene(float DeltaTime)
{
	if (!FlowMoveBrain)
	{
		return;
	}
	if (IsInServer())
	{
		TaskState.CheckPerceptronTag();
	}

	for (auto Item :  FlowMoveBrain->FlowMovePerceptronSet)
	{
		if (Item.Value)
		{
			if (TaskState.PerceptronReady.HasTag(Item.Key))
			{
				UpdateScene_Server(DeltaTime, Item.Key, EGBWFlowMoveEventType::Init);
				TaskState.ReadyPerceptronTag(Item.Key);
			}
			else if (TaskState.PerceptronNow.HasTag(Item.Key))
			{
				if (IsInServer())
				{
					FGBWFlowMoveScene FlowMoveSceneResult = FGBWFlowMoveScene();
					if (Item.Value->GetThePerceptionResult(
						FlowMoveSceneResult,
						EGBWFlowMoveEventType::Update,
						TaskState.OwnerCharacter,
						this))
					{
						SetFlowMoveScene(
							FlowMoveSceneResult,
							Item.Key,
							false);
					}
					else
					{
						SetFlowMoveScene(
							FlowMoveSceneResult,
							Item.Key,
							true);
					}
				}
				Item.Value->Update(Item.Key,EGBWFlowMoveEventType::Update,this);
			}
			else if (TaskState.PerceptronToEnd.HasTag(Item.Key))
			{
				UpdateScene_Server(DeltaTime, Item.Key, EGBWFlowMoveEventType::End);
				TaskState.FinishPerceptronTag(Item.Key);
			}
		}
	}
}

void UGBWFlowMoveComponent::UpdateScene_Server_Implementation(float DeltaTime, FGameplayTag PerceptionKey,
	EGBWFlowMoveEventType EventType)
{
	UpdateScene_Multicast(DeltaTime, PerceptionKey, EventType);
}

void UGBWFlowMoveComponent::UpdateScene_Multicast_Implementation(float DeltaTime, FGameplayTag PerceptionKey,
	EGBWFlowMoveEventType EventType)
{
	for (auto Item :  FlowMoveBrain->FlowMovePerceptronSet)
	{
		if (Item.Key == PerceptionKey && Item.Value)
		{
			if (IsInServer())
			{
				FGBWFlowMoveScene FlowMoveSceneResult = FGBWFlowMoveScene();
				if (Item.Value->GetThePerceptionResult(
					FlowMoveSceneResult,
					EventType,
					TaskState.OwnerCharacter,
					this))
				{
					if (EventType == EGBWFlowMoveEventType::End)
					{
						SetFlowMoveScene(
							FlowMoveSceneResult,
							Item.Key,
							true);
					}
					else
					{
						SetFlowMoveScene(
							FlowMoveSceneResult,
							Item.Key,
							false);	
					}
				}
				else
				{
					SetFlowMoveScene(
						FlowMoveSceneResult,
						Item.Key,
						true);
				}
			}

			Item.Value->Update(PerceptionKey, EventType,this);
		}
	}
}

void UGBWFlowMoveComponent::UpdateMoveControlParam(float DeltaTime)
{
	if (IsInServer() && TaskState.OwnerCharacter)
    {
		FVector OldVelocity = TaskState.RMSParamNow.GetLocation();
		OldVelocity.Z = 0.0f;
		FVector VelocityNow = TaskState.OwnerCharacter->GetVelocity();
		VelocityNow.Z = 0.0f;
		TaskState.LostSpeed = OldVelocity.Length() - VelocityNow.Length();
		
		FGuid ActionGuidOld = TaskState.MoveControlParam.ActionGuid;
		FGameplayTag TargetSceneTagOld = TaskState.MoveControlParam.ActionTargetScene;
		
		UAnimInstance* AnimIns = TaskState.OwnerCharacter->GetMesh()->GetAnimInstance();
		FGBWAnimPlayStateSet AnimPlayStateSet = FGBWAnimPlayStateSet();
		
		if (UGBWAnimFuncLib::GBWGetAnimPlayState(
			TaskState.OwnerCharacter,
			AnimPlayStateSet
			))
		{
			UGBWFlowMoveANS_ActionLock* FMANSAL = nullptr;
			UGBWFlowMoveANS_MoveControl* FMANSMC = nullptr;
			if (UAnimNotifyState* NotifyState = UGBWAnimFuncLib::GetNotifyStateByClass(AnimPlayStateSet,UGBWFlowMoveANS_MoveControl::StaticClass()))
			{
				FMANSMC = Cast<UGBWFlowMoveANS_MoveControl>(NotifyState);
			}
			if (FMANSMC)
			{
				if (AnimPlayStateSet.MainAnimPlayState.bIsReplay
					|| ActionGuidOld != FMANSMC->MoveControlSettings.ActionGuid
					|| FMANSMC->MoveControlSettings.ActionTargetScene != TargetSceneTagOld
					|| !CurrentActionTargetSceneInfo.bIsValid
					|| CurrentActionTargetSceneInfo.ActionTargetScene != FMANSMC->MoveControlSettings.ActionTargetScene)
				{
					/*if (!CurrentActionTargetSceneInfo.bIsValid
						|| CurrentActionTargetSceneInfo.ActionTargetScene != FMANSMC->MoveControlSettings.ActionTargetScene)
					{
						SetCurrentTargetScene(TaskState.MoveControlParam.ActionTargetScene);
					}*/
					
					TaskState.LastFlowMoveActionTag = TaskState.MoveControlParam.ActionTag;
					TaskState.CurrentFlowMoveActionTag = FMANSMC->MoveControlSettings.ActionTag;
					TaskState.CurrentFlowMoveExecutedTime = 0.0f;

					TaskState.LostSpeed = 0.0f;
				}

				SetMoveControl(FMANSMC->MoveControlSettings);
				TaskState.CurrentFlowMoveExecutedTime += DeltaTime;
			}
			else
			{
				//DeactiveMoveControl();
				/*if (TaskState.CurrentFlowMoveActionTag.IsValid())
				{
					TaskState.LastFlowMoveActionTag = TaskState.CurrentFlowMoveActionTag;
					TaskState.CurrentFlowMoveActionTag = FGameplayTag::EmptyTag;
				}
				TaskState.CurrentFlowMoveExecutedTime = 0.0f;*/
			}

			if (UAnimNotifyState* NotifyState = UGBWAnimFuncLib::GetNotifyStateByClass(AnimPlayStateSet,UGBWFlowMoveANS_ActionLock::StaticClass()))
			{
				FMANSAL = Cast<UGBWFlowMoveANS_ActionLock>(NotifyState);
			}
			if (FMANSAL)
			{
				ApplyActionLockStateSetting(true,FMANSAL->ActionLockSettings);
			}
			else
			{
				ApplyActionLockStateSetting(false,FGBWFlowMoveActionLockStateSettings());
			}
		}
		
		float ToTargetPointRate = 0.0f;
		float Gravity = 0.0f;
		float RotationLockToMoveDirectionYaw = 0.0f;
		float MoveSpeed = 0.0f;
		float MoveToDirectionSmoothSpeed = 0.0f;
		FVector PathOffset = FVector::ZeroVector;
		FRotator RotationOffset = FRotator::ZeroRotator;

		float ToTargetPointRate_Delta = 0.0f;
		float Gravity_Delta = 0.0f;
		float RotationLockToMoveDirectionYaw_Delta = 0.0f;
		float MoveSpeed_Delta = 0.0f;
		float MoveToDirectionSmoothSpeed_Delta = 0.0f;
		FVector PathOffset_Delta = FVector::ZeroVector;
		FRotator RotationOffset_Delta = FRotator::ZeroRotator;

		bool bHas_ToTargetPointRate = false;
		bool bHas_Gravity = false;
		bool bHas_RotationLockToMoveDirectionYaw = false;
		bool bHas_MoveSpeed = false;
		bool bHas_MoveToDirectionSmoothSpeed = false;
		bool bHas_PathOffset_X = false;
		bool bHas_PathOffset_Y = false;
		bool bHas_PathOffset_Z = false;
		bool bHas_RotationOffset_Pitch = false;
		bool bHas_RotationOffset_Yaw = false;
		bool bHas_RotationOffset_Roll = false;
		
		bool bIsActionNow = IsActionNow();
		
		if (FlowMoveBrain && bIsActionNow)
		{
			if (AnimIns)
			{
				FGBWAnimCurveValue AnimCurveValue = FGBWAnimCurveValue();
				
				AnimCurveValue = UGBWAnimFuncLib::GetCurveValueByName(AnimPlayStateSet,FlowMoveBrain->AnimCurveNameSetting.ToTargetPointRate);
				if (AnimCurveValue.CurveName.IsValid())
				{
					bHas_ToTargetPointRate = true;
					ToTargetPointRate = AnimCurveValue.Value;
					ToTargetPointRate_Delta = AnimCurveValue.DeltaValue;
				}
				AnimCurveValue = UGBWAnimFuncLib::GetCurveValueByName(AnimPlayStateSet,FlowMoveBrain->AnimCurveNameSetting.Gravity);
				if (AnimCurveValue.CurveName.IsValid())
				{
					bHas_Gravity = true;
					Gravity = AnimCurveValue.Value;
					Gravity_Delta = AnimCurveValue.DeltaValue;
				}
				AnimCurveValue = UGBWAnimFuncLib::GetCurveValueByName(AnimPlayStateSet,FlowMoveBrain->AnimCurveNameSetting.RotationLockToMoveDirectionYaw);
				if (AnimCurveValue.CurveName.IsValid())
				{
					bHas_RotationLockToMoveDirectionYaw = true;
					RotationLockToMoveDirectionYaw = AnimCurveValue.Value;
					RotationLockToMoveDirectionYaw_Delta = AnimCurveValue.DeltaValue;
				}
				AnimCurveValue = UGBWAnimFuncLib::GetCurveValueByName(AnimPlayStateSet,FlowMoveBrain->AnimCurveNameSetting.MoveSpeed);
				if (AnimCurveValue.CurveName.IsValid())
				{
					bHas_MoveSpeed = true;
					MoveSpeed = AnimCurveValue.Value;
					MoveSpeed_Delta = AnimCurveValue.DeltaValue;
				}
				AnimCurveValue = UGBWAnimFuncLib::GetCurveValueByName(AnimPlayStateSet,FlowMoveBrain->AnimCurveNameSetting.MoveToDirectionSmoothSpeed);
				if (AnimCurveValue.CurveName.IsValid())
				{
					bHas_MoveToDirectionSmoothSpeed = true;
					MoveToDirectionSmoothSpeed = AnimCurveValue.Value;
					MoveToDirectionSmoothSpeed_Delta = AnimCurveValue.DeltaValue;
				}
				AnimCurveValue = UGBWAnimFuncLib::GetCurveValueByName(AnimPlayStateSet,FlowMoveBrain->AnimCurveNameSetting.PathOffset_Forward);
				if (AnimCurveValue.CurveName.IsValid())
				{
					bHas_PathOffset_X = true;
					PathOffset.X = AnimCurveValue.Value;
					PathOffset_Delta.X = AnimCurveValue.DeltaValue;
				}
				AnimCurveValue = UGBWAnimFuncLib::GetCurveValueByName(AnimPlayStateSet,FlowMoveBrain->AnimCurveNameSetting.PathOffset_Right);
				if (AnimCurveValue.CurveName.IsValid())
				{
					bHas_PathOffset_Y = true;
					PathOffset.Y = AnimCurveValue.Value;
					PathOffset_Delta.Y = AnimCurveValue.DeltaValue;
				}
				AnimCurveValue = UGBWAnimFuncLib::GetCurveValueByName(AnimPlayStateSet,FlowMoveBrain->AnimCurveNameSetting.PathOffset_Up);
				if (AnimCurveValue.CurveName.IsValid())
				{
					bHas_PathOffset_Z = true;
					PathOffset.Z = AnimCurveValue.Value;
					PathOffset_Delta.Z = AnimCurveValue.DeltaValue;
				}
				AnimCurveValue = UGBWAnimFuncLib::GetCurveValueByName(AnimPlayStateSet,FlowMoveBrain->AnimCurveNameSetting.RotationOffset_Pitch);
				if (AnimCurveValue.CurveName.IsValid())
				{
					bHas_RotationOffset_Pitch = true;
					RotationOffset.Pitch = AnimCurveValue.Value;
					RotationOffset_Delta.Pitch = AnimCurveValue.DeltaValue;
				}
				AnimCurveValue = UGBWAnimFuncLib::GetCurveValueByName(AnimPlayStateSet,FlowMoveBrain->AnimCurveNameSetting.RotationOffset_Yaw);
				if (AnimCurveValue.CurveName.IsValid())
				{
					bHas_RotationOffset_Yaw = true;
					RotationOffset.Yaw = AnimCurveValue.Value;
					RotationOffset_Delta.Yaw = AnimCurveValue.DeltaValue;
				}
				AnimCurveValue = UGBWAnimFuncLib::GetCurveValueByName(AnimPlayStateSet,FlowMoveBrain->AnimCurveNameSetting.RotationOffset_Roll);
				if (AnimCurveValue.CurveName.IsValid())
				{
					bHas_RotationOffset_Roll = true;
					RotationOffset.Roll = AnimCurveValue.Value;
					RotationOffset_Delta.Roll = AnimCurveValue.DeltaValue;
				}
			}
		}

		TaskState.MoveControlParam.DeltaPathOffset = PathOffset_Delta;
		TaskState.MoveControlParam.PathOffset = PathOffset;

		TaskState.MoveControlParam.DeltaToTargetPointRate = ToTargetPointRate_Delta;
		TaskState.MoveControlParam.ToTargetPointRate = ToTargetPointRate;

		float GravityNow = TaskState.MoveControlParam.Gravity;
        if (!TaskState.MoveControlParam.bIsLock_Gravity)
        {
        	TaskState.MoveControlParam.TargetGravity = !bIsActionNow || !bHas_Gravity? TaskState.MoveControlParam.TargetGravity : Gravity;
        }
		TaskState.MoveControlParam.Gravity = UKismetMathLibrary::FInterpTo(
			TaskState.MoveControlParam.Gravity,
			TaskState.MoveControlParam.TargetGravity,
			DeltaTime, TaskState.MoveControlParam.GravitySmoothSpeed);
		TaskState.MoveControlParam.DeltaGravity = TaskState.MoveControlParam.Gravity - GravityNow;

		if (!TaskState.MoveControlParam.bIsLock_RotationLockToMoveDirectionYaw)
		{
			TaskState.MoveControlParam.DeltaRotationLockToMoveDirectionYaw = !bIsActionNow || !bHas_RotationLockToMoveDirectionYaw? 0.0f : RotationLockToMoveDirectionYaw_Delta;
			TaskState.MoveControlParam.RotationLockToMoveDirectionYaw = !bIsActionNow || !bHas_RotationLockToMoveDirectionYaw? 0.0f : RotationLockToMoveDirectionYaw;
		}

		float MoveSpeedNow = TaskState.MoveControlParam.MoveSpeed;
		if (!TaskState.MoveControlParam.bIsLock_MoveSpeed)
		{
			if (!bIsActionNow || !bHas_MoveSpeed || TaskState.MoveControlParam.ActionGuid != ActionGuidOld)
			{
				FVector Velocity = TaskState.OwnerCharacter->GetVelocity();
				Velocity.Z = 0.0f;
				TaskState.MoveControlParam.TargetMoveSpeed = Velocity.Length();
				TaskState.MoveControlParam.MoveSpeed = UKismetMathLibrary::FInterpTo(
					TaskState.MoveControlParam.MoveSpeed,
					Velocity.Length(),
					DeltaTime, TaskState.MoveControlParam.MoveSpeedSmoothSpeed);
			}
			else
			{
				TaskState.MoveControlParam.TargetMoveSpeed = MoveSpeed;
				TaskState.MoveControlParam.MoveSpeed = UKismetMathLibrary::FInterpTo(
					TaskState.MoveControlParam.MoveSpeed,
					TaskState.MoveControlParam.TargetMoveSpeed,
					DeltaTime, TaskState.MoveControlParam.MoveSpeedSmoothSpeed);
			}
		}
		else
		{
			TaskState.MoveControlParam.MoveSpeed = UKismetMathLibrary::FInterpTo(
				TaskState.MoveControlParam.MoveSpeed,
				TaskState.MoveControlParam.TargetMoveSpeed,
				DeltaTime, TaskState.MoveControlParam.MoveSpeedSmoothSpeed);
		}
		TaskState.MoveControlParam.DeltaMoveSpeed = TaskState.MoveControlParam.MoveSpeed - MoveSpeedNow;

		if (!TaskState.MoveControlParam.bIsLock_MoveToDirectionSmoothSpeed)
		{
			TaskState.MoveControlParam.DeltaMoveToDirectionSmoothSpeed =!bIsActionNow || !bHas_MoveToDirectionSmoothSpeed? 0.0f : MoveToDirectionSmoothSpeed_Delta;
			TaskState.MoveControlParam.MoveToDirectionSmoothSpeed =!bIsActionNow || !bHas_MoveToDirectionSmoothSpeed? 0.0f : MoveToDirectionSmoothSpeed;
		}
		
		if (bIsActionNow)
		{
			if (!bHas_RotationOffset_Pitch && !bHas_RotationOffset_Yaw && !bHas_RotationOffset_Roll)
			{
				TaskState.MoveControlParam.DeltaRotationOffset = FRotator::ZeroRotator;
			}
			else if (TaskState.MoveControlParam.ActionGuid == ActionGuidOld)
			{
				TaskState.MoveControlParam.DeltaRotationOffset = RotationOffset_Delta;//UKismetMathLibrary::NormalizedDeltaRotator(RotationOffset ,TaskState.MoveControlParam.RotationOffset);
			}
		}
		TaskState.MoveControlParam.RotationOffset = !bIsActionNow || (!bHas_RotationOffset_Pitch && !bHas_RotationOffset_Yaw && !bHas_RotationOffset_Roll)? FRotator::ZeroRotator : RotationOffset;
		
		TaskState.MoveControlParam.AnimPlayStateSet = AnimPlayStateSet;

        if (FlowMoveBrain)
        {
        	FRotator CurrentRt = TaskState.MoveControlParam.MoveToDirection.Rotation();
        	FRotator TargetRt = GetMoveToVector(
									DeltaTime,
									FlowMoveBrain->bUseDefaultMoveVectorWhenZero,
									FlowMoveBrain->DefaultMoveVector
								).Rotation();
        	FRotator ResRt = UKismetMathLibrary::RInterpTo(CurrentRt,TargetRt,DeltaTime,TaskState.MoveControlParam.MoveToDirectionSmoothSpeed);
        	FVector Res_MoveToDirection  = ResRt.Vector();
        	Res_MoveToDirection.Normalize();
            if (GetMoveVector().IsNearlyZero())
            {
            	Res_MoveToDirection *= 0.01f;
            }
        	TaskState.MoveControlParam.MoveToDirection = Res_MoveToDirection;
        }
    }
}

void UGBWFlowMoveComponent::PostureAdjust(const float DeltaTime) const
{
	if (TaskState.MoveControlParam.bIsLock_UsePostureAdjust)
	{
		const FRotator RotationNow = TaskState.OwnerCharacter->GetCapsuleComponent()->GetRelativeRotation();
	
		FVector Velocity = FVector(TaskState.ControlVector.X,TaskState.ControlVector.Y,0).Rotation().RotateVector(TaskState.MoveVector);
		Velocity.Z = 0.0f;
		Velocity.Normalize();
		const FRotator CurrentRotation = TaskState.MoveControlParam.MoveToDirection.Rotation();
		const FRotator TargetRotation = Velocity.Rotation();
		const float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw,TargetRotation.Yaw);
		
		TaskState.OwnerCharacter->GetCapsuleComponent()->SetRelativeRotation(
			FRotator(
				0.0f,
				RotationNow.Yaw,
				FMath::Min(1.0f,TaskState.OwnerCharacter->GetVelocity().Length() / (TaskState.MoveControlParam.PostureAdjustReferenceSpeed> 0? TaskState.MoveControlParam.PostureAdjustReferenceSpeed : 1000.0f))
				* FMath::FInterpTo(
					RotationNow.Roll,
					FMath::ClampAngle(DeltaYaw,-TaskState.MoveControlParam.PostureAdjustAmplitude,TaskState.MoveControlParam.PostureAdjustAmplitude),
					DeltaTime,
					3.0f)));
	}
	else
	{
		RecoveryPosture();
	}
}

void UGBWFlowMoveComponent::RecoveryPosture() const
{
	const FRotator RotationNow = TaskState.OwnerCharacter->GetCapsuleComponent()->GetRelativeRotation();
	TaskState.OwnerCharacter->GetCapsuleComponent()->SetRelativeRotation(
		FRotator(
			0.0f,
			RotationNow.Yaw,
			0));
}

void UGBWFlowMoveComponent::MovingAdjust()
{
	if (TaskState.OwnerCharacter && IsInServer())
	{
		float Speed = 0.0f;
		FVector Velocity = TaskState.OwnerCharacter->GetVelocity();
		Velocity.Z = 0.0f;
		Speed = Velocity.Length()  / GetActorScale();

		float NewRate = 1.0f;
		if (GetMoveControlParam().AnimPlayRateAdjust.bUseSpeedAdjustAnimPlayRate)
		{
			if (GetMoveControlParam().AnimPlayRateAdjust.AnimPlayRateAdjustReferenceSpeed > 0)
			{
				NewRate = Speed / GetMoveControlParam().AnimPlayRateAdjust.AnimPlayRateAdjustReferenceSpeed;
				if (NewRate >= 1)
				{
					NewRate = 1 + FMath::Abs(NewRate-1) * GetMoveControlParam().AnimPlayRateAdjust.AdjustAnimPlayRateScale;
				}
				else
				{
					NewRate = FMath::Max(0.1f, 1 - FMath::Abs(NewRate-1) * GetMoveControlParam().AnimPlayRateAdjust.AdjustAnimPlayRateScale);
				}

				NewRate = UKismetMathLibrary::FClamp(
				NewRate,
				GetMoveControlParam().AnimPlayRateAdjust.Min_AnimPlayRate,
				GetMoveControlParam().AnimPlayRateAdjust.Max_AnimPlayRate);
			}
		}

		TaskState.MoveControlParam.AnimPlayRateNow = NewRate;
	}

	if (TaskState.OwnerCharacter
		&& TaskState.OwnerCharacter->GetMesh()
		&& TaskState.OwnerCharacter->GetMesh()->GetAnimInstance())
	{
		if (TaskState.OwnerCharacter->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
		{
			TaskState.OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_SetPlayRate(nullptr,TaskState.MoveControlParam.AnimPlayRateNow);
		}
		else
		{
			TaskState.OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_SetPlayRate(nullptr,1.0f);
		}
	}
}

void UGBWFlowMoveComponent::UpdateRMS(float DeltaTime)
{
	if (IsInServer())//if (IsInServer() || IsLocalOwn())
	{
		TaskState.MakeVelocity();
		
		FTransform NewParam = FTransform();
		if (!IsActionNow())
		{
			NewParam = FTransform();
		}

		if (DeltaTime > SMALL_NUMBER && FlowMoveBrain)
		{
			FVector TargetPoint = GetActionTargetPoint(GetMoveControlParam().ActionTargetPoint);
			const bool bIsToTargetPoint = GetMoveControlParam().ToTargetPointRate > 0 && !TargetPoint.IsZero();

			const FVector TargetRealDirection = TargetPoint - CurrentActionTargetSceneInfo.BeginTransform.GetLocation();
			FVector TargetDirection = TargetRealDirection;
			TargetDirection.Z = 0.0f;
			TargetDirection.Normalize();

			const FVector DtPathOffset = (bIsToTargetPoint? TargetDirection : TaskState.MoveControlParam.MoveToDirection).Rotation().RotateVector(GetMoveControlParam().DeltaPathOffset);
			
			const FRotator DtRotationOffset = GetMoveControlParam().DeltaRotationOffset;
			
			FVector CurrentTargetDtPathOffset = FVector::ZeroVector;
			if (CurrentActionTargetSceneInfo.bIsValid && bIsToTargetPoint && IsActionNow())
			{
				CurrentTargetDtPathOffset = TargetRealDirection * GetMoveControlParam().DeltaToTargetPointRate;
			}

			const float FallDistance = 0.5f * 10000.0f * GetMoveControlParam().Gravity * DeltaTime * DeltaTime;

			FVector DtMove = CurrentTargetDtPathOffset + DtPathOffset + FVector(0,0,FallDistance);
			DtMove = DtMove + TaskState.MoveControlParam.MoveToDirection * GetMoveControlParam().MoveSpeed * DeltaTime;
			
			//Compensate for lost speed in the current speed direction
			if (TaskState.MoveControlParam.CompensateLostSpeedToCurrentSpeedDirection)
			{
				FVector OldVelocity = TaskState.RMSParamNow.GetLocation();
				OldVelocity.Z = 0.0f;
				FVector VelocityNow = TaskState.OwnerCharacter->GetVelocity();
				VelocityNow.Z = 0.0f;
				float AddSpeed = VelocityNow.Length()>0.0f? (TaskState.LostSpeed * OldVelocity.Length() / VelocityNow.Length()) : 0.0f;
				VelocityNow.Normalize();
				DtMove = DtMove + VelocityNow * AddSpeed * DeltaTime;
			}
			
			const FVector Force = DtMove / DeltaTime;
			
			const FRotator CurrentRotation = TaskState.OwnerCharacter->GetActorRotation();
			FRotator RotationDt = FRotator::ZeroRotator;

			//rotation
			FRotator TargetRotation = TaskState.ForwardVector.Rotation();
			if (TaskState.MoveControlParam.HasActiveAnimCurveState(FlowMoveBrain->AnimCurveNameSetting.RotationLockToMoveDirectionYaw) ||
				TaskState.MoveControlParam.bIsLock_RotationLockToMoveDirectionYaw)
			{
				TargetRotation = GetDirectionVector(GetMoveControlParam().RotationLockTo).Rotation();
				TargetRotation = UKismetMathLibrary::ComposeRotators(
					FRotator(0.0f,
					GetMoveControlParam().RotationLockToMoveDirectionYaw
					,0.0f),
					TargetRotation);
			}

			/*//the rotation of inertia
			if (TaskState.MoveControlParam.bIsLock_UseInertia
				&& TaskState.CurrentFlowMoveExecutedTime <= TaskState.MoveControlParam.InertiaTime)
			{
				TargetRotation = UKismetMathLibrary::ComposeRotators(
					TargetRotation * (TaskState.CurrentFlowMoveExecutedTime / TaskState.MoveControlParam.InertiaTime),
					CurrentActionTargetSceneInfo.BeginTransform.Rotator() * (1.0f - TaskState.CurrentFlowMoveExecutedTime / TaskState.MoveControlParam.InertiaTime)
					);
			}*/
			
			const float DeltaYaw = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation ,CurrentRotation).Yaw;
			RotationDt = FRotator{
				0.0f,
				FMath::FInterpTo(0, DeltaYaw, DeltaTime, GetMoveControlParam().RotationLockToSmoothSpeed),
				0.0f};

			RotationDt = UKismetMathLibrary::ComposeRotators(RotationDt, DtRotationOffset);
			
			NewParam = UKismetMathLibrary::ComposeTransforms(NewParam,FTransform(RotationDt, Force));
		}
		else
		{
			NewParam = FTransform();
		}

		NewParam.SetLocation(NewParam.GetLocation() * GetActorScale());

		/*//the force of inertia
		if (TaskState.MoveControlParam.bIsLock_UseInertia
			&& TaskState.CurrentFlowMoveExecutedTime <= TaskState.MoveControlParam.InertiaTime)
		{
			NewParam.SetLocation(
				NewParam.GetLocation() * TaskState.CurrentFlowMoveExecutedTime / TaskState.MoveControlParam.InertiaTime
				+ CurrentActionTargetSceneInfo.BeginVelocity * (1.0f - TaskState.CurrentFlowMoveExecutedTime / TaskState.MoveControlParam.InertiaTime)
			);

			GEngine->AddOnScreenDebugMessage(465451,20,FColor::Green,FString::Printf(TEXT("%.2f + %.2f")
				,TaskState.CurrentFlowMoveExecutedTime / TaskState.MoveControlParam.InertiaTime
				,1.0f - TaskState.CurrentFlowMoveExecutedTime / TaskState.MoveControlParam.InertiaTime
				));
		}*/

		if (!TaskState.MoveControlParam.bIsLock_MaxSpeed
			||(TaskState.MoveControlParam.bIsLock_MaxSpeed
				&& NewParam.GetLocation().Length() <= TaskState.MoveControlParam.MaxMoveSpeed))
		{
			if (IsInServer())
			{
				//SetRMSParamNow(NewParam);
				TaskState.RMSParamNow = NewParam;
				TaskState.RMSParamNow_Local = NewParam;
			}
			if (IsLocalOwn())
			{
				TaskState.RMSParamNow_Local = NewParam;
			}
		}

		//Constrain Movement
		if (TaskState.MoveControlParam.bIsLock_ConstrainMoveToTargetPlane
			&& TaskState.CurrentFlowMoveExecutedTime <= TaskState.MoveControlParam.ConstrainMoveTime)
		{
			FVector PlaneNormal;
			FVector PlaneOrigin;
			if (CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.GetPlaneConstraintSetting(PlaneNormal, PlaneOrigin))
			{
				SetMovementConstrain(PlaneNormal, PlaneOrigin);
			}
		}
		else
		{
			ResetMovementConstrain();
		}
	}

	//PostureAdjust
	PostureAdjust(DeltaTime);

	//MovingAdjust
	MovingAdjust();
}

void UGBWFlowMoveComponent::UpdateCurrentAction(float DeltaTime)
{
	if (TaskState.CurrentFlowMoveActionTag.IsValid())
	{
		OnActionUpdate.Broadcast(TaskState.CurrentFlowMoveActionTag);
		OnEvent(FFlowMoveEvent(EFlowMoveEventType::OnActionUpdate,TaskState.CurrentFlowMoveActionTag));
	}
}

void UGBWFlowMoveComponent::OnActiveEvent()
{
	OnActive.Broadcast(TaskState);
	OnEvent(FFlowMoveEvent(EFlowMoveEventType::OnActive));
}
//这个Tick函数调用了许多运行时的东西
void UGBWFlowMoveComponent::FlowMoveTick(float DeltaTime)
{
	//Clear FlowMoveEvent
	TaskState.ClearFlowMoveEvent();

	//Check Update Valid
	if (!CheckUpdateValid())
	{
		return;
	}

	//Check CheckActive
	CheckActive();

	//Check Input
	CheckInput();

	//Check ViewMode
	CheckViewMode();
	
	//Update CharacterNetState
	UpdateCharacterState(DeltaTime);
	
	//Update Script
	UpdateScript(DeltaTime);

	//Update Scene
	UpdateScene(DeltaTime);

	//Check Stop
	CheckStop();

	OnUpdate.Broadcast(TaskState);
	OnEvent(FFlowMoveEvent(EFlowMoveEventType::OnUpdate));
	
	//Update Current Action
	UpdateCurrentAction(DeltaTime);

	//Update MoveControlParam
	UpdateMoveControlParam(DeltaTime);

	//Update RMS
	UpdateRMS(DeltaTime);
}
//在蓝图那个地方使用了PS（Powerful State）bool的一个改动 这里检测到并且激活这个Active组件
void UGBWFlowMoveComponent::CheckActive()
{
	if (IsInServer())
	{
		bool IsActive = false;
		bool IsWaitForCurrentActionFinished = true;
		if (FlowMoveBrain)
		{
			FlowMoveBrain->GetFMIsActive(
				TaskState.OwnerCharacter,
				this,
				TaskState.FrameDeltaTime,
				TaskState,
				IsActive,
				IsWaitForCurrentActionFinished);
		}

		if (IsActive && (!IsFlowMoveActive() || IsFlowMoveStopping()))
		{
			ActiveFlowMove();
		}
		else if (!IsActive && IsFlowMoveActive())
		{
			StopFlowMove(IsWaitForCurrentActionFinished);
		}

		/*
		if (GFrameCounter % 60 == 0)
		{
			SetActive_Multicast(IsActive);
		}*/
	}
}

void UGBWFlowMoveComponent::CheckStop()
{
	if (IsInServer())
	{
		if (TaskState.bIsStopping && !IsAnyActionLock())
		{
			Stop();
		}
	}
}

void UGBWFlowMoveComponent::CheckInput(bool bReset)
{
	if (IsLocalPlayer() && FlowMoveBrain)
	{
		FVector MoveVector;
		FVector ControlVector;
		bool IsMoveInput;
		bool IsControlInput;
		
		FlowMoveBrain->GetFMMoveVector(
			TaskState.OwnerCharacter,
			this,
			TaskState.FrameDeltaTime,
            TaskState,
            IsMoveInput,
            MoveVector);
		FlowMoveBrain->GetFMControlVector(
			TaskState.OwnerCharacter,
			this,
			TaskState.FrameDeltaTime,
			TaskState,
			IsControlInput,
			ControlVector);
		if (IsMoveInput)
		{
			SetMoveVector(
				MoveVector,
				FlowMoveBrain->MoveVectorZeroFaultToleranceDuration);
		}
		if (IsControlInput)
		{
			SetControlVector(ControlVector);
		}

		const FTransform TraceT = TaskState.OwnerCharacter->GetTransform();
		FVector Direction = UKismetMathLibrary::ProjectVectorOnToPlane(TaskState.ControlVector, TaskState.OwnerCharacter->GetActorForwardVector());
		FVector DirectionResult = UKismetMathLibrary::InverseTransformDirection(TraceT,Direction);
		if (const float Pitch = DirectionResult.Z * 90.0f; -FlowMoveBrain->PerceptionInputSettings.DownAngleThreshold< Pitch && Pitch < FlowMoveBrain->PerceptionInputSettings.UpAngleThreshold)
		{
			DirectionResult.Z =0;
		}
		if (const float Yaw = DirectionResult.Y * 90.0f; -FlowMoveBrain->PerceptionInputSettings.LeftAndRightAngleThreshold< Yaw && Yaw < FlowMoveBrain->PerceptionInputSettings.LeftAndRightAngleThreshold)
		{
			DirectionResult.Y =0;
		}
		DirectionResult.X = 0;
		Direction.Normalize();
		const FVector PerceptionVector = DirectionResult;
		SetPerceptionVector(PerceptionVector);
	}
}

void UGBWFlowMoveComponent::CheckViewMode()
{
	if (IsInServer())
	{
		if (TaskState.ViewMode == EFlowMoveCharacterViewMode::TP_ActorLockMode
			&& TaskState.FocusActorComponent)
		{
			FVector ForwardVector = TaskState.FocusActorComponent->GetComponentLocation() - TaskState.OwnerCharacter->GetActorLocation();
			ForwardVector.Z = 0.0f;
			ForwardVector.Normalize();
			
			TaskState.ForwardVector = ForwardVector;
		}
		else if (TaskState.ViewMode == EFlowMoveCharacterViewMode::TP_ForwardLockMode)
		{
			FVector ControlVector = GetControlVector();
			ControlVector.Z = 0.0f;
			ControlVector.Normalize();
			TaskState.ForwardVector = ControlVector;
		}
		else if (TaskState.ViewMode == EFlowMoveCharacterViewMode::TP_FreeMode)
		{
			TaskState.ForwardVector = GetMoveControlParam().MoveToDirection;
		}
		else
		{
			TaskState.ForwardVector = GetMoveControlParam().MoveToDirection;
			if (TaskState.ViewMode != DefaultViewMode)
			{
				SetFlowMoveViewMode(DefaultViewMode,false);
			}
		}
	}
}

void UGBWFlowMoveComponent::NetInit()
{
	TaskState.OwnerCharacter = Cast<ACharacter>(GetOwner());
	TaskState.Timer = 0.0f;
	TaskState.FlowMoveComponent = this;

	if (IsInServer())
	{
		if (!TaskState.FlowMoveComponentGuid.IsValid())
		{
			TaskState.FlowMoveComponentGuid = FGuid::NewGuid();
		}
	}

	if (TaskState.OwnerCharacter && FlowMoveBrain)
	{
		CapsuleHalfHeightCache = TaskState.OwnerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		MeshRelativeLocationCache = TaskState.OwnerCharacter->GetMesh()->GetRelativeLocation();
		CapsuleRadiusCache = TaskState.OwnerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
		bUseControllerRotationYawCache = TaskState.OwnerCharacter->bUseControllerRotationYaw;
		MovementCompMaxAccelerationCache = TaskState.OwnerCharacter->GetCharacterMovement()->GetMaxAcceleration();
		bIsNetInit = true;

		IsLocalPlayer(true);
		IsAIPlayer(true);
		IsLocalOwn(true);
		IsInServer(true);
	}
}

void UGBWFlowMoveComponent::SetRMSParamNow(FTransform NewRMSParam)
{
	SetRMSParamNow_Server(NewRMSParam);
}

void UGBWFlowMoveComponent::SetRMSParamNow_Server_Implementation(FTransform NewRMSParam)
{
	SetRMSParamNow_Multicast(NewRMSParam);
}

void UGBWFlowMoveComponent::SetRMSParamNow_Multicast_Implementation(FTransform NewRMSParam)
{
	TaskState.RMSParamNow = NewRMSParam;
}

void UGBWFlowMoveComponent::SetFlowMoveViewMode(EFlowMoveCharacterViewMode NewViewMode, bool bAsDefaultViewMode)
{
	if (IsLocalOwn())
	{
		SetFlowMoveViewMode_Server(NewViewMode, bAsDefaultViewMode);
	}
}
void UGBWFlowMoveComponent::SetFlowMoveViewMode_Server_Implementation(
	EFlowMoveCharacterViewMode NewViewMode, bool bAsDefaultViewMode)
{
	if (TaskState.ViewMode != NewViewMode)
	{
		TaskState.ViewMode = NewViewMode;
		SetFlowMoveViewMode_Multicast(NewViewMode);
	}
	if (bAsDefaultViewMode && DefaultViewMode != NewViewMode)
	{
		DefaultViewMode = NewViewMode;
	}
}
void UGBWFlowMoveComponent::SetFlowMoveViewMode_Multicast_Implementation(EFlowMoveCharacterViewMode NewViewMode)
{
	SetFlowMoveViewMode_Imp(NewViewMode);
}
void UGBWFlowMoveComponent::SetFlowMoveViewMode_Imp(EFlowMoveCharacterViewMode NewViewMode)
{
	OnEvent(FFlowMoveEvent(NewViewMode));
}

void UGBWFlowMoveComponent::SetFocusActor(USceneComponent* InComponent, bool bSetViewModeAsActorLock)
{
	if (IsLocalOwn())
	{
		SetFocusActor_Server(InComponent);
		if (InComponent && bSetViewModeAsActorLock)
		{
			SetFlowMoveViewMode(EFlowMoveCharacterViewMode::TP_ActorLockMode, false);
		}
		else
		{
			SetFlowMoveViewMode(DefaultViewMode, false);
		}
	}
}
void UGBWFlowMoveComponent::SetFocusActor_Server_Implementation(USceneComponent* InComponent)
{
	TaskState.FocusActorComponent = InComponent;
	SetFocusActor_Multicast(InComponent);
}
void UGBWFlowMoveComponent::SetFocusActor_Multicast_Implementation(USceneComponent* InComponent)
{
	SetFocusActor_Imp(InComponent);
}
void UGBWFlowMoveComponent::SetFocusActor_Imp(USceneComponent* InComponent)
{
	OnEvent(FFlowMoveEvent(InComponent));
}

void UGBWFlowMoveComponent::TryLockActor(FGameplayTag GetTargetFunctionTag)
{
	if (IsLocalOwn() && FlowMoveBrain)
	{
		UGBWFlowMoveGetFocusActor_Base* Fun = FlowMoveBrain->FlowMoveGetTargetFunctionSet.FindRef(GetTargetFunctionTag);
		if (Fun)
		{
			SetFocusActor(Fun->GetFocusActor(this),true);
		}
		else
		{
			SetFocusActor(nullptr,true);
		}
	}
}

bool UGBWFlowMoveComponent::CheckSceneChange(
	const FGBWFlowMoveScene& NewScene,
	FGameplayTag TargetScene,
	FGBWFlowMoveSceneChangeInfo& ChangeInfo)
{
	if (const int Index = TaskState.GetSupplementarySceneIndex(TargetScene); Index>=0)
	{
		return TaskState.SupplementaryFlowMoveScene[Index].Scene.CheckSceneChange(NewScene,ChangeInfo);
	}

	return false;
}

void UGBWFlowMoveComponent::OnStopEvent()
{
	if (!IsFlowMoveActive() || !FlowMoveBrain)
	{
		return;
	}
	
	TaskState.bIsActive = false;
	TaskState.bIsStopping = false;
	
	TaskState.OwnerCharacter->GetCharacterMovement()->MaxAcceleration = MovementCompMaxAccelerationCache;
	
	UGBWRMSBPFuncLib::GBWRMS_StopRMSCommon(TaskState.OwnerCharacter->GetCharacterMovement());
	
	Recovery();
	RecoveryPosture();

	if (IsInServer())
	{
		TaskState.PerceptronReady.Reset();
		FGameplayTagContainer ThePerceptronNow = TaskState.PerceptronNow;
		for (const auto Item :  ThePerceptronNow)
		{
			DeactivePerceptron(Item);
			UpdateScene(TaskState.FrameDeltaTime);
		}

		TaskState.ScriptReady.Reset();
		FGameplayTagContainer TheScriptNow = TaskState.ScriptNow;
		for (const auto Item :  TheScriptNow)
		{
			DeactiveScript(Item);
			UpdateScript(TaskState.FrameDeltaTime);
		}
	}

	OnStop.Broadcast(TaskState);
	OnEvent(FFlowMoveEvent(EFlowMoveEventType::OnStop));
}

void UGBWFlowMoveComponent::OnActionEndEvent(FGameplayTag ActionTag)
{
	OnActionEnd.Broadcast(ActionTag);
	OnEvent(FFlowMoveEvent(EFlowMoveEventType::OnActionEnd,ActionTag));
}

bool UGBWFlowMoveComponent::IsLocked(FGameplayTag ForActionTag) const
{
	return TaskState.ActionLockState.IsLock(ForActionTag);
}

void UGBWFlowMoveComponent::Recovery()
{
	TaskState.ClearCurrentFlowMoveAction();
	ResetCapsuleAndMesh();
	ResetMovementConstrain();
	ResetUseControllerRotationYaw();
}