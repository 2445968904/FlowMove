// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GBWFlowMoveObjects.h"
#include "GBWPowerfulStateComponent.h"
#include "Animation/BlendSpace1D.h"
#include "Components/ActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "RMS/GBWRMSInterface.h"
#include "GBWFlowMoveComponent.generated.h"


UCLASS(Blueprintable, editinlinenew, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBWFLOWMOVE_API UGBWFlowMoveComponent : public UActorComponent, public IGBWRMSInterface
{
	//两个广播委托 FlowMove 传入 State
	// FlowMoveAction 传入的是ActionTag
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFlowMoveDlg, FGBWFlowMoveState, FlowMoveState);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFlowMoveActionDlg, FGameplayTag, ActionTag);
	GENERATED_BODY()
    
public:	
	UGBWFlowMoveComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

// BEGIN IGBWRMSInterface 这是Resource Management System（分开看就是GBW RMS）
	//这个函数用于获取根运动方面的信息
	virtual bool GetRootMotionParamDirect(
		FTransform& Result,
		float SimulationTime,
		float MovementTickTime,
		const ACharacter& Character,
		const UCharacterMovementComponent& MoveComponent) override;
// END IGBWRMSInterface

// BEGIN Dlg
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "GBW|FlowMove")
	FFlowMoveDlg OnActive;
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "GBW|FlowMove")
	FFlowMoveDlg OnActiveFailed;
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "GBW|FlowMove")
	FFlowMoveDlg OnUpdate;
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "GBW|FlowMove")
	FFlowMoveDlg OnStop;
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "GBW|FlowMove")
	FFlowMoveActionDlg OnActionStart;
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "GBW|FlowMove")
	FFlowMoveActionDlg OnActionUpdate;
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "GBW|FlowMove")
	FFlowMoveActionDlg OnActionEnd;
// END Dlg

	UFUNCTION()
	void OnEvent(const FFlowMoveEvent& FlowMoveEvent);

// BEGIN Get Input
	/** 
	 * Automatically adjust the MoveVector, ControlVector, and PerceptionVector by specifying a target location.
	 * This is very useful for AI characters.
	 * Note that this method only takes effect when the role has network permissions.
	 * For example, for local players, players on the server, and AI players on the server.
	 * In other words, for AI Characters on the client, this method does not take effect.
	 */
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Input")
	void SetInputValueByTargetPoint(FVector TargetPoint);
	UFUNCTION(Reliable, Server, Category = "GBW|FlowMove|Input")
	void SetInputValueByTargetPoint_Server(FVector TargetPoint);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Input")
	void SetMoveVector(FVector NewMoveVector, float ZeroFaultToleranceDuration = 0.1f);
	UFUNCTION(Reliable, Server, Category = "GBW|FlowMove|Input")
	void SetMoveVector_Server(FVector NewMoveVector);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Input")
	FVector GetMoveVector(bool Consumed=true);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Input")
	void SetControlVector(FVector NewControlVector);
	UFUNCTION(Reliable, Server, Category = "GBW|FlowMove|Input")
	void SetControlVector_Server(FVector ControlVector);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Input")
	FVector GetControlVector();
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Input")
	void SetPerceptionVector(FVector NewPerceptionVector);
	UFUNCTION(Reliable, Server, Category = "GBW|FlowMove|Input")
	void SetPerceptionVector_Server(FVector NewPerceptionVector);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Input")
	FVector GetPerceptionVector();

	//这个函数会返回当前BlendSpace 的 方向（X） 速度(Y) 速率 （这个是一个2D的）
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Input")
	void GetMoveParamForBlendSpace(UBlendSpace* ForBS, float& Direction, float& Speed, float& PlayRate) const;
// END Get Input

