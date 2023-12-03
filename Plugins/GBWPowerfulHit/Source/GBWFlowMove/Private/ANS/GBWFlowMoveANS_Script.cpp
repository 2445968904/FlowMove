// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "ANS/GBWFlowMoveANS_Script.h"

#include "GBWFlowMoveComponent.h"
#include "GBWFlowMoveFuncLib.h"

void UGBWFlowMoveANS_Script::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	UGBWFlowMoveComponent* TheFlowMoveComponent = UGBWFlowMoveFuncLib::FM_GetFlowMoveComponent(MeshComp->GetOwner());
	if (TheFlowMoveComponent && TheFlowMoveComponent->IsInServer())
	{
		for (const auto Element : ScriptTags)
		{
			TheFlowMoveComponent->ActiveScript(Element);	
		}
	}
}

void UGBWFlowMoveANS_Script::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UGBWFlowMoveANS_Script::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	UGBWFlowMoveComponent* TheFlowMoveComponent = UGBWFlowMoveFuncLib::FM_GetFlowMoveComponent(MeshComp->GetOwner());
	if (TheFlowMoveComponent && TheFlowMoveComponent->IsInServer())
	{
		for (const auto Element : ScriptTags)
		{
			TheFlowMoveComponent->DeactiveScript(Element);
		}
	}
}

FString UGBWFlowMoveANS_Script::GetNotifyName_Implementation() const
{
	if(!Description.IsEmpty())
    {
    	return Description;
    }

    return UAnimNotifyState::GetNotifyName_Implementation();
}
