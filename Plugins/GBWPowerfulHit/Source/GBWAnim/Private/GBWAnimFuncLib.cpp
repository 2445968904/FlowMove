// Copyright GanBowen 2022-2023. All Rights Reserved.

#include "GBWAnimFuncLib.h"

#include "AnimationStateMachineLibrary.h"
#include "GBWAnimMetaData.h"
#include "Animation/AnimExecutionContext.h"
#include "Animation/AnimNode_AssetPlayerBase.h"
#include "Animation/AnimNode_LinkedAnimGraph.h"
#include "Animation/AnimNode_StateMachine.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNodes/AnimNode_Slot.h"
#include "Animation/BlendSpace.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/Package.h"
#include "Animation/AnimClassInterface.h"
#include "Runtime/Launch/Resources/Version.h"

bool FGBWAnimPlayStateSet::IsValid() const
{
	if (!Instance)
	{
		return false;
	}

	if (AnimPlayStateSet.IsEmpty())
	{
		return false;
	}

	return true;
}

bool FGBWConditionMontage::CanPlay() const
{
	if (Condition
		&& MontageSetting.MontageToPlay)
	{
		return true;
	}

	return false;
}

UGBWAnimComponent* UGBWAnimFuncLib::GetGBWAnimComponent(AActor* InActor)
{
	if (!InActor)
	{
		return nullptr;
	}
	UGBWAnimComponent* AComp = nullptr;
	if (UActorComponent* Component = InActor->GetComponentByClass(UGBWAnimComponent::StaticClass()))
	{
		AComp = Cast<UGBWAnimComponent>(Component);
	}

	if (!AComp && InActor->HasAuthority())
	{
		AComp = Cast<UGBWAnimComponent>(
			InActor->AddComponentByClass(UGBWAnimComponent::StaticClass(),
				false,
				FTransform(),
				false));
		AComp->SetIsReplicated(true);
	}

	return AComp->IsReadyToUse()? AComp : nullptr;
}

void UGBWAnimFuncLib::SetIncludeMainInstance(AActor* InActor, bool bInclude)
{
	if (UGBWAnimComponent* AnimComp = GetGBWAnimComponent(InActor))
	{
		AnimComp->SetIncludeMainInstance(bInclude);
	}
}

void UGBWAnimFuncLib::SetIncludeLinkedInstance(AActor* InActor, bool bInclude)
{
	if (UGBWAnimComponent* AnimComp = GetGBWAnimComponent(InActor))
	{
		AnimComp->SetIncludeLinkedInstance(bInclude);
	}
}

void UGBWAnimFuncLib::AddIncludeLinkedInstanceClass(AActor* InActor,
	TArray<TSubclassOf<UAnimInstance>> IncludeLinkedAnimInstanceClassSet)
{
	if (UGBWAnimComponent* AnimComp = GetGBWAnimComponent(InActor))
	{
		AnimComp->AddIncludeLinkedInstanceClass(IncludeLinkedAnimInstanceClassSet);
	}
}

void UGBWAnimFuncLib::GBWPlayMontage(USkeletalMeshComponent* SKMComponent, UAnimMontage* MontageToPlay, float PlayRate,
                                     float StartingPosition, FName StartingSection)
{
	if (SKMComponent && SKMComponent->GetOwner())
	{
		if (UGBWAnimComponent* AComp = UGBWAnimFuncLib::GetGBWAnimComponent(SKMComponent->GetOwner()))
		{
			AComp->PlayMontage(SKMComponent,MontageToPlay,PlayRate,StartingPosition,StartingSection);
		}
	}
}

void UGBWAnimFuncLib::GBWStopMontage(USkeletalMeshComponent* SKMComponent)
{
	if (SKMComponent && SKMComponent->GetOwner())
	{
		if (UGBWAnimComponent* AComp = UGBWAnimFuncLib::GetGBWAnimComponent(SKMComponent->GetOwner()))
		{
			AComp->StopMontage(SKMComponent);
		}
	}
}

void UGBWAnimFuncLib::GBWConditionPlayMontage(USkeletalMeshComponent* SKMComponent,
                                              TArray<FGBWConditionMontage> MontagePlaySettings,
                                              bool& CanPlay,
                                              FGBWConditionMontage& MontageToPlay)
{
	if (SKMComponent)
	{
		for (auto Element : MontagePlaySettings)
		{
			if (Element.CanPlay())
			{
				CanPlay = true;
				MontageToPlay = Element;
				GBWPlayMontage(
					SKMComponent,
					MontageToPlay.MontageSetting.MontageToPlay,
					MontageToPlay.MontageSetting.PlayRate,
					MontageToPlay.MontageSetting.StartingPosition,
					MontageToPlay.MontageSetting.StartingSection);
			}
		}
	}

	CanPlay = false;
}

UAnimMontage* UGBWAnimFuncLib::GetNextMontageFromList(AActor* InActor, const TArray<UAnimMontage*>& MontageList)
{
	if (UGBWAnimComponent* AnimComp = GetGBWAnimComponent(InActor))
	{
		if (AnimComp)
		{
			return AnimComp->GetNextMontageFromList(MontageList);
		}
	}

	return nullptr;
}

