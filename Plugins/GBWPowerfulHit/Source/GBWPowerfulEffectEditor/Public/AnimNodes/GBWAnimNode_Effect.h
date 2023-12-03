// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimNodes/GBWAnimNodeStruct_Effect.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "GBWAnimNode_Effect.generated.h"

class FCompilerResultsLog;

UCLASS()
class GBWPOWERFULEFFECTEDITOR_API UGBWAnimNodeEffect : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()

	UGBWAnimNodeEffect(const FObjectInitializer& ObjectInitializer);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FGBWAnimNode_Effect Node;

public:
	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	// End of UEdGraphNode interface

	protected:
	// UAnimGraphNode_Base interface
	virtual void ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog) override;
	// End of UAnimGraphNode_Base interface

	// UAnimGraphNode_SkeletalControlBase interface
	virtual FText GetControllerDescription() const override;
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }
	// End of UAnimGraphNode_SkeletalControlBase interface

	private:
	/** Constructing FText strings can be costly, so we cache the node's title */
	FNodeTitleTextTable CachedNodeTitles;
};