// BEGIN FlowMove Control
	UFUNCTION(BlueprintCallable,Category = "GBW|FlowMove|Action")
	void StopFlowMove(bool WaitForCurrentActionFinished);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|Action")
	void StopFlowMove_Server(bool WaitForCurrentActionFinished);
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|FlowMove|Action")
	void StopFlowMove_Multicast(bool WaitForCurrentActionFinished);

	UFUNCTION(Category = "GBW|FlowMove|Action")
	void Stop();
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|Action")
	void Stop_Server();
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|FlowMove|Action")
	void Stop_Multicast();
	
	UFUNCTION(BlueprintCallable,Category = "GBW|FlowMove|Action")
	void ActiveFlowMove();
	UFUNCTION(Reliable, Server, Category = "GBW|FlowMove|Action")
	void Active_Server();
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|FlowMove|Action")
	void Active_Multicast();
	void Active_Imp();

	UFUNCTION(Reliable, Server, Category = "GBW|FlowMove|Action")
	void SetActive_Server(bool NewActive);
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|FlowMove|Action")
	void SetActive_Multicast(bool NewActive);

	//设置运动的约束（将运动限制在平面）
	UFUNCTION(Category = "GBW|FlowMove|Movement")
	void SetMovementConstrain(const FVector& PlaneNormal, const FVector& PlaneOrigin);
	UFUNCTION(Reliable, Server, Category = "GBW|FlowMove|Movement")
	void SetMovementConstrain_Server(FVector PlaneNormal, FVector PlaneOrigin);
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|FlowMove|Movement")
	void SetMovementConstrain_Multicast(FVector PlaneNormal, FVector PlaneOrigin);

	//重新设置运动的约束
	UFUNCTION(Category = "GBW|FlowMove|Movement")
	void ResetMovementConstrain();
	UFUNCTION(Reliable, Server, Category = "GBW|FlowMove|Movement")
	void ResetMovementConstrain_Server();
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|FlowMove|Movement")
	void ResetMovementConstrain_Multicast();

	UFUNCTION(Category = "GBW|FlowMove|Movement")
	void ResetUseControllerRotationYaw();
	UFUNCTION(Reliable, Server, Category = "GBW|FlowMove|Movement")
	void ResetUseControllerRotationYaw_Server();
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|FlowMove|Movement")
	void ResetUseControllerRotationYaw_Multicast();
// END FlowMove Control

