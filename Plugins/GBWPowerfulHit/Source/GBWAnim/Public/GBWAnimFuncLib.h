// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GBWAnimComponent.h"
#include "GBWPowerfulAnimInstance.h"
#include "Animation/AnimNodeReference.h"
#include "GBWAnimFuncLib.generated.h"

USTRUCT(BlueprintType)
struct FGBWAnimSequencePlaySettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float PlayRateBasis = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float PlayRate = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float StartPosition = 0.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool LoopAnimation = true;
	
	FGBWAnimSequencePlaySettings(){}
};

UCLASS()
class GBWANIM_API UGBWAnimFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "GBW|Anim", meta=(DefaultToSelf="InActor"))
	static UGBWAnimComponent* GetGBWAnimComponent(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "GBW|Anim", meta=(DefaultToSelf="InActor"))
	static void SetIncludeMainInstance(AActor* InActor, bool bInclude = false);
	UFUNCTION(BlueprintCallable, Category = "GBW|Anim", meta=(DefaultToSelf="InActor"))
	static void SetIncludeLinkedInstance(AActor* InActor, bool bInclude = true);
	UFUNCTION(BlueprintCallable, Category = "GBW|Anim", meta=(DefaultToSelf="InActor"))
	static void AddIncludeLinkedInstanceClass(AActor* InActor, TArray<TSubclassOf<UAnimInstance>> IncludeLinkedAnimInstanceClassSet);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|Anim")
	static bool GetAnimPlayState(
		UAnimInstance* Instance,
		FGBWAnimStateGetType GetType,
		float DeltaTime,
		FGBWAnimPlayStateSet OldAnimPlayStateSet,
		FGBWAnimPlayStateSet& AnimPlayStateSet);
	UFUNCTION(BlueprintCallable, Category = "GBW|Anim", meta=(DefaultToSelf="InActor"))
	static bool GBWGetAnimPlayState(AActor* InActor, FGBWAnimPlayStateSet& AnimPlayStateSet);
	UFUNCTION(BlueprintCallable, Category = "GBW|Anim", meta=(DefaultToSelf="InActor"))
	static bool GBWGetAnimPlayState_Local(AActor* InActor, FGBWAnimPlayStateSet& AnimPlayStateSet);

	UFUNCTION(BlueprintCallable, Category = "GBW|Anim", meta=(DefaultToSelf="InActor"))
	static bool GBWGetAnimPlayStateFromArrayByAsset(
		TArray<FGBWAnimPlayState> AnimPlayStateArr,
		UAnimationAsset* AnimAsset,
		int32 AnimNodeIndex,
		FGBWAnimPlayState& AnimPlayState);
	UFUNCTION(BlueprintCallable, Category = "GBW|Anim", meta=(DefaultToSelf="InActor"))
	static bool GBWGetAnimStateMachineStateFromArrayByNodeIndex(
		TArray<FGBWAnimStateMachineNodeInfo> StateMachineNodeInfoArr,
		int32 NodeIndex,
		FGBWAnimStateMachineNodeInfo& StateMachineNodeInfo);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|Anim")
	static void GBWPlayMontage(USkeletalMeshComponent* SKMComponent,
		UAnimMontage* MontageToPlay,
		float PlayRate=1.0f,
		float StartingPosition=0.0f,
		FName StartingSection=NAME_None);
	UFUNCTION(BlueprintCallable, Category = "GBW|Anim")
	static void GBWStopMontage(USkeletalMeshComponent* SKMComponent);

	UFUNCTION(BlueprintCallable, Category = "GBW|Anim")
	static void GBWConditionPlayMontage(USkeletalMeshComponent* SKMComponent,
		TArray<FGBWConditionMontage> MontagePlaySettings,
		bool& CanPlay,
		FGBWConditionMontage& MontageToPlay);

	UFUNCTION(BlueprintPure, Category = "GBW|Anim", meta=(DefaultToSelf="InActor"))
	static UAnimMontage* GetNextMontageFromList(AActor* InActor,
	                                            const TArray<UAnimMontage*>& MontageList);

	UFUNCTION(BlueprintPure, Category = "GBW|Anim", meta=(DefaultToSelf="InActor"))
	static UAnimSequence* GetNextAnimSequenceFromList(
		AActor* InActor,
		bool bHoldNowWhenAnimActive,
		bool bHoldNowWhenAnimIsPreAnim,
		const TArray<UAnimSequence*>& AnimSequenceList);
	UFUNCTION(BlueprintPure, Category = "GBW|Anim|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static UAnimSequence* Anim_Pure_GetNextAnimSequenceFromList(
		const UAnimInstance* AnimInstance,
		EGBWAnimNodeBlendState StateBlendState,
		const TArray<UAnimSequence*>& AnimSequenceList);
	UFUNCTION(BlueprintPure, Category = "GBW|Anim|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void Anim_Pure_GetNextAnimSequenceFromList_Pro(
		const UAnimInstance* AnimInstance,
		EGBWAnimNodeBlendState StateBlendState,
		const TMap<UAnimSequence*, FGBWAnimSequencePlaySettings>& AnimSequenceSettingList,
		UAnimSequence*& AnimRes,
		FGBWAnimSequencePlaySettings& PlaySettings);

	UFUNCTION(BlueprintCallable, Category = "GBW|Anim")
	static FString GetDebugStringFromAnimPlayState(
		FGBWAnimPlayStateSet AnimPlayStateSet);
	UFUNCTION(BlueprintCallable, Category = "GBW|Anim")
	static UAnimNotifyState* GetNotifyStateByClass(FGBWAnimPlayStateSet AnimPlayStateSet, TSubclassOf<UAnimNotifyState> NotifyStateClass);
	UFUNCTION(BlueprintCallable, Category = "GBW|Anim")
	static FGBWAnimCurveValue GetCurveValueByName(FGBWAnimPlayStateSet AnimPlayStateSet, FName CurveName);

	UFUNCTION(BlueprintCallable, Category = "GBW|Anim")
	static float GetMontageSectionLength(UAnimMontage* Montage, FName SectionName);

	UFUNCTION(BlueprintCallable, Category="GBW|Anim")
	static bool GetAnimCurveValue(
		UAnimSequenceBase* Anim,
		FName CurveName,
		float TimeOffset,
		float Position,
		float DeltaTime,
		float& Result,
		float& ResultDelta);

	UFUNCTION(BlueprintCallable, Category = "GBW|Anim")
	static bool IsPlayingSlotAnim(
		UAnimInstance* Instance,
		FName SlotName,
		UAnimSequenceBase*& AnimSequence,
		UAnimMontage*& OutMontage);

	UFUNCTION(BlueprintCallable, Category = "GBW|Anim|Condition", meta=(DefaultToSelf="InActor"))
	static bool Anim_IsBlendFinished(AActor* InActor);
	UFUNCTION(BlueprintPure, Category = "GBW|Anim|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static bool Anim_Pure_IsBlendFinished(const UAnimInstance* AnimInstance);

	//On the server, OutCondition is the same as InCondition;
	//On the client side, OutCondition is false.
	UFUNCTION(BlueprintPure, Category = "GBW|Anim|Pure|Condition", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void Anim_Pure_OnlyServer(
		bool InCondition,
		const UAnimInstance* AnimInstance,
		bool& OutCondition);

	UFUNCTION(BlueprintCallable, Category = "GBW|Anim", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void GBW_AnimSM_FollowServer(const UAnimInstance* AnimInstance, FAnimUpdateContext Context, FAnimNodeReference Node);
};