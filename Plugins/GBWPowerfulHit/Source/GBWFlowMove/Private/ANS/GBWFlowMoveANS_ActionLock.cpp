// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "ANS/GBWFlowMoveANS_ActionLock.h"
#include "GBWFlowMoveComponent.h"
#include "GBWFlowMoveFuncLib.h"


void UGBWFlowMoveANS_ActionLock::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UGBWFlowMoveANS_ActionLock::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                            float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UGBWFlowMoveANS_ActionLock::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

FString UGBWFlowMoveANS_ActionLock::GetNotifyName_Implementation() const
{
	if(!Description.IsEmpty())
	{
		return Description;
	}

	return UAnimNotifyState::GetNotifyName_Implementation();
}
