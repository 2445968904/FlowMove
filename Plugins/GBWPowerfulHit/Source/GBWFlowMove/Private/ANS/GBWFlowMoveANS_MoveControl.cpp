// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "ANS/GBWFlowMoveANS_MoveControl.h"
#include "GBWFlowMoveComponent.h"

void UGBWFlowMoveANS_MoveControl::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	MoveControlSettings.ActionGuid = FGuid::NewGuid();
	AnimationNow = Animation;
	AnimTotalDuration = TotalDuration;
	AnimDuration = 0.0f;
}
//这个地方的Tick需要留意一下
void UGBWFlowMoveANS_MoveControl::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	AnimDuration += FrameDeltaTime;

	if (AnimDuration >= AnimTotalDuration)
	{
		AnimDuration = AnimDuration - AnimTotalDuration;
		MoveControlSettings.ActionGuid = FGuid::NewGuid();
	}
}

void UGBWFlowMoveANS_MoveControl::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

FString UGBWFlowMoveANS_MoveControl::GetNotifyName_Implementation() const
{
	if(!Description.IsEmpty())
	{
		return Description;
	}

	return UAnimNotifyState::GetNotifyName_Implementation();
}

bool UGBWFlowMoveANS_MoveControl::GetAnimPlayState(UAnimSequenceBase*& Animation, float& TotalDuration, float& Duration) const
{
	if (AnimationNow && AnimTotalDuration>0.0f && AnimDuration>=0.0f)
	{
		Animation = AnimationNow;
		TotalDuration = AnimTotalDuration;
		Duration = AnimDuration;
		return true;
	}
	return false;
}