UAnimSequence* UGBWAnimFuncLib::GetNextAnimSequenceFromList(
	AActor* InActor,
	bool bHoldNowWhenAnimActive,
	bool bHoldNowWhenAnimIsPreAnim,
	const TArray<UAnimSequence*>& AnimSequenceList)
{
	if (UGBWAnimComponent* AnimComp = GetGBWAnimComponent(InActor))
	{
		if (AnimComp)
		{
			return AnimComp->GetNextAnimSequenceFromList(bHoldNowWhenAnimActive,bHoldNowWhenAnimIsPreAnim,AnimSequenceList);
		}
	}

	return nullptr;
}

UAnimSequence* UGBWAnimFuncLib::Anim_Pure_GetNextAnimSequenceFromList(
		const UAnimInstance* AnimInstance,
		EGBWAnimNodeBlendState StateBlendState,
		const TArray<UAnimSequence*>& AnimSequenceList)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		bool bHoldNowWhenAnimActive = true;
		bool bHoldNowWhenAnimIsPreAnim = false;
		switch (StateBlendState)
		{
		case StateBlendIn:
			bHoldNowWhenAnimActive = true;
			bHoldNowWhenAnimIsPreAnim = false;
			break;
		case StateBlendOut: 
			bHoldNowWhenAnimActive = true;
			bHoldNowWhenAnimIsPreAnim = true;
			break;
		case NoBlend: 
			bHoldNowWhenAnimActive = true;
			bHoldNowWhenAnimIsPreAnim = false;
			break;
		case StateReBlendIn:
			bHoldNowWhenAnimActive = false;
			bHoldNowWhenAnimIsPreAnim = false;
			break;
		default: 
			bHoldNowWhenAnimActive = true;
			bHoldNowWhenAnimIsPreAnim = false;
		}
		return UGBWAnimFuncLib::GetNextAnimSequenceFromList(
			AnimInstance->GetOwningActor(),
			bHoldNowWhenAnimActive,
			bHoldNowWhenAnimIsPreAnim,
			AnimSequenceList);
	}

	return nullptr;
}

void UGBWAnimFuncLib::Anim_Pure_GetNextAnimSequenceFromList_Pro(
		const UAnimInstance* AnimInstance,
		EGBWAnimNodeBlendState StateBlendState,
		const TMap<UAnimSequence*, FGBWAnimSequencePlaySettings>& AnimSequenceSettingList,
		UAnimSequence*& AnimRes,
		FGBWAnimSequencePlaySettings& PlaySettings)
{
	AnimRes = nullptr;
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		bool bHoldNowWhenAnimActive = true;
		bool bHoldNowWhenAnimIsPreAnim = false;
		switch (StateBlendState)
		{
		case StateBlendIn:
			bHoldNowWhenAnimActive = true;
			bHoldNowWhenAnimIsPreAnim = false;
			break;
		case StateBlendOut: 
			bHoldNowWhenAnimActive = true;
			bHoldNowWhenAnimIsPreAnim = true;
			break;
		case NoBlend: 
			bHoldNowWhenAnimActive = true;
			bHoldNowWhenAnimIsPreAnim = false;
			break;
		case StateReBlendIn:
			bHoldNowWhenAnimActive = false;
			bHoldNowWhenAnimIsPreAnim = false;
			break;
		default: 
			bHoldNowWhenAnimActive = true;
			bHoldNowWhenAnimIsPreAnim = false;
		}
		TArray<UAnimSequence*> AnimSequenceList ={};
		AnimSequenceSettingList.GetKeys(AnimSequenceList);
		AnimRes = UGBWAnimFuncLib::GetNextAnimSequenceFromList(AnimInstance->GetOwningActor(),bHoldNowWhenAnimActive,bHoldNowWhenAnimIsPreAnim,AnimSequenceList);
		if (AnimRes)
		{
			PlaySettings = AnimSequenceSettingList.FindRef(AnimRes);
		}
	}
}

