// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "GBWPowerfulAnimInstance.h"

#include "GBWAnimFuncLib.h"
#include "Animation/AnimExecutionContext.h"
#include "Animation/AnimNode_StateMachine.h"

EGBWAnimNodeBlendState UGBWPowerfulAnimInstance::GetStateBlendState(int32 MachineIndex, int32 StateIndex) const
{
	if (UGBWAnimComponent* AnimComponent = UGBWAnimFuncLib::GetGBWAnimComponent(TryGetPawnOwner()))
	{
		const FAnimNode_StateMachine* MachineInstance = GetStateMachineInstance(MachineIndex);
		FBakedAnimationState BakedState = MachineInstance->GetStateInfo(StateIndex);

		const IAnimClassInterface* AnimClassInterface = IAnimClassInterface::GetFromClass(GetClass());
		const TArray<FStructProperty*>& AnimNodeProperties = AnimClassInterface->GetAnimNodeProperties();

		for (const auto PlayerNodeIndex : BakedState.PlayerNodeIndices)
		{
			const int32 InstanceIdx = AnimNodeProperties.Num() - 1 - PlayerNodeIndex;
			bool bIsReplay = false;
			const EGBWAnimPlayStateType PlayState = AnimComponent->GetAnimPlayStateByNodeIndex(InstanceIdx, bIsReplay);

			if (bIsReplay)
			{
				return EGBWAnimNodeBlendState::StateReBlendIn;
			}
			else if (PlayState == EGBWAnimPlayStateType::BlendIn)
			{
				return EGBWAnimNodeBlendState::StateBlendIn;
			}
			else if (PlayState == EGBWAnimPlayStateType::BlendOut)
			{
				return EGBWAnimNodeBlendState::StateBlendOut;
			}
		}
	}
	
	return EGBWAnimNodeBlendState::NoBlend;
}

void UGBWPowerfulAnimInstance::AnimSM_FollowServer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	UGBWAnimFuncLib::GBW_AnimSM_FollowServer(this, Context, Node);
}
