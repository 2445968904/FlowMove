// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "ANS/GBWFlowMoveANS_Perceptron.h"

#include "GBWFlowMoveComponent.h"
#include "GBWFlowMoveFuncLib.h"

void UGBWFlowMoveANS_Perceptron::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	UGBWFlowMoveComponent* TheFlowMoveComponent = UGBWFlowMoveFuncLib::FM_GetFlowMoveComponent(MeshComp->GetOwner());
	if (TheFlowMoveComponent && TheFlowMoveComponent->IsInServer())
	{
		for (const auto Element : PerceptronTags)
		{
			TheFlowMoveComponent->ActivePerceptron(Element);	
		}
	}
}

void UGBWFlowMoveANS_Perceptron::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UGBWFlowMoveANS_Perceptron::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	UGBWFlowMoveComponent* TheFlowMoveComponent = UGBWFlowMoveFuncLib::FM_GetFlowMoveComponent(MeshComp->GetOwner());
	if (TheFlowMoveComponent && TheFlowMoveComponent->IsInServer())
	{
		for (const auto Element : PerceptronTags)
		{
			TheFlowMoveComponent->DeactivePerceptron(Element);
		}
	}
}

FString UGBWFlowMoveANS_Perceptron::GetNotifyName_Implementation() const
{
	if(!Description.IsEmpty())
	{
		return Description;
	}

	return UAnimNotifyState::GetNotifyName_Implementation();
}
