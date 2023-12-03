// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GBWFlowMoveObjects.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GBWFlowMoveANS_ActionLock.generated.h"

UCLASS(Blueprintable, meta = (DisplayName = "FM_ActionLock"))
class GBWFLOWMOVE_API UGBWFlowMoveANS_ActionLock : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "FlowMove")
	FString Description = "FM_ActionLock";

	UPROPERTY(EditAnywhere, Category = "FlowMove")
	FGBWFlowMoveActionLockStateSettings ActionLockSettings = FGBWFlowMoveActionLockStateSettings();
	
	virtual void NotifyBegin(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;
	// Overridden from UAnimNotifyState to provide custom notify name.
	FString GetNotifyName_Implementation() const override;
	virtual FLinearColor GetEditorColor() override {return FColor::Red;}
};