// BEGIN FlowMove State
	
	void NetInit();
	//Resource Management System = RMS 资源管理系统
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|State")
	void SetRMSParamNow(FTransform NewRMSParam);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|State")
	void SetRMSParamNow_Server(FTransform NewRMSParam);
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|FlowMove|State")
	void SetRMSParamNow_Multicast(FTransform NewRMSParam);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|State")
	void SetFlowMoveViewMode(EFlowMoveCharacterViewMode NewViewMode, bool bAsDefaultViewMode);
	UFUNCTION(Reliable, Server, Category = "GBW|FlowMove|State")
	void SetFlowMoveViewMode_Server(EFlowMoveCharacterViewMode NewViewMode, bool bAsDefaultViewMode);
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|FlowMove|State")
	void SetFlowMoveViewMode_Multicast(EFlowMoveCharacterViewMode NewViewMode);
	void SetFlowMoveViewMode_Imp(EFlowMoveCharacterViewMode NewViewMode);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|State")
	void SetFocusActor(USceneComponent* InComponent, bool bSetViewModeAsActorLock);
	UFUNCTION(Reliable, Server, Category = "GBW|FlowMove|State")
	void SetFocusActor_Server(USceneComponent* InComponent);
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|FlowMove|State")
	void SetFocusActor_Multicast(USceneComponent* InComponent);
	void SetFocusActor_Imp(USceneComponent* InComponent);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|State")
	void TryLockActor(FGameplayTag GetTargetFunctionTag);

	//该函数的作用是检查新场景是否符合目标场景，如果是，则将变化信息存储到ChangeInfo中。函数的返回类型是bool，可能用于指示是否发生了场景变化
	bool CheckSceneChange(
		const FGBWFlowMoveScene& NewScene,
		FGameplayTag TargetScene,
		FGBWFlowMoveSceneChangeInfo& ChangeInfo);
	UFUNCTION(Category = "GBW|FlowMove|State")
	void SetFlowMoveScene(
		const FGBWFlowMoveScene& NewScene,
		FGameplayTag TargetScene,
		bool bIsRemoveSupplementaryScene);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|State")
	void SetFlowMoveScene_Server(
		FGBWFlowMoveScene NewScene,
		FGameplayTag TargetScene,
		bool bIsRemoveSupplementaryScene);
	void SetFlowMoveScene_Imp(
		const FGBWFlowMoveScene& NewScene,
		FGameplayTag TargetScene,
		bool bIsRemoveSupplementaryScene);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|State")
	void ApplyActionLockStateSetting(bool bHasActionLock,
		const FGBWFlowMoveActionLockStateSettings ActionLockStateSetting);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|State")
	void ApplyActionLockStateSetting_Server(bool bHasActionLock,
		const FGBWFlowMoveActionLockStateSettings ActionLockStateSetting);
	void ApplyActionLockStateSetting_Imp(bool bHasActionLock,
		const FGBWFlowMoveActionLockStateSettings& ActionLockStateSetting);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|State")
	void SetActionLockState(
		bool bHasActionLock,
		FGameplayTagQuery ActionToLock);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|State")
	void SetActionLockState_Server(
		bool bHasActionLock,
		FGameplayTagQuery ActionToLock);
	void SetActionLockState_Imp(
		bool bHasActionLock,
		const FGameplayTagQuery& ActionToLock);

	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|State")
	void SetFlowMoveScene_SceneType_Server(
		FName SceneType,
		FGameplayTag TargetScene);
	void SetFlowMoveScene_SceneType_Imp(
		const FName SceneType,
		FGameplayTag TargetScene);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|State")
	void SetFlowMoveScene_TargetActor_Server(
		AActor* TargetActor,
		FGameplayTag TargetScene);
	void SetFlowMoveScene_TargetActor_Imp(
		AActor* TargetActor,
		FGameplayTag TargetScene);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|State")
	void SetFlowMoveScene_TargetLocation_Server(
		FGBWFlowMovePointScene TargetLocation,
		FGameplayTag TargetScene);
	void SetFlowMoveScene_TargetLocation_Imp(
		const FGBWFlowMovePointScene& TargetLocation,
		FGameplayTag TargetScene);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|State")
	void SetFlowMoveScene_ActorLocation_Server(
		FGBWFlowMovePointScene ActorLocation,
		FGameplayTag TargetScene);
	void SetFlowMoveScene_ActorLocation_Imp(
		const FGBWFlowMovePointScene& ActorLocation,
		FGameplayTag TargetScene);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|State")
	void SetFlowMoveScene_LeftLocation_Server(
		FGBWFlowMovePointScene Location,
		FGameplayTag TargetScene);
	void SetFlowMoveScene_LeftLocation_Imp(
		const FGBWFlowMovePointScene& Location,
		FGameplayTag TargetScene);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|State")
	void SetFlowMoveScene_RightLocation_Server(
		FGBWFlowMovePointScene Location,
		FGameplayTag TargetScene);
	void SetFlowMoveScene_RightLocation_Imp(
		const FGBWFlowMovePointScene& Location,
		FGameplayTag TargetScene);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|State")
	void SetFlowMoveScene_Slope_Server(
		float Slope,
		FGameplayTag TargetScene);
	void SetFlowMoveScene_Slope_Imp(
		const float Slope,
		FGameplayTag TargetScene);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|State")
	void SetFlowMoveScene_PerceptionResult_Server(
		FGBWPerceptionResult PerceptionResult,
		FGameplayTag TargetScene);
	void SetFlowMoveScene_PerceptionResult_Imp(
		const FGBWPerceptionResult PerceptionResult,
		FGameplayTag TargetScene);

	UFUNCTION(Category = "GBW|FlowMove|State")
	void SetFlowMoveLastAction(
		FGameplayTag NewLastActionTag);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|State")
	void SetFlowMoveLastAction_Server(
		FGameplayTag NewLastActionTag);
	void SetFlowMoveLastAction_Imp(
		FGameplayTag NewLastActionTag);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Perceptron")
	void ActivePerceptron(const FGameplayTag PerceptronTag);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|Perceptron")
	void ActivePerceptron_Server(const FGameplayTag PerceptronTag);
	void ActivePerceptron_Imp(const FGameplayTag PerceptronTag);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Perceptron")
	void DeactivePerceptron(const FGameplayTag PerceptronTag);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|Perceptron")
	void DeactivePerceptron_Server(const FGameplayTag PerceptronTag);
	void DeactivePerceptron_Imp(const FGameplayTag PerceptronTag);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Perceptron")
	void ActiveScript(const FGameplayTag ScriptTag);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|Perceptron")
	void ActiveScript_Server(const FGameplayTag ScriptTag);
	void ActiveScript_Imp(const FGameplayTag ScriptTag);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Perceptron")
	void DeactiveScript(const FGameplayTag ScriptTag);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|Perceptron")
	void DeactiveScript_Server(const FGameplayTag ScriptTag);
	void DeactiveScript_Imp(const FGameplayTag ScriptTag);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Movement")
	void SetMoveControl(const FGBWFlowMoveControlSettings MoveControlSettings);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|Movement")
	void SetMoveControl_Server(const FGBWFlowMoveControlSettings MoveControlSettings);
	void SetMoveControl_Imp(const FGBWFlowMoveControlSettings& MoveControlSettings);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Movement")
	void DeactiveMoveControl();
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|Movement")
	void DeactiveMoveControl_Server();
	void DeactiveMoveControl_Imp();

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Movement")
	void ResetMoveControl();
	UFUNCTION(Reliable, Server, Category = "GBW|FlowMove|Movement")
	void ResetMoveControl_Server();
	void ResetMoveControl_Imp();

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Movement")
	FGBWFlowMoveControlParam GetMoveControlParam() const;

	FVector GetMoveToVector(
		float DeltaTime,
		bool bUseDefaultMoveVectorWhenZero,
		const FVector& DefaultMoveVector);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|State")
	bool IsActionNow() const;

	UFUNCTION(BlueprintCallable, Category="GBW|FlowMove|State")
	UGBWFlowMoveBrain_Base* GetFlowMoveBrain();

	UFUNCTION(BlueprintCallable, Category="GBW|FlowMove|State")
	FVector GetDirectionVector(EFlowMoveDirectionType Type);

	UFUNCTION(BlueprintCallable, Category="GBW|FlowMove|Perceptron")
	bool GetScene(FGBWFlowMoveScene& Scene, FGameplayTag SceneSlot, bool bIsConsume);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|State")
	bool IsActionLock(FGameplayTagContainer ActionToLock) const;
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|State")
	bool IsAnyActionLock() const;

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Scene")
	void SetCurrentTargetScene(FGameplayTag ActionTargetScene);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|Scene")
	void SetCurrentTargetScene_Server(FGameplayTag ActionTargetScene);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Action")
	void SetCurrentActionTag(FGameplayTag ActionTag);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|Action")
	void SetCurrentActionTag_Server(FGameplayTag ActionTag);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Perceptron")
	FVector GetActionTargetPoint(EGBWFlowMoveScenePointType ActionTargetPoint);