bool UGBWAnimFuncLib::GetAnimPlayState(
		UAnimInstance* Instance,
		FGBWAnimStateGetType GetType,
		float DeltaTime,
		FGBWAnimPlayStateSet OldAnimPlayStateSet,
		FGBWAnimPlayStateSet& AnimPlayStateSet)
{
	if (!Instance)
	{
		return false;
	}

	TArray<FGBWAnimPlayState> Res_AnimPlayState;
	FGBWAnimPlayState MainAnimPlayState = FGBWAnimPlayState();
	TArray<UAnimNotifyState*> InNotifyStateSet;
	TMap<FName,FGBWAnimCurveValue> InCurveValueState;

	const IAnimClassInterface* Interface = IAnimClassInterface::GetFromClass(Instance->GetClass());
	
	const TArray<FStructProperty*>& LinkedAnimGraphNodeProperties = Interface->GetLinkedAnimGraphNodeProperties();
	TArray<UAnimInstance*> LinkedAnimInstances = {};
	TArray<IAnimClassInterface*> LinkedAnimInstanceInterfaces = {};

	if (GetType.IncludeLinkedAnimInstance)
	{
		for(auto& LinkedAnimGraphNode : LinkedAnimGraphNodeProperties)
		{
			if (LinkedAnimGraphNode->Struct->IsChildOf(FAnimNode_LinkedAnimGraph::StaticStruct()))
			{
				if (FAnimNode_LinkedAnimGraph* NodePtr = LinkedAnimGraphNode->ContainerPtrToValuePtr<FAnimNode_LinkedAnimGraph>(Instance))
				{
					bool bShouldInclude = false;
					for (auto Class : GetType.IncludeLinkedAnimInstanceClassSet)
					{
						if (NodePtr->InstanceClass->IsChildOf(Class))
						{
							bShouldInclude = true;
							break;
						}
					}
					if (bShouldInclude)
					{
						LinkedAnimInstances.Add(NodePtr->GetTargetInstance<UAnimInstance>());
						LinkedAnimInstanceInterfaces.Add(IAnimClassInterface::GetFromClass(NodePtr->InstanceClass));
					}
				}
			}
		}
	}

	TMap<FStructProperty*, UAnimInstance*> AnimNodePropertiesMap;
	
	TArray<FStructProperty*> AnimNodePropertyArr = Interface->GetAnimNodeProperties();
	if (GetType.IncludeMainAnimInstance)
	{
		for(auto AnimNodeProperty : AnimNodePropertyArr)
		{
			AnimNodePropertiesMap.Add(AnimNodeProperty,Instance);
		}
	}

	for (int i = 0; i < LinkedAnimInstances.Num(); i++)
	{
		AnimNodePropertyArr = LinkedAnimInstanceInterfaces[i]->GetAnimNodeProperties();
		for(auto AnimNodeProperty : AnimNodePropertyArr)
		{
			AnimNodePropertiesMap.Add(AnimNodeProperty,LinkedAnimInstances[i]);
		}
	}
	
	for(auto AnimNodePropertyItem : AnimNodePropertiesMap)
	{
		if (AnimNodePropertyItem.Key->Struct->IsChildOf(FAnimNode_AssetPlayerBase::StaticStruct()))
		{
			FAnimNode_AssetPlayerBase* NodePtr = AnimNodePropertyItem.Key->ContainerPtrToValuePtr<FAnimNode_AssetPlayerBase>(AnimNodePropertyItem.Value);
			if (NodePtr->GetAnimAsset())
			{
				EGBWAnimAssetType AssetType = EGBWAnimAssetType::None;
				if (NodePtr->GetAnimAsset()->GetClass()->IsChildOf(UAnimSequenceBase::StaticClass()))
				{
					AssetType = EGBWAnimAssetType::AnimSequence;
				}
				else if (NodePtr->GetAnimAsset()->GetClass()->IsChildOf(UBlendSpace::StaticClass()))
				{
					AssetType = EGBWAnimAssetType::AnimBlendSpace;
				}
				
				Res_AnimPlayState.Add(FGBWAnimPlayState(
					AnimNodePropertyItem.Value,
					AssetType,
					NodePtr->GetAnimAsset(),
					NodePtr->GetNodeIndex(),
					0.0f,
					0.0f,
					NodePtr->GetAnimAsset()->GetPlayLength(),
					NodePtr->GetAccumulatedTime(),
					NodePtr->GetAnimAsset()->GetPlayLength(),
					NodePtr->GetAccumulatedTime(),
					NodePtr->GetCachedBlendWeight()
				));
			}
		}
	}

	if (const FAnimMontageInstance* MontageIns = Instance->GetActiveMontageInstance())
	{
		if (MontageIns->Montage)
		{
			float StartTime = 0.0f;
			float EndTime = 0.0f;
			MontageIns->Montage->GetSectionStartAndEndTime(MontageIns->Montage->GetSectionIndex(MontageIns->GetCurrentSection()),StartTime,EndTime);
			Res_AnimPlayState.Add(FGBWAnimPlayState(
					Instance,
					EGBWAnimAssetType::AnimMontage,
					MontageIns->Montage,
					-1,
					MontageIns->Montage->BlendIn.GetBlendTime(),
					MontageIns->Montage->BlendOut.GetBlendTime(),
					GetMontageSectionLength(MontageIns->Montage,MontageIns->GetCurrentSection()),
					MontageIns->GetPosition() - StartTime,
					MontageIns->Montage->GetPlayLength(),
					MontageIns->GetPosition(),
					MontageIns->GetWeight()
				));
		}
	}

	//Check Asset PlayState
	for (int i = 0; i < Res_AnimPlayState.Num(); i++)
	{
		FGBWAnimPlayState OldAnimPlayState;
		if ( UGBWAnimFuncLib::GBWGetAnimPlayStateFromArrayByAsset(
			OldAnimPlayStateSet.AnimPlayStateSet,
			Res_AnimPlayState[i].AnimAsset,
			Res_AnimPlayState[i].AnimNodeIndex,
			OldAnimPlayState))
		{
			if (OldAnimPlayState.AnimPlayPosition != Res_AnimPlayState[i].AnimPlayPosition
				|| OldAnimPlayState.AnimBlendWeight != Res_AnimPlayState[i].AnimBlendWeight)
			{
				if (OldAnimPlayState.AnimBlendWeight > Res_AnimPlayState[i].AnimBlendWeight)
				{
					Res_AnimPlayState[i].PlayState = EGBWAnimPlayStateType::BlendOut;
				}
				else if(OldAnimPlayState.AnimBlendWeight < Res_AnimPlayState[i].AnimBlendWeight)
				{
					Res_AnimPlayState[i].PlayState = EGBWAnimPlayStateType::BlendIn;
				}
				else
				{
					Res_AnimPlayState[i].PlayState = EGBWAnimPlayStateType::Active;
				}
				
				if (OldAnimPlayState.AnimPlayPosition > Res_AnimPlayState[i].AnimPlayPosition
					&& (OldAnimPlayState.PlayState == EGBWAnimPlayStateType::Active
						|| OldAnimPlayState.PlayState == EGBWAnimPlayStateType::BlendIn))
				{
					Res_AnimPlayState[i].bIsReplay = true;
				}
				else
				{
					Res_AnimPlayState[i].bIsReplay = false;
				}
			}
			else
			{
				Res_AnimPlayState[i].PlayState = EGBWAnimPlayStateType::Deactivated;
			}
		}
		else
		{
			Res_AnimPlayState[i].PlayState = EGBWAnimPlayStateType::Deactivated;
		}
	}

	//Get MainAnimAsset
	float MaxBlendWeight = 0.0f;
	for (int i=0; i<Res_AnimPlayState.Num(); i++)
	{
		if (Res_AnimPlayState[i].AnimAsset
			&& (Res_AnimPlayState[i].PlayState == EGBWAnimPlayStateType::Active || Res_AnimPlayState[i].PlayState == EGBWAnimPlayStateType::BlendIn)
			&& Res_AnimPlayState[i].AnimBlendWeight >= MaxBlendWeight)
		{
			MaxBlendWeight = Res_AnimPlayState[i].AnimBlendWeight;
			MainAnimPlayState = Res_AnimPlayState[i];
		}
	}

	int MainAssetPriorityLevel = -1;
	for (auto AnimPlayState : Res_AnimPlayState)
	{
		TArray<UAnimMetaData*> AnimMetaData = {};
		if (AnimPlayState.AnimAsset
			&& (AnimPlayState.PlayState == EGBWAnimPlayStateType::Active || AnimPlayState.PlayState == EGBWAnimPlayStateType::BlendIn))
		{
			AnimMetaData = AnimPlayState.AnimAsset->GetMetaData();

			for (auto MetaData : AnimMetaData)
			{
				if (UGBWAnimMetaData_MainAssetSetting* MainAssetSetting = Cast<UGBWAnimMetaData_MainAssetSetting>(MetaData))
				{
					if (MainAssetSetting->MainAssetPriorityLevel >= MainAssetPriorityLevel)
					{
						MainAssetPriorityLevel = MainAssetSetting->MainAssetPriorityLevel;
						MainAnimPlayState = AnimPlayState;
						break;
					}
				}
			}
		}
	}

	if (!MainAnimPlayState.AnimAsset)
	{
		MainAnimPlayState = OldAnimPlayStateSet.MainAnimPlayState;
		/*MainAnimPlayState.AnimPlayDuration += DeltaTime;
		MainAnimPlayState.AnimPlayPosition += DeltaTime;*/
	}
	
	//Get Anim StateMachineState //Check BlendFinished
	TArray<FGBWAnimStateMachineNodeInfo> StateMachineState = {};
	bool bIsBlendFinished = true;
	
	TMap<FStructProperty*, UAnimInstance*> StateMachineNodePropertiesMap;
	TArray<FStructProperty*> StateMachineNodePropertyArr = Interface->GetStateMachineNodeProperties();
	for(auto StateMachineNodeProperty : StateMachineNodePropertyArr)
	{
		StateMachineNodePropertiesMap.Add(StateMachineNodeProperty,Instance);
	}
	for (int i = 0; i < LinkedAnimInstances.Num(); i++)
	{
		StateMachineNodePropertyArr = LinkedAnimInstanceInterfaces[i]->GetStateMachineNodeProperties();
		for(auto StateMachineNodeProperty : StateMachineNodePropertyArr)
		{
			StateMachineNodePropertiesMap.Add(StateMachineNodeProperty,LinkedAnimInstances[i]);
		}
	}
	
	for (auto StateMachineNodePropertyItem : StateMachineNodePropertiesMap)
	{
		if (StateMachineNodePropertyItem.Key->Struct->IsChildOf(FAnimNode_StateMachine::StaticStruct()))
		{
			if (FAnimNode_StateMachine* NodePtr = StateMachineNodePropertyItem.Key->ContainerPtrToValuePtr<FAnimNode_StateMachine>(StateMachineNodePropertyItem.Value))
			{
				bool bIsActive = false;
				FGBWAnimStateMachineNodeInfo OldStateMachineNodeInfo = FGBWAnimStateMachineNodeInfo();
				if (UGBWAnimFuncLib::GBWGetAnimStateMachineStateFromArrayByNodeIndex(
						OldAnimPlayStateSet.StateMachineState,
						NodePtr->GetNodeIndex(),
						OldStateMachineNodeInfo))
				{
					if (NodePtr && NodePtr->GetCurrentStateElapsedTime() != OldStateMachineNodeInfo.ElapsedTime)
					{
						bIsActive = true;
					}
					if (NodePtr && NodePtr->GetNodeIndex() == OldStateMachineNodeInfo.NodeIndex && NodePtr->GetStateWeight(NodePtr->GetCurrentState()) != OldStateMachineNodeInfo.CurrentStateWeight)
					{
						bIsBlendFinished = false;
					}
				}
				
				StateMachineState.Add(FGBWAnimStateMachineNodeInfo(
					StateMachineNodePropertyItem.Value,
					NodePtr->GetNodeIndex(),
					NodePtr->GetCurrentStateName(),
					OldStateMachineNodeInfo.CurrentStateName != NodePtr->GetCurrentStateName() && OldStateMachineNodeInfo.ElapsedTime>=0.2f? OldStateMachineNodeInfo.CurrentStateName : OldStateMachineNodeInfo.LastStateName,
					NodePtr->GetStateWeight(NodePtr->GetCurrentState()),
					bIsActive,
					NodePtr->GetCurrentStateElapsedTime(),
					OldStateMachineNodeInfo.CurrentStateName != NodePtr->GetCurrentStateName() && OldStateMachineNodeInfo.ElapsedTime>=0.2f? OldStateMachineNodeInfo.ElapsedTime : OldStateMachineNodeInfo.LastStateElapsedTime
				));
			}
		}
	}

	//Make AnimPlayStateSet
	AnimPlayStateSet = FGBWAnimPlayStateSet(Instance, LinkedAnimInstances, MainAnimPlayState, Res_AnimPlayState, StateMachineState, bIsBlendFinished);
	
	if (AnimPlayStateSet.IsValid())
	{
		//Get Anim NotifyState
		if (AnimPlayStateSet.MainAnimPlayState.AssetType == EGBWAnimAssetType::AnimMontage
			|| AnimPlayStateSet.MainAnimPlayState.AssetType == EGBWAnimAssetType::AnimSequence)
		{
			UAnimSequenceBase* A = Cast<UAnimSequenceBase>(AnimPlayStateSet.MainAnimPlayState.AnimAsset);
			if (A)
			{
				for (auto Notify : A->Notifies)
				{
					if (Notify.NotifyStateClass
						&& Notify.GetTriggerTime() <= AnimPlayStateSet.MainAnimPlayState.AnimPlayPosition
						&& Notify.GetEndTriggerTime() >= AnimPlayStateSet.MainAnimPlayState.AnimPlayPosition)
					{
						AnimPlayStateSet.NotifyStateSet.Add(Notify.NotifyStateClass);
					}
				}
			}
		}
		else if (AnimPlayStateSet.MainAnimPlayState.AssetType == EGBWAnimAssetType::AnimBlendSpace)
		{
			UBlendSpace* B = Cast<UBlendSpace>(AnimPlayStateSet.MainAnimPlayState.AnimAsset);
			for (auto BSSample : B->GetBlendSamples())
			{
				for (auto Notify : BSSample.Animation->Notifies)
				{
					if (Notify.NotifyStateClass
						&& Notify.GetTriggerTime() <= AnimPlayStateSet.MainAnimPlayState.AnimPlayPosition
						&& Notify.GetEndTriggerTime() >= AnimPlayStateSet.MainAnimPlayState.AnimPlayPosition)
					{
						AnimPlayStateSet.NotifyStateSet.Add(Notify.NotifyStateClass);
					}
				}
			}
			/*for (const auto Notify : Instance->ActiveAnimNotifyState)
			{
				if (Notify.NotifyStateClass)
				{
					AnimPlayStateSet.NotifyStateSet.Add(Notify.NotifyStateClass);
				}
			}*/
		}

		//Get Anim Curve State
		TArray<FName> ActiveCurveNames = {};
		Instance->GetActiveCurveNames(EAnimCurveType::AttributeCurve,ActiveCurveNames);
		
		if (AnimPlayStateSet.MainAnimPlayState.AssetType == EGBWAnimAssetType::AnimMontage
			|| AnimPlayStateSet.MainAnimPlayState.AssetType == EGBWAnimAssetType::AnimSequence)
		{
			UAnimSequenceBase* A = Cast<UAnimSequenceBase>(AnimPlayStateSet.MainAnimPlayState.AnimAsset);
			if (A)
			{
				for (const auto ActiveCurveName : ActiveCurveNames)
				{
					float CValue = 0.0f;
					float CValueDelta = 0.0f;
					if (UGBWAnimFuncLib::GetAnimCurveValue(
						A,
						ActiveCurveName,
						AnimPlayStateSet.MainAnimPlayState.BlendInTime,
						AnimPlayStateSet.MainAnimPlayState.AnimPlayPosition,
						DeltaTime,
						CValue,
						CValueDelta))
					{
						AnimPlayStateSet.CurveValueState.Add(
							FGBWAnimCurveValue(
								ActiveCurveName,
								CValue,
								CValueDelta
							));
					}
				}
			}
		}
		else if (AnimPlayStateSet.MainAnimPlayState.AssetType == EGBWAnimAssetType::AnimBlendSpace)
		{
			UBlendSpace* B = Cast<UBlendSpace>(AnimPlayStateSet.MainAnimPlayState.AnimAsset);
			UAnimSequenceBase* A = Cast<UAnimSequenceBase>(B->GetBlendSample(0).Animation);
			if (A)
			{
				const float AnimScale = A->GetPlayLength() / B->GetPlayLength();
				for (const auto ActiveCurveName : ActiveCurveNames)
				{
					float CValue = 0.0f;
					float CValueDelta = 0.0f;
					if (UGBWAnimFuncLib::GetAnimCurveValue(
						A,
						ActiveCurveName,
						AnimPlayStateSet.MainAnimPlayState.BlendInTime,
						AnimPlayStateSet.MainAnimPlayState.AnimPlayPosition * AnimScale,
						DeltaTime * AnimScale,
						CValue,
						CValueDelta))
					{
						AnimPlayStateSet.CurveValueState.Add(
							FGBWAnimCurveValue(
								ActiveCurveName,
								CValue,
								CValueDelta
							));
					}
				}
			}
		}
	}
	
	return AnimPlayStateSet.IsValid();
}

