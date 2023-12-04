// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GBWFlowMoveObjects.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GBWFlowMoveANS_MoveControl.generated.h"

UCLASS(Blueprintable, meta = (DisplayName = "FM_MoveControl"))
//ANS 就是 Anim Notify State
//这个通知一般是动画全长都设置了
class GBWFLOWMOVE_API UGBWFlowMoveANS_MoveControl : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "FlowMove")
	FString Description = "FM_MoveControl";

	UPROPERTY(EditAnywhere, Category = "FlowMove")
	FGBWFlowMoveControlSettings MoveControlSettings;
	
	UPROPERTY()
	UAnimSequenceBase * AnimationNow = nullptr;
	float AnimTotalDuration = 0.0f;
	float AnimDuration = 0.0f;
	
	virtual void NotifyBegin(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;
	// Overridden from UAnimNotifyState to provide custom notify name.
	FString GetNotifyName_Implementation() const override;
	virtual FLinearColor GetEditorColor() override {return FColor::Yellow;}

	bool GetAnimPlayState(UAnimSequenceBase*& Animation, float& TotalDuration, float& Duration) const;
};