// END FlowMove State

// BEGIN FlowMove Tools
	UFUNCTION(BlueprintPure,Category = "GBW|FlowMove|State")
	bool IsLocalPlayer(bool bIsReset = false);
	UFUNCTION(BlueprintPure,Category = "GBW|FlowMove|State")
	bool IsAIPlayer(bool bIsReset = false);
	UFUNCTION(BlueprintPure,Category = "GBW|FlowMove|State")
	bool IsLocalOwn(bool bIsReset = false);
	UFUNCTION(BlueprintPure,Category = "GBW|FlowMove|State")
	bool IsInServer(bool bIsReset = false);
	UFUNCTION(BlueprintPure,Category = "GBW|FlowMove|State")
	bool IsFlowMoveActive() const;
	UFUNCTION(BlueprintPure,Category = "GBW|FlowMove|State")
	bool IsFlowMoveStopping() const;
	
	UFUNCTION(BlueprintCallable,Category = "GBW|FlowMove|Tools")
	AActor* GetNetWorkActor(FGuid ActorGuid);
	
	UFUNCTION(BlueprintCallable,Category = "GBW|FlowMove|Tools")
	void SpawnNetWorkActor(FGuid ActorGuid, TSubclassOf<AActor> ActorClass, FTransform Transform);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|Tools")
	void SpawnNetWorkActor_Server(FGuid ActorGuid, TSubclassOf<AActor> ActorClass, FTransform Transform);
	void SpawnNetWorkActor_Imp(FGuid ActorGuid, TSubclassOf<AActor> ActorClass, FTransform Transform);

	UFUNCTION(BlueprintCallable,Category = "GBW|FlowMove|Tools")
	void DestroyNetWorkActor(FGuid ActorGuid);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|Tools")
	void DestroyNetWorkActor_Server(FGuid ActorGuid);
	void DestroyNetWorkActor_Imp(FGuid ActorGuid);

	UFUNCTION(BlueprintCallable,Category = "GBW|FlowMove|Tools")
	void SetNetWorkActorLocation(FGuid ActorGuid, FVector NewLocation);
	UFUNCTION(Reliable,Server,Category = "GBW|FlowMove|Tools")
	void SetNetWorkActorLocation_Server(FGuid ActorGuid, FVector NewLocation);
	void SetNetWorkActorLocation_Imp(FGuid ActorGuid, FVector NewLocation);

	UFUNCTION(BlueprintCallable,Category = "GBW|FlowMove|Tools")
	void ResetCapsuleAndMesh();
	UFUNCTION(Reliable, Server, Category = "GBW|FlowMove|Tools")
	void ResetCapsuleAndMesh_Server();
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|FlowMove|Tools")
	void ResetCapsuleAndMesh_Multicast();
	void ResetCapsuleAndMesh_Imp() const;

	UFUNCTION(BlueprintCallable,Category = "GBW|FlowMove|Tools")
	void SetCapsuleAndKeepMesh(float NewHeight);
	UFUNCTION(Reliable, Server, Category = "GBW|FlowMove|Tools")
	void SetCapsuleAndKeepMesh_Server(float TargetRadius, float TargetHalfHeight, float TargetMeshRlz);
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|FlowMove|Tools")
	void SetCapsuleAndKeepMesh_Multicast(float TargetRadius, float TargetHalfHeight, float TargetMeshRlz);
	void SetCapsuleAndKeepMesh_Imp(float TargetRadius, float TargetHalfHeight, float TargetMeshRlz) const;

	UFUNCTION(BlueprintCallable,Category = "GBW|FlowMove|Tools")
	void AdjustCapsuleAndKeepMeshByTargetLocation(FGBWFlowMoveScene FlowMoveScene);
	UFUNCTION(BlueprintCallable,Category = "GBW|FlowMove|Tools")
	void AdjustCapsuleAndKeepMeshByCurrentLocation(FGBWFlowMoveScene FlowMoveScene);

	UFUNCTION(BlueprintCallable,Category = "GBW|FlowMove|Tools")
	float GetActorScale() const;
	
	void PostureAdjust(float DeltaTime) const;
	void RecoveryPosture() const;
	void MovingAdjust();
