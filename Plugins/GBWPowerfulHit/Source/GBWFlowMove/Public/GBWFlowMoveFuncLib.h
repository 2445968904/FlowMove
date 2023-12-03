// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GBWFlowMoveObjects.h"
#include "GameFramework/Character.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CollisionShape.h"
#include "CollisionQueryParams.h"
#include "GBWFlowMoveFuncLib.generated.h"

UCLASS()
class GBWFLOWMOVE_API UGBWFlowMoveFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove")
	static bool GetSKMeshByTag(
		USkeletalMeshComponent*& SKMeshComp,
		ACharacter* Character,
		FName MeshTag,
		TArray<FName> ExcludedMeshTag,
		UAnimationAsset* TargetAnimationAsset);

	UFUNCTION(BlueprintCallable, Category = "GBW|Net")
	static bool IsLocalOwn(AActor* Actor);
	UFUNCTION(BlueprintCallable, Category = "GBW|Net")
	static bool IsAIPlayer(AActor* Actor);

	static FHitResult SweepMulti(
		const UWorld* World,
		TArray<struct FHitResult>& OutHits,
		const FVector& Start,
		const FVector& End,
		const FQuat& Rot,
		FGBWFlowMoveTraceSetting TraceSetting,
		const FCollisionShape& CollisionShape,
		const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam);

	static bool OverlapAnyTest(
		const UWorld* World,
		const FVector& Pos,
		const FQuat& Rot,
		FGBWFlowMoveTraceSetting TraceSetting,
		const FCollisionShape& CollisionShape,
		const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam);

	static FHitResult LineTrace(
		const UWorld* World,
		TArray<struct FHitResult>& OutHits,
		const FVector& Start,
		const FVector& End,
		FGBWFlowMoveTraceSetting TraceSetting,
		const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam);

	static bool GetTheLastValidHitResult(TArray<FHitResult> HitResultArr, FHitResult& Result);
	static void FindFloorAndRoof(
		ACharacter* Character,
		FVector FromLocation,
		float FloorTraceDistance,
		float RoofTraceDistance,
		bool bFindLedge,
		float LedgeTraceDistance,
		FGBWFlowMoveTraceSetting TraceSetting,
		bool& Floor,
		FHitResult& OutHit_Floor,
		bool& Roof,
		FHitResult& OutHit_Roof,
		FGBWFlowMoveFloorRoofScene& FloorAndRoofRes);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Trace")
	static void FindBlockWall(
		const ACharacter* Character,
		FVector ForwardVector,
		float WallTraceDistance,
		bool bFindLedge,
		float LedgeTraceDistance,
		FGBWFlowMoveTraceSetting TraceSetting,
		FGBWFlowMoveBlockWallScene& WallRes);

	UFUNCTION(BlueprintCallable, Category="GBW|FlowMove|Perception")
	static void AddBool(FGBWPerceptionResult PerceptionResult, FName Key, bool Value);
	UFUNCTION(BlueprintCallable, Category="GBW|FlowMove|Perception")
	static void AddFloat(FGBWPerceptionResult PerceptionResult, FName Key, float Value);
	UFUNCTION(BlueprintCallable, Category="GBW|FlowMove|Perception")
	static void AddVector(FGBWPerceptionResult PerceptionResult, FName Key, FVector Value);
	UFUNCTION(BlueprintCallable, Category="GBW|FlowMove|Perception")
	static void AddTransform(FGBWPerceptionResult PerceptionResult, FName Key, FTransform Value);
	UFUNCTION(BlueprintCallable, Category="GBW|FlowMove|Perception")
	static void AddString(FGBWPerceptionResult PerceptionResult, FName Key, FString Value);
	UFUNCTION(BlueprintCallable, Category="GBW|FlowMove|Perception")
	static void AddGameplayTag(FGBWPerceptionResult PerceptionResult, FName Key, FGameplayTag Value);
	UFUNCTION(BlueprintCallable, Category="GBW|FlowMove|Perception")
	static void AddActor(FGBWPerceptionResult PerceptionResult, FName Key, AActor* Value);
	UFUNCTION(BlueprintCallable, Category="GBW|FlowMove|Perception")
	static void AddObject(FGBWPerceptionResult PerceptionResult, FName Key, UObject* Value);
	UFUNCTION(BlueprintCallable, Category="GBW|FlowMove|Perception")
	static void AddMovementMode(FGBWPerceptionResult PerceptionResult, FName Key, EMovementMode Value);

	UFUNCTION(BlueprintPure, Category="GBW|FlowMove|Perception")
	static void GetBool(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet, bool& Value);
	UFUNCTION(BlueprintPure, Category="GBW|FlowMove|Perception")
	static void GetFloat(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet, float& Value);
	UFUNCTION(BlueprintPure, Category="GBW|FlowMove|Perception")
	static void GetVector(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet, FVector& Value);
	UFUNCTION(BlueprintPure, Category="GBW|FlowMove|Perception")
	static void GetTransform(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet, FTransform& Value);
	UFUNCTION(BlueprintPure, Category="GBW|FlowMove|Perception")
	static void GetString(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet, FString& Value);
	UFUNCTION(BlueprintPure, Category="GBW|FlowMove|Perception")
	static void GetGameplayTag(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet, FGameplayTag& Value);
	UFUNCTION(BlueprintPure, Category="GBW|FlowMove|Perception")
	static void GetActor(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet, AActor*& Value);
	UFUNCTION(BlueprintPure, Category="GBW|FlowMove|Perception")
	static void GetObject(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet, UObject*& Value);
	UFUNCTION(BlueprintPure, Category="GBW|FlowMove|Perception")
	static void GetMovementMode(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet, TEnumAsByte<EMovementMode>& Value);

	UFUNCTION(BlueprintCallable, Category="GBW|FlowMove|Perception")
	static FGBWFlowMovePointScene MakeFlowMovePointScene(ACharacter* InCharacter, const FVector& InPoint, bool bFindLedge, float LedgeTraceDistance, const FGBWFlowMoveTraceSetting& TraceSetting);
	
	//Math - start
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Math")
	static FVector RadianToDirectionXY(float Radian);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Math")
	static FVector AngleToDirectionXY(float Angle);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Math")
	static float GetVelocityYawAngle(FVector Velocity);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Math")
	static float GetInputYawAngle(FVector InputDirection);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Math")
	static float GetPointToLineDistance(FVector LineStart, FVector LineEnd, FVector Point);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Math")
	static FVector GetPointToLineFoot(const FVector& LineStart, const FVector& LineEnd, const FVector& Point);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Math")
	static float GetMoveDirection(const ACharacter* OwnerCharacter, FVector DirectionVector, bool bIsUseActorSpeedVector = false);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Math")
	static FVector GetMoveVectorFromCharacterInputVector(const ACharacter* OwnerCharacter);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Math")
	static void Boxplot_Float(
		const TArray<float>& InDataArr,
		TArray<float>& OutDataArr);
	//Math - end

	//FlowMove - start
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove", meta=(DefaultToSelf="InActor"))
	static UGBWFlowMoveComponent* FM_GetFlowMoveComponent(const AActor* InActor);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove", meta=(DefaultToSelf="InActor"))
	static void FM_ActiveFlowMove(const AActor* InActor);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove", meta=(DefaultToSelf="InActor"))
	static void FM_StopFlowMove(const AActor* InActor, bool WaitForCurrentActionFinished = true);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Input", meta=(DefaultToSelf="InActor"))
	static bool FM_SetMoveVector(const AActor* InActor, FVector NewMoveVector, float ZeroFaultToleranceDuration = 0.1f);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Input", meta=(DefaultToSelf="InActor"))
	static bool FM_SetControlVector(const AActor* InActor, FVector NewControlVector);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Input", meta=(DefaultToSelf="InActor"))
	static bool FM_SetPerceptionVector(const AActor* InActor, FVector NewPerceptionVector);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|State", meta=(DefaultToSelf="InActor"))
	static bool FM_SetFocusActor(AActor* InActor, USceneComponent* FocusActorComponent, bool bSetViewModeAsActorLock);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|State", meta=(DefaultToSelf="InActor"))
	static bool FM_SetViewMode(AActor* InActor, EFlowMoveCharacterViewMode NewViewMode, bool bAsDefaultViewMode);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|State", meta=(DefaultToSelf="InActor"))
	static bool FM_TryLockActor(AActor* InActor, FGameplayTag GetTargetFunctionTag);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsSceneType(AActor* InActor,
		FGameplayTag TargetScene,
		FFlowMoveSceneTypeCondition SceneTypeCondition,
		bool SetAsCurrentTargetSceneWhenMet = true);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsSceneType(const UAnimInstance* AnimInstance,
	                                FGameplayTag TargetScene,
	                                const FFlowMoveSceneTypeCondition& SceneTypeCondition,
									bool SetAsCurrentTargetSceneWhenMet = true);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsSceneType_Simple(AActor* InActor,
		FGameplayTag TargetScene,
		TEnumAsByte<EInclusivityType> ConditionType = EInclusivityType::Include_,
		FName SceneType = NAME_None,
		bool SetAsCurrentTargetSceneWhenMet = true);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsSceneType_Simple(const UAnimInstance* AnimInstance,
		FGameplayTag TargetScene,
		TEnumAsByte<EInclusivityType> ConditionType = EInclusivityType::Include_,
		FName SceneType = NAME_None,
		bool SetAsCurrentTargetSceneWhenMet = true);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsActionLock(AActor* InActor, FGameplayTagContainer ActionToLock);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsActionLock(const UAnimInstance* AnimInstance, const FGameplayTagContainer& ActionToLock);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsActionLock_Simple(AActor* InActor, FGameplayTag ActionToLock);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsActionLock_Simple(const UAnimInstance* AnimInstance, FGameplayTag ActionToLock);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsAnyActionLock(AActor* InActor);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsAnyActionLock(const UAnimInstance* AnimInstance);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static void FM_IsSide(AActor* InActor,
		FGameplayTag TargetScene,
		bool& IsLeft,
		bool& IsRight,
		float& LeftDistance,
		float& RightDistance,
		bool& IsLeftCloser,
		bool& IsRightCloser);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void FM_Pure_IsSide(const UAnimInstance* AnimInstance, 
		FGameplayTag TargetScene,
		bool& IsLeft,
		bool& IsRight,
		float& LeftDistance,
		float& RightDistance,
		bool& IsLeftCloser,
		bool& IsRightCloser);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsSceneHeight(AActor* InActor,
		FGameplayTag TargetScene,
		FGBWFlowMoveFloatScope SceneHeightRange);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsSceneHeight(const UAnimInstance* AnimInstance, 
		FGameplayTag TargetScene,
		FGBWFlowMoveFloatScope SceneHeightRange);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsSceneRight(AActor* InActor,
		FGameplayTag TargetScene,
		FGBWFlowMoveFloatScope SceneRightRange);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsSceneRight(const UAnimInstance* AnimInstance, 
		FGameplayTag TargetScene,
		FGBWFlowMoveFloatScope SceneRightRange);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsSceneForward(AActor* InActor,
		FGameplayTag TargetScene,
		FGBWFlowMoveFloatScope SceneForwardRange);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsSceneForward(const UAnimInstance* AnimInstance, 
		FGameplayTag TargetScene,
		FGBWFlowMoveFloatScope SceneForwardRange);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsSceneAngle(AActor* InActor,
		FGameplayTag TargetScene,
		FGBWFlowMoveFloatScope SceneAngleRange);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsSceneAngle(const UAnimInstance* AnimInstance, 
		FGameplayTag TargetScene,
		FGBWFlowMoveFloatScope SceneAngleRange);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsSceneDirection(AActor* InActor,
		FGameplayTag TargetScene,
		FFlowMoveSceneDirectionCondition SceneDirectionCondition);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsSceneDirection(const UAnimInstance* AnimInstance, 
		FGameplayTag TargetScene,
		const FFlowMoveSceneDirectionCondition& SceneDirectionCondition);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsMovementMode(AActor* InActor, FFlowMoveMovementModeCondition MovementModeCondition);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsMovementMode(const UAnimInstance* AnimInstance, const FFlowMoveMovementModeCondition& MovementModeCondition);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsHeightFromGround(AActor* InActor,
		FGameplayTag TargetScene,
		FGBWFlowMoveFloatScope HeightFromGroundRange);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsHeightFromGround(const UAnimInstance* AnimInstance,
		FGameplayTag TargetScene,
		FGBWFlowMoveFloatScope HeightFromGroundRange);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsHeightFromRoof(AActor* InActor,
		FGameplayTag TargetScene,
		FGBWFlowMoveFloatScope HeightFromRoofRange);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsHeightFromRoof(const UAnimInstance* AnimInstance,
		FGameplayTag TargetScene,
		FGBWFlowMoveFloatScope HeightFromRoofRange);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsFloor(AActor* InActor,
		FGameplayTag TargetScene,
		FGBWFlowMoveFloatScope HeightFromFloorRange,
		bool bIsAnyRequireLedge,
		FGBWFlowMoveFloatScope FloorLedgeRange,
		bool bIsRequireForwardLedge,
		FGBWFlowMoveFloatScope ForwardFloorLedgeRange,
		bool bIsRequireBackwardLedge,
		FGBWFlowMoveFloatScope BackwardFloorLedgeRange);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsFloor(const UAnimInstance* AnimInstance,
		FGameplayTag TargetScene,
		FGBWFlowMoveFloatScope HeightFromFloorRange,
		bool bIsAnyRequireLedge,
		FGBWFlowMoveFloatScope FloorLedgeRange,
		bool bIsRequireForwardLedge,
		FGBWFlowMoveFloatScope ForwardFloorLedgeRange,
		bool bIsRequireBackwardLedge,
		FGBWFlowMoveFloatScope BackwardFloorLedgeRange);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsRoof(AActor* InActor,
		FGameplayTag TargetScene,
		FGBWFlowMoveFloatScope HeightFromRoofRange,
		bool bIsAnyRequireLedge,
		FGBWFlowMoveFloatScope RoofLedgeRange,
		bool bIsRequireForwardLedge,
		FGBWFlowMoveFloatScope ForwardRoofLedgeRange,
		bool bIsRequireBackwardLedge,
		FGBWFlowMoveFloatScope BackwardRoofLedgeRange);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsRoof(const UAnimInstance* AnimInstance,
		FGameplayTag TargetScene,
		FGBWFlowMoveFloatScope HeightFromRoofRange,
		bool bIsAnyRequireLedge,
		FGBWFlowMoveFloatScope RoofLedgeRange,
		bool bIsRequireForwardLedge,
		FGBWFlowMoveFloatScope ForwardRoofLedgeRange,
		bool bIsRequireBackwardLedge,
		FGBWFlowMoveFloatScope BackwardRoofLedgeRange);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsSlope(AActor* InActor, FGameplayTag TargetScene, FGBWFlowMoveFloatScope SlopeRange);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsSlope(const UAnimInstance* AnimInstance, FGameplayTag TargetScene, FGBWFlowMoveFloatScope SlopeRange);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsMoveVector(AActor* InActor, FFlowMoveMoveVectorCondition MoveVectorRange);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsMoveVector(const UAnimInstance* AnimInstance, const FFlowMoveMoveVectorCondition& MoveVectorRange);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsHasMoveVector(AActor* InActor);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsHasMoveVector(const UAnimInstance* AnimInstance);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsLastActionTag(AActor* InActor,
		FGameplayTagQuery LastActionCondition);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsLastActionTag(const UAnimInstance* AnimInstance,
	                                    const FGameplayTagQuery& LastActionCondition);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsCurrentActionTag(AActor* InActor,
		FGameplayTagQuery CurrentActionCondition,
		float MinTimeCondition = 0.0f,
		float MaxTimeCondition = 0.0f);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsCurrentActionTag(const UAnimInstance* AnimInstance,
	    const FGameplayTagQuery& CurrentActionCondition,
		float MinTimeCondition = 0.0f,
		float MaxTimeCondition = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsCurrentActionTagAndActionLock(AActor* InActor,
		FGameplayTagQuery CurrentActionCondition,
		float MinTimeCondition = 0.0f,
		float MaxTimeCondition = 0.0f,
		bool IsRequireNoActionLock = false,
		bool IsNegateActionLock = false,
		FGameplayTagContainer ActionLockCondition = FGameplayTagContainer());
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsCurrentActionTagAndActionLock(const UAnimInstance* AnimInstance,
		const FGameplayTagQuery& CurrentActionCondition,
		float MinTimeCondition = 0.0f,
		float MaxTimeCondition = 0.0f,
		bool IsRequireNoActionLock = false,
		bool IsNegateActionLock = false,
		FGameplayTagContainer ActionLockCondition = FGameplayTagContainer());
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsFlowMoveSystemEvent(AActor* InActor,
		FFlowMoveEvent FlowMoveSystemEvent);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsCharacterSpeed(AActor* InActor,
		bool OnlyForHorizontalSpeed,
		bool OnlyForMoveToDirectionSpeed,
		FGBWFlowMoveFloatScope CharacterSpeedRange);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsCharacterSpeed(const UAnimInstance* AnimInstance,
		bool OnlyForHorizontalSpeed,
		bool OnlyForMoveToDirectionSpeed,
		FGBWFlowMoveFloatScope CharacterSpeedRange);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsViewMode(AActor* InActor,
		TEnumAsByte<EInclusivityType> ConditionType,
		TSet<TEnumAsByte<EFlowMoveCharacterViewMode>> ViewModeSet);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsViewMode(const UAnimInstance* AnimInstance,
		TEnumAsByte<EInclusivityType> ConditionType,
		TSet<TEnumAsByte<EFlowMoveCharacterViewMode>> ViewModeSet);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsAngle(AActor* InActor,
		EFlowMoveDirectionType From = EFlowMoveDirectionType::TargetMoveToDirection,
		EFlowMoveDirectionType To = EFlowMoveDirectionType::Velocity,
		FGBWFlowMoveFloatScope AngleRange = FGBWFlowMoveFloatScope());
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool FM_Pure_IsAngle(const UAnimInstance* AnimInstance,
		EFlowMoveDirectionType From = EFlowMoveDirectionType::TargetMoveToDirection,
		EFlowMoveDirectionType To = EFlowMoveDirectionType::Velocity,
		FGBWFlowMoveFloatScope AngleRange = FGBWFlowMoveFloatScope());
	
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Condition", meta=(DefaultToSelf="InActor"))
	static bool FM_IsFlowMoveActive(AActor* InActor, bool& IsStopping);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void FM_Pure_IsFlowMoveActive(const UAnimInstance* AnimInstance, bool& IsActive, bool& IsStopping);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|State", meta=(DefaultToSelf="InActor"))
	static bool FM_GetMoveControlParam(AActor* InActor, FGBWFlowMoveControlParam& MoveControlParam);

	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Input", meta=(DefaultToSelf="InActor", BlueprintThreadSafe))
	static void FM_GetMoveParamForBlendSpace(AActor* InActor, UBlendSpace* ForBS, float& Direction, float& Speed, float& PlayRate);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Input", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void FM_Pure_GetMoveParamForBlendSpace(const UAnimInstance* AnimInstance, UBlendSpace* ForBS, float& Direction, float& Speed, float& PlayRate);

	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Input", meta=(DefaultToSelf="InActor", BlueprintThreadSafe))
	static void FM_GetAnimExpectedPlayRate(const AActor* InActor, float& PlayRate);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Input", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void FM_Pure_GetAnimExpectedPlayRate(const UAnimInstance* AnimInstance, float& PlayRate);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Scene", meta=(DefaultToSelf="InActor"))
	static void FM_SetCurrentTargetScene(
		AActor* InActor,
		FGameplayTag ActionTargetScene);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Scene", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void FM_Pure_SetCurrentTargetScene(
		bool InCondition,
		bool ExecuteWhenNotMeetCondition,
		const UAnimInstance* AnimInstance,
		const FGameplayTag& ActionTargetScene,
		bool& OutCondition);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Action", meta=(DefaultToSelf="InActor"))
	static void FM_SetCurrentActionTag(
		AActor* InActor,
		FGameplayTag ActionTag);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Action", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void FM_Pure_SetCurrentActionTag(
		bool InCondition,
		bool ExecuteWhenNotMeetCondition,
		const UAnimInstance* AnimInstance,
		const FGameplayTag& ActionTag,
		bool& OutCondition);

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Action", meta=(DefaultToSelf="InActor"))
	static void FM_ClearCurrentActionTag(AActor* InActor);
	UFUNCTION(BlueprintPure, Category = "GBW|FlowMove|Pure|Action", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void FM_Pure_ClearCurrentActionTag(
		bool InCondition,
		bool ExecuteWhenNotMeetCondition,
		const UAnimInstance* AnimInstance,
		bool& OutCondition);
	//FlowMove - end

	//Debug - start
	static void DrawDebugShape(
		const UObject* WorldContext,
		const FVector Location,
		const FQuat Rotation,
		const FCollisionShape CollisionShape,
		const FColor Color,
		float Duration = 0.0f,
		float Thickness = 1.0f);
	static void PrintDebugString(TArray<FString> DebugStrArr, FName Key = NAME_None,const FColor Color = FColor::Red, float Duration = 1.0f);
	static void PrintDebugStringNet(
		const AActor* WorldActor,
		TArray<FString> DebugStrArr, 
		FName Key = NAME_None,
		const FColor ServerColor = FColor::Red,
		const FColor ClientColor = FColor::Green,
		float Duration = 1.0f);
	//Debug - end
};
