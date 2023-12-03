// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "GBWAnimNodeStruct_Effect.generated.h"
class UGBWPowerfulEffectComponent;

USTRUCT(BlueprintType)
struct FGBWEffect_PostureAdjustData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	FName PostureRootBoneName;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	FVector TargetPoint;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	FVector2D RollAdjustRange;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	FVector2D YawAdjustRange;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
    float BaseRollAdjust;
    
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
    float BaseYawAdjust;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	float AdjustRate;
	
	FGBWEffect_PostureAdjustData():
	PostureRootBoneName(NAME_None),
	TargetPoint(FVector::Zero()),
	RollAdjustRange(FVector2D(-70.0f,70.0f)),
	YawAdjustRange(FVector2D(-70.0f,70.0f)),
	BaseRollAdjust(0.0f),
	BaseYawAdjust(0.0f),
	AdjustRate(1.0f)
	{
	}
};

USTRUCT(BlueprintType)
struct FGBWEffect_BodyShakeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	FName BoneName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	float Degree;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	float Duration;
	
	FGBWEffect_BodyShakeData():
	BoneName(NAME_None),
	Degree(2.0f),
	Duration(0.2f)
	{
	}
};

USTRUCT(BlueprintType)
struct FGBWEffect_BodyTwistData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	FName BoneName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	FRotator Rotation;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	float InTime;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	float HoldTime;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	float OutTime;
	
	FGBWEffect_BodyTwistData():
	BoneName(NAME_None),
	Rotation(FRotator::ZeroRotator),
	InTime(0.1f),
	HoldTime(0.2f),
	OutTime(0.1f)
	{
	}

	float GetTotalTime()
	{
		return InTime+HoldTime+OutTime;
	}

	bool isIn(float time)
	{
		return time<InTime;
	}
	bool isHold(float time)
	{
		return time>=InTime && time<InTime+HoldTime;
	}
	bool isOut(float time)
	{
		return time>=InTime+HoldTime && time<GetTotalTime();
	}
};

USTRUCT(BlueprintInternalUseOnly)
struct GBWPOWERFULEFFECT_API FGBWAnimNode_Effect : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()
	
	FGBWAnimNode_Effect();

	// FAnimNode_Base interface
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	// End of FAnimNode_Base interface

	// FAnimNode_SkeletalControlBase interface
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override
	{
		DeltaTime = Context.GetDeltaTime();
	};
	// End of FAnimNode_SkeletalControlBase interface

	private:
	// FAnimNode_SkeletalControlBase interface
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

	bool CheckOwner(FComponentSpacePoseContext Context);
	bool CheckValid(FComponentSpacePoseContext Context);

	float DeltaTime = 0.0f;

	UPROPERTY()
	AActor* OwnerActor = nullptr;
	UPROPERTY()
	UGBWPowerfulEffectComponent* EffectComponent = nullptr;

	//For PostureAdjust
	bool PostureAdjustActiveCache = false;
	FGBWEffect_PostureAdjustData PostureAdjustDataCache = FGBWEffect_PostureAdjustData();
	FRotator BaseRotationCache = FRotator::ZeroRotator;
	FRotator TargetRotationCache = FRotator::ZeroRotator;
	float interpSpeedCache = 0.0f;
	float interpSpeedMax = 10.0f;
	float interpSpeedSpeed = 4.0f;
	
	//For BodyShake
	float BodyShakeTimer = 0.0f;
	//For BodyTwist
	float BodyTwistTimer = 0.0f;
};