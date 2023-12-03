// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "AnimNodes/GBWAnimNode_Effect.h"
#include "Animation/AnimNode_Inertialization.h"
#include "Kismet2/CompilerResultsLog.h"

#define LOCTEXT_NAMESPACE "GBWEffectAnimationGraphNode"

UGBWAnimNodeEffect::UGBWAnimNodeEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGBWAnimNodeEffect::ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog)
{
	Super::ValidateAnimNodeDuringCompilation(ForSkeleton, MessageLog);
}

FText UGBWAnimNodeEffect::GetControllerDescription() const
{
	return LOCTEXT("GBW_Effect", "GBW_Effect");
}

FText UGBWAnimNodeEffect::GetTooltipText() const
{
	return LOCTEXT("GBW_Effect_Tooltip", "GBW_Effect");
}

FText UGBWAnimNodeEffect::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("GBW_Effect_Tooltip", "GBW_Effect");
}
#undef LOCTEXT_NAMESPACE