// END FlowMove Tools

// BEGIN Config
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "FlowMoveSettings")
	TEnumAsByte<EFlowMoveCharacterViewMode> DefaultViewMode = EFlowMoveCharacterViewMode::TP_FreeMode;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FlowMoveSettings")
	UGBWFlowMoveBrain_Base* FlowMoveBrain = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="FlowMoveSettings|PowerfulState|Settings")
	int MaxStateOperationNum = 2048;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="FlowMoveSettings|PowerfulState|Debug")
	bool bPS_IsDebug;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="FlowMoveSettings|PowerfulState|Debug")
	int PS_StateOperationNum = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="FlowMoveSettings|PowerfulState|Debug")
	FColor PS_DebugTextColor = FColor::Green;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="FlowMoveSettings|PowerfulState|Debug")
	FColor PS_DebugTextColor2 = FColor::Yellow;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="FlowMoveSettings|PowerfulState|Debug")
	bool bPS_PrintToScreen = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="FlowMoveSettings|PowerfulState|Debug")
	bool bPS_PrintToLog = true;
// END Config
	
//BEGIN Runtime 
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "GBW|FlowMove")
	FGBWFlowMoveState TaskState = FGBWFlowMoveState();//TaskState是一个结构体
	UPROPERTY(Replicated)
	TArray<FGuid> NetWorkActorGuids;
	UPROPERTY(Replicated)
	TArray<AActor*> NetWorkActors;

	UPROPERTY(Replicated)
	UGBWPowerfulStateComponent* PSStateComponent = nullptr;
	
	UPROPERTY()
	bool bIsNetInit = false;
	
	float CapsuleHalfHeightCache = 90.0f;
	float CapsuleRadiusCache = 30.0f;
	FVector MeshRelativeLocationCache = FVector(0,0,-92.0f);
	float MovementCompMaxAccelerationCache = 0.0f;//最大加速度选取的是Movement的最大加速度（在begin play的时候调用）

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "GBW|FlowMove")
	FGBWFlowMoveRMSControllerInitInfo CurrentActionTargetSceneInfo = FGBWFlowMoveRMSControllerInitInfo();

	bool bUseControllerRotationYawCache = false;
	
	bool bIsConstrainToPlaneCache = false;
	FVector PlaneConstraintNormalCache = FVector::ZeroVector;
	FVector PlaneConstraintOriginCache = FVector::ZeroVector;