bool UGBWAnimFuncLib::GBWGetAnimPlayState(AActor* InActor, FGBWAnimPlayStateSet& AnimPlayStateSet)
{
	if (UGBWAnimComponent* AComp = UGBWAnimFuncLib::GetGBWAnimComponent(InActor))
	{
		return AComp->GetAnimPlayState(AnimPlayStateSet);
	}

	return false;
}

bool UGBWAnimFuncLib::GBWGetAnimPlayState_Local(AActor* InActor, FGBWAnimPlayStateSet& AnimPlayStateSet)
{
	if (UGBWAnimComponent* AComp = UGBWAnimFuncLib::GetGBWAnimComponent(InActor))
	{
		return AComp->GetAnimPlayState_Local(AnimPlayStateSet);
	}

	return false;
}

bool UGBWAnimFuncLib::GBWGetAnimPlayStateFromArrayByAsset(
		TArray<FGBWAnimPlayState> AnimPlayStateArr,
		UAnimationAsset* AnimAsset,
		int32 AnimNodeIndex,
		FGBWAnimPlayState& AnimPlayState)
{
	for (const auto Element : AnimPlayStateArr)
	{
		if (!Element.AnimAsset)
		{
			continue;
		}
		if (AnimNodeIndex<0 || (AnimNodeIndex >=0 && AnimNodeIndex == Element.AnimNodeIndex))
		{
			AnimPlayState = Element;
			return true;
		}
	}

	return false;
}

