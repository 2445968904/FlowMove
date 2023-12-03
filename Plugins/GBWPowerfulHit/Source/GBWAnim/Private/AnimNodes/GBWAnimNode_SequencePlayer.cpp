// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "AnimNodes/GBWAnimNode_SequencePlayer.h"

#include "AnimEncoding.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimTrace.h"

#define LOCTEXT_NAMESPACE "GBWAnimNode_SequencePlayer"

/////////////////////////////////////////////////////
// FGBWAnimSequencePlayerNode

bool FGBWAnimNode_SequencePlayer::SetSequence(UAnimSequenceBase* InSequence)
{
#if WITH_EDITORONLY_DATA
	Sequence = InSequence;
	GET_MUTABLE_ANIM_NODE_DATA(TObjectPtr<UAnimSequenceBase>, Sequence) = InSequence;
#endif
	
	if(TObjectPtr<UAnimSequenceBase>* SequencePtr = GET_INSTANCE_ANIM_NODE_DATA_PTR(TObjectPtr<UAnimSequenceBase>, Sequence))
	{
		*SequencePtr = InSequence;
		return true;
	}

	return false;
}

bool FGBWAnimNode_SequencePlayer::SetLoopAnimation(bool bInLoopAnimation)
{
#if WITH_EDITORONLY_DATA
	bLoopAnimation = bInLoopAnimation;
#endif
	
	if(bool* bLoopAnimationPtr = GET_INSTANCE_ANIM_NODE_DATA_PTR(bool, bLoopAnimation))
	{
		*bLoopAnimationPtr = bInLoopAnimation;
		return true;
	}

	return false;
}

UAnimSequenceBase* FGBWAnimNode_SequencePlayer::GetSequence() const
{
	return GET_ANIM_NODE_DATA(TObjectPtr<UAnimSequenceBase>, Sequence);
}

float FGBWAnimNode_SequencePlayer::GetPlayRateBasis() const
{
	return GET_ANIM_NODE_DATA(float, PlayRateBasis);
}

float FGBWAnimNode_SequencePlayer::GetPlayRate() const
{
	return GET_ANIM_NODE_DATA(float, PlayRate);
}

const FInputScaleBiasClampConstants& FGBWAnimNode_SequencePlayer::GetPlayRateScaleBiasClampConstants() const
{
	return GET_ANIM_NODE_DATA(FInputScaleBiasClampConstants, PlayRateScaleBiasClampConstants);
}

float FGBWAnimNode_SequencePlayer::GetStartPosition() const
{
	return GET_ANIM_NODE_DATA(float, StartPosition);
}

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 2
bool FGBWAnimNode_SequencePlayer::GetLoopAnimation() const
{
	return GET_ANIM_NODE_DATA(bool, bLoopAnimation);
}
#endif

bool FGBWAnimNode_SequencePlayer::GetStartFromMatchingPose() const
{
	return GET_ANIM_NODE_DATA(bool, bStartFromMatchingPose);
}

FName FGBWAnimNode_SequencePlayer::GetGroupName() const
{
	return GET_ANIM_NODE_DATA(FName, GroupName);
}

EAnimGroupRole::Type FGBWAnimNode_SequencePlayer::GetGroupRole() const
{
	return GET_ANIM_NODE_DATA(TEnumAsByte<EAnimGroupRole::Type>, GroupRole);
}

EAnimSyncMethod FGBWAnimNode_SequencePlayer::GetGroupMethod() const
{
	return GET_ANIM_NODE_DATA(EAnimSyncMethod, Method);
}

bool FGBWAnimNode_SequencePlayer::GetIgnoreForRelevancyTest() const
{
	return GET_ANIM_NODE_DATA(bool, bIgnoreForRelevancyTest);
}

bool FGBWAnimNode_SequencePlayer::SetGroupName(FName InGroupName)
{
#if WITH_EDITORONLY_DATA
	GroupName = InGroupName;
#endif

	if(FName* GroupNamePtr = GET_INSTANCE_ANIM_NODE_DATA_PTR(FName, GroupName))
	{
		*GroupNamePtr = InGroupName;
		return true;
	}

	return false;
}

bool FGBWAnimNode_SequencePlayer::SetGroupRole(EAnimGroupRole::Type InRole)
{
#if WITH_EDITORONLY_DATA
	GroupRole = InRole;
#endif

	if(TEnumAsByte<EAnimGroupRole::Type>* GroupRolePtr = GET_INSTANCE_ANIM_NODE_DATA_PTR(TEnumAsByte<EAnimGroupRole::Type>, GroupRole))
	{
		*GroupRolePtr = InRole;
		return true;
	}

	return false;
}

bool FGBWAnimNode_SequencePlayer::SetGroupMethod(EAnimSyncMethod InMethod)
{
#if WITH_EDITORONLY_DATA
	Method = InMethod;
#endif

	if(EAnimSyncMethod* MethodPtr = GET_INSTANCE_ANIM_NODE_DATA_PTR(EAnimSyncMethod, Method))
	{
		*MethodPtr = InMethod;
		return true;
	}

	return false;
}

bool FGBWAnimNode_SequencePlayer::SetIgnoreForRelevancyTest(bool bInIgnoreForRelevancyTest)
{
#if WITH_EDITORONLY_DATA
	bIgnoreForRelevancyTest = bInIgnoreForRelevancyTest;
#endif

	if(bool* bIgnoreForRelevancyTestPtr = GET_INSTANCE_ANIM_NODE_DATA_PTR(bool, bIgnoreForRelevancyTest))
	{
		*bIgnoreForRelevancyTestPtr = bInIgnoreForRelevancyTest;
		return true;
	}

	return false;
}

bool FGBWAnimNode_SequencePlayer::SetStartPosition(float InStartPosition)
{
	if(float* StartPositionPtr = GET_INSTANCE_ANIM_NODE_DATA_PTR(float, StartPosition))
	{
		*StartPositionPtr = InStartPosition;
		return true;
	}

	return false;
}


bool FGBWAnimNode_SequencePlayer::SetPlayRate(float InPlayRate)
{
	if(float* PlayRatePtr = GET_INSTANCE_ANIM_NODE_DATA_PTR(float, PlayRate))
	{
		*PlayRatePtr = InPlayRate;
		return true;
	}

	return false;
}

#undef LOCTEXT_NAMESPACE