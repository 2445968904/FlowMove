// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GBWPowerfulAnimInstance.generated.h"

UENUM(BlueprintType)
enum EGBWAnimNodeBlendState
{
	NoBlend,
	StateBlendIn,
	StateBlendOut,
	StateReBlendIn
};

UCLASS()
class GBWANIM_API UGBWPowerfulAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="States", meta = (DisplayName="State Blend State", BlueprintInternalUseOnly = "true", AnimGetter="true", BlueprintThreadSafe))
	EGBWAnimNodeBlendState GetStateBlendState(int32 MachineIndex, int32 StateIndex) const;

	UFUNCTION(BlueprintCallable, Category = "GBW|Anim", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	void AnimSM_FollowServer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
};