bool UGBWAnimFuncLib::GBWGetAnimStateMachineStateFromArrayByNodeIndex(
	TArray<FGBWAnimStateMachineNodeInfo> StateMachineNodeInfoArr, int32 NodeIndex,
	FGBWAnimStateMachineNodeInfo& StateMachineNodeInfo)
{
	bool bIsGet = false;
	for (const auto Element : StateMachineNodeInfoArr)
	{
		if (Element.NodeIndex == NodeIndex)
		{
			StateMachineNodeInfo = Element;
			bIsGet = true;
			break;
		}
	}

	return bIsGet;
}

FString UGBWAnimFuncLib::GetDebugStringFromAnimPlayState(FGBWAnimPlayStateSet AnimPlayStateSet)
{
	FString Res = "IsBlendFinished:" + (AnimPlayStateSet.bIsBlendFinished? FString("True") : FString("False"));
	Res = Res + "\nMainAnimPlayState:" + FString::Printf(
										  TEXT("\n    (%s)(%s)(%d)%s %s (%f / %f)  (%f / %f)  %f")
										  ,*(StaticEnum<EGBWAnimPlayStateType>()->GetNameStringByIndex( ( int32 ) AnimPlayStateSet.MainAnimPlayState.PlayState))
										  ,*(AnimPlayStateSet.MainAnimPlayState.bIsReplay? FString("r") : FString("."))
										  ,AnimPlayStateSet.MainAnimPlayState.AnimNodeIndex
										  ,*(StaticEnum<EGBWAnimAssetType>()->GetNameStringByIndex( ( int32 ) AnimPlayStateSet.MainAnimPlayState.AssetType))
										  ,*(AnimPlayStateSet.MainAnimPlayState.AnimAsset? AnimPlayStateSet.MainAnimPlayState.AnimAsset->GetName() : "")
										  ,AnimPlayStateSet.MainAnimPlayState.AnimPlayDuration
										  ,AnimPlayStateSet.MainAnimPlayState.AnimPlayLength
										  ,AnimPlayStateSet.MainAnimPlayState.AnimPlayPosition
										  ,AnimPlayStateSet.MainAnimPlayState.AnimAllPlayLength
										  ,AnimPlayStateSet.MainAnimPlayState.AnimBlendWeight
									  );
	Res = Res + "\nAnimPlayedCache:" + AnimPlayStateSet.AnimPlayedCache.GetAnimSequencePlayedCacheDebugString(5);
	
	Res = Res + "\nAnimStateMachineStates:";
	for (int i=0; i<AnimPlayStateSet.StateMachineState.Num(); i++)
	{
		Res = Res + FString::Printf(
			  TEXT("\n    (%s)(%d) %s (%f) (%f) - %s (%f)")
			  ,*(AnimPlayStateSet.StateMachineState[i].bIsActive? FString("a") : FString("."))
			  ,AnimPlayStateSet.StateMachineState[i].NodeIndex
			  ,*AnimPlayStateSet.StateMachineState[i].CurrentStateName.ToString()
			  ,AnimPlayStateSet.StateMachineState[i].CurrentStateWeight
			  ,AnimPlayStateSet.StateMachineState[i].ElapsedTime
			  ,*AnimPlayStateSet.StateMachineState[i].LastStateName.ToString()
			  ,AnimPlayStateSet.StateMachineState[i].LastStateElapsedTime
		  );
	}
	
	Res = Res + "\nAllAnimPlayState";
	for (int i=0; i<AnimPlayStateSet.AnimPlayStateSet.Num(); i++)
	{
		Res = Res + FString::Printf(
			  TEXT("\n    (%s)(%s)(%d)%s %s (%f / %f)  (%f / %f)  %f")
			  ,*(StaticEnum<EGBWAnimPlayStateType>()->GetNameStringByIndex( ( int32 ) AnimPlayStateSet.AnimPlayStateSet[i].PlayState))
			  ,*(AnimPlayStateSet.AnimPlayStateSet[i].bIsReplay? FString("r") : FString("."))
			  ,AnimPlayStateSet.AnimPlayStateSet[i].AnimNodeIndex
			  ,*(StaticEnum<EGBWAnimAssetType>()->GetNameStringByIndex( ( int32 ) AnimPlayStateSet.AnimPlayStateSet[i].AssetType))
			  ,*(AnimPlayStateSet.AnimPlayStateSet[i].AnimAsset? AnimPlayStateSet.AnimPlayStateSet[i].AnimAsset->GetName() : "")
			  ,AnimPlayStateSet.AnimPlayStateSet[i].AnimPlayDuration
			  ,AnimPlayStateSet.AnimPlayStateSet[i].AnimPlayLength
			  ,AnimPlayStateSet.AnimPlayStateSet[i].AnimPlayPosition
			  ,AnimPlayStateSet.AnimPlayStateSet[i].AnimAllPlayLength
			  ,AnimPlayStateSet.AnimPlayStateSet[i].AnimBlendWeight
		  );
	}

	Res = Res + "\nAnimNotifyState:";
	for (int i=0; i<AnimPlayStateSet.NotifyStateSet.Num(); i++)
	{
		Res = Res + FString::Printf(
			  TEXT("\n    %s")
			  ,*AnimPlayStateSet.NotifyStateSet[i]->GetName()
		);
	}

	Res = Res + "\nCurveValueState:";
	for (auto Curve : AnimPlayStateSet.CurveValueState)
	{
		Res = Res + FString::Printf(
			  TEXT("\n    %s (%f  %f)")
			  ,*Curve.CurveName.ToString()
			  ,Curve.Value
			  ,Curve.DeltaValue
		  );
	}
	
	return Res;
}

