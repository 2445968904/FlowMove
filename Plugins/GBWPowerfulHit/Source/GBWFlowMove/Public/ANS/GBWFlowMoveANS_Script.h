// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GBWFlowMoveANS_Script.generated.h"

UCLASS(Blueprintable, meta = (DisplayName = "FM_Script"))
class GBWFLOWMOVE_API UGBWFlowMoveANS_Script : public UAnimNotifyState
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = "FlowMove")
	FString Description = "FM_Script";

	UPROPERTY(EditAnywhere, Category = "FlowMove")
	TSet<FGameplayTag> ScriptTags;
	
	virtual void NotifyBegin(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;
	// Overridden from UAnimNotifyState to provide custom notify name.
	FString GetNotifyName_Implementation() const override;
	virtual FLinearColor GetEditorColor() override {return FColor::Emerald;}
};