//END Runtime
	
private:

	bool bIsLocalPlayer = false;
	bool bIsLocalPlayerInit = false;
	
	UPROPERTY(Replicated)
	bool bIsAIPlayer = false;
	UPROPERTY(Replicated)
	bool bIsAIPlayerInit = false;
	
	bool bIsLocalOwn = false;
	bool bIsLocalOwnInit = false;
	bool bIsInServer = false;
	bool bIsInServerInit = false;
	bool bIsFlowMoveActive= false;
	bool bIsFlowMoveActiveInit = false;

	int FrameNumber = INDEX_NONE;
	
//BEGIN Runtime
	//FlowMove Tick
	void FlowMoveTick(float DeltaTime);
	//Check CheckActive
	void CheckActive();
	//Check Stop
	void CheckStop();
	//Check Input
	void CheckInput(bool bReset = false);
	//Check ViewMode
	void CheckViewMode();
	//Check Valid
	bool CheckUpdateValid();
	void EnsureRMSActivation();
	//Update CharacterNetState
	void UpdateCharacterState(float DeltaTime);
	//Update Script
	void UpdateScript(float DeltaTime);
	UFUNCTION(Reliable, Server, Category = "GBW|FlowMove|Script")
	void UpdateScript_Server(float DeltaTime, FGameplayTag ScriptKey, EGBWFlowMoveEventType EventType);
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|FlowMove|Script")
	void UpdateScript_Multicast(float DeltaTime, FGameplayTag ScriptKey, EGBWFlowMoveEventType EventType);
	//Update Scene
	void UpdateScene(float DeltaTime);
	UFUNCTION(Reliable, Server, Category = "GBW|FlowMove|Perception")
	void UpdateScene_Server(float DeltaTime, FGameplayTag PerceptionKey, EGBWFlowMoveEventType EventType);
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|FlowMove|Perception")
	void UpdateScene_Multicast(float DeltaTime, FGameplayTag PerceptionKey, EGBWFlowMoveEventType EventType);
	//Update MoveControlParam And RMS
	void UpdateMoveControlParam(float DeltaTime);
	//Update RMS
	void UpdateRMS(float DeltaTime);
	//Update Current Action  //Update RMS
	void UpdateCurrentAction(float DeltaTime);
//END Runtime

	void OnActiveEvent();
	void OnStopEvent();
	void OnActionEndEvent(FGameplayTag ActionTag);
	bool IsLocked(FGameplayTag ForActionTag) const;
	void Recovery();
};