UAnimNotifyState* UGBWAnimFuncLib::GetNotifyStateByClass(FGBWAnimPlayStateSet AnimPlayStateSet,
	TSubclassOf<UAnimNotifyState> NotifyStateClass)
{
	for (const auto Element : AnimPlayStateSet.NotifyStateSet)
	{
		if (Element && Element->GetClass()->IsChildOf(NotifyStateClass))
		{
			return Element;
		}
	}
	return nullptr;
}

FGBWAnimCurveValue UGBWAnimFuncLib::GetCurveValueByName(FGBWAnimPlayStateSet AnimPlayStateSet, FName CurveName)
{
	for (auto Element : AnimPlayStateSet.CurveValueState)
	{
		if (Element.CurveName == CurveName)
		{
			return Element;
		}
	}

	return FGBWAnimCurveValue();
}

float UGBWAnimFuncLib::GetMontageSectionLength(UAnimMontage* Montage, FName SectionName)
{
	if (!Montage)
	{
		return -1;
	}

	return Montage->GetSectionLength(Montage->GetSectionIndex(SectionName));
}

bool UGBWAnimFuncLib::GetAnimCurveValue(
	UAnimSequenceBase* Anim,
	FName CurveName,
	float TimeOffset,
	float Position,
	float DeltaTime,
	float& Result,
	float& ResultDelta)
{
	if (Anim)
	{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
		if (Anim->HasCurveData(CurveName))
		{
			const float AnimLength = Anim->GetPlayLength();
			Result = Anim->EvaluateCurveData(CurveName, UKismetMathLibrary::FClamp(Position - TimeOffset,0.0f,AnimLength));
			ResultDelta = Anim->EvaluateCurveData(CurveName, UKismetMathLibrary::FClamp(Position + DeltaTime - TimeOffset,0.0f,AnimLength)) - Result;
			return true;
		}
		return false;
#else
		const USkeleton* Skeleton = Anim->GetSkeleton();
		if (!Skeleton)
		{
			return false;
		}
		const FSmartNameMapping* NameMapping = Skeleton->GetSmartNameContainer(USkeleton::AnimCurveMappingName);
		if (!NameMapping)
		{
			return false;
		}
		const USkeleton::AnimCurveUID Uid = NameMapping->FindUID(CurveName);
		if (Uid == INDEX_NONE)
		{
			return false;
		}
		const float AnimLength = Anim->GetPlayLength();
		Result = Anim->EvaluateCurveData(Uid, UKismetMathLibrary::FClamp(Position - TimeOffset,0.0f,AnimLength));
		ResultDelta = Anim->EvaluateCurveData(Uid, UKismetMathLibrary::FClamp(Position + DeltaTime - TimeOffset,0.0f,AnimLength)) - Result;
		return true;
#endif
	}

	return false;
}

bool UGBWAnimFuncLib::IsPlayingSlotAnim(
	UAnimInstance* Instance,
	FName SlotName,
	UAnimSequenceBase*& AnimSequence,
	UAnimMontage*& OutMontage)
{
	for (int32 InstanceIndex = 0; InstanceIndex < Instance->MontageInstances.Num(); InstanceIndex++)
	{
		// check if this is playing
		const FAnimMontageInstance* MontageInstance = Instance->MontageInstances[InstanceIndex];
		// make sure what is active right now is transient that we created by request
		if (MontageInstance && MontageInstance->IsActive() && MontageInstance->IsPlaying())
		{
			UAnimMontage* CurMontage = MontageInstance->Montage;
			if (CurMontage && CurMontage->GetOuter() == GetTransientPackage())
			{
				const FAnimTrack* AnimTrack = CurMontage->GetAnimationData(SlotName);
				if (AnimTrack && AnimTrack->AnimSegments.Num() == 1)
				{
					OutMontage = CurMontage;
					AnimSequence = AnimTrack->AnimSegments[0].GetAnimReference();
					if (AnimSequence)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool UGBWAnimFuncLib::Anim_IsBlendFinished(AActor* InActor)
{
	FGBWAnimPlayStateSet AnimPlayStateSet = FGBWAnimPlayStateSet();
	if (GBWGetAnimPlayState(InActor,AnimPlayStateSet)
		&& AnimPlayStateSet.bIsBlendFinished)
	{
		return true;
	}

	return false;
}

bool UGBWAnimFuncLib::Anim_Pure_IsBlendFinished(const UAnimInstance* AnimInstance)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWAnimFuncLib::Anim_IsBlendFinished(AnimInstance->GetOwningActor());
	}

	return false;
}

void UGBWAnimFuncLib::Anim_Pure_OnlyServer(bool InCondition, const UAnimInstance* AnimInstance, bool& OutCondition)
{
	if (AnimInstance && AnimInstance->GetOwningActor() && AnimInstance->GetOwningActor()->HasAuthority())
	{
		OutCondition = InCondition;
		return;
	}

	OutCondition = false;
}

void UGBWAnimFuncLib::GBW_AnimSM_FollowServer(const UAnimInstance* AnimInstance, FAnimUpdateContext Context, FAnimNodeReference Node)
{
	if (AnimInstance && AnimInstance->GetOwningActor() && !AnimInstance->GetOwningActor()->HasAuthority())
	{
		if (const UGBWAnimComponent* AComp = UGBWAnimFuncLib::GetGBWAnimComponent(AnimInstance->GetOwningActor()))
		{
			FAnimationStateMachineReference AnimationStateMachine = FAnimationStateMachineReference();
			bool bIsSMNode = false;
			UAnimationStateMachineLibrary::ConvertToAnimationStateMachinePure(Node, AnimationStateMachine, bIsSMNode);
			if (bIsSMNode)
			{
				if (const FAnimNode_StateMachine* SMNode = Node.GetAnimNodePtr<FAnimNode_StateMachine>())
				{
					FGBWAnimStateMachineNodeInfo SMInfo = FGBWAnimStateMachineNodeInfo();
					if (AComp->GetStateMachineInfoByNodeIndex(SMNode->GetNodeIndex(),SMInfo) && SMInfo.bIsActive)
					{
						if (SMNode->GetCurrentStateName() != SMInfo.CurrentStateName)
						{
							UAnimationStateMachineLibrary::SetState(
                            	Context,
                            	AnimationStateMachine,
                            	SMInfo.CurrentStateName,
                            	0.2f,
                            	ETransitionLogicType::Type::TLT_StandardBlend,
                            	nullptr,
                            	EAlphaBlendOption::Linear,
                            	nullptr);
						}
					}
				}
			}
		}
	}
}
