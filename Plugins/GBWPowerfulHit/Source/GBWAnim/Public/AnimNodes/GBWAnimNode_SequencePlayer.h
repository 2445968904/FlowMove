// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimNode_AssetPlayerBase.h"
#include "Animation/AnimNode_SequencePlayer.h"
#include "Animation/InputScaleBias.h"
#include "Animation/AnimSequenceBase.h"
#include "Runtime/Launch/Resources/Version.h"
#include "GBWAnimNode_SequencePlayer.generated.h"

USTRUCT(BlueprintType)
struct FGBWAnimSequenceNodeSettings
{
	GENERATED_BODY()

	// The animation sequence asset to play
	UPROPERTY(EditAnywhere, Category = Settings, meta = (PinHiddenByDefault, DisallowedClasses="AnimMontage", FoldProperty))
	TObjectPtr<UAnimSequenceBase> Sequence = nullptr;

	// The Basis in which the PlayRate is expressed in. This is used to rescale PlayRate inputs.
	// For example a Basis of 100 means that the PlayRate input will be divided by 100.
	UPROPERTY(EditAnywhere, Category = Settings, meta = (PinHiddenByDefault, FoldProperty))
	float PlayRateBasis = 1.0f;

	// The play rate multiplier. Can be negative, which will cause the animation to play in reverse.
	UPROPERTY(EditAnywhere, Category = Settings, meta = (PinHiddenByDefault, FoldProperty))
	float PlayRate = 1.0f;
	
	// Additional scaling, offsetting and clamping of PlayRate input.
	// Performed after PlayRateBasis.
	UPROPERTY(EditAnywhere, Category = Settings, meta = (DisplayName="PlayRateScaleBiasClamp", FoldProperty))
	FInputScaleBiasClampConstants PlayRateScaleBiasClampConstants;

	UPROPERTY()
	FInputScaleBiasClamp PlayRateScaleBiasClamp_DEPRECATED;

	// The start position in [0, 1] to use when initializing. When looping, play will still jump back to the beginning when reaching the end.
	UPROPERTY(EditAnywhere, Category = Settings, meta = (PinHiddenByDefault, FoldProperty))
	float StartPosition = 0.0f;

	// Should the animation loop back to the start when it reaches the end?
	UPROPERTY(EditAnywhere, Category = Settings, meta = (PinHiddenByDefault, FoldProperty))
	bool bLoopAnimation = true;
	
	FGBWAnimSequenceNodeSettings(){}
};

// Sequence player node that can be used with constant folding
USTRUCT(BlueprintInternalUseOnly)
struct GBWANIM_API FGBWAnimNode_SequencePlayer : public FAnimNode_SequencePlayerBase
{
	GENERATED_BODY()

protected:
	friend class UGBWAnimGraphNode_SequencePlayer;

#if WITH_EDITORONLY_DATA
	// The group name (NAME_None if it is not part of any group)
	UPROPERTY(EditAnywhere, Category=Sync, meta=(FoldProperty))
	FName GroupName = NAME_None;

	// The role this player can assume within the group (ignored if GroupIndex is INDEX_NONE)
	UPROPERTY(EditAnywhere, Category=Sync, meta=(FoldProperty))
	TEnumAsByte<EAnimGroupRole::Type> GroupRole = EAnimGroupRole::CanBeLeader;

	// How synchronization is determined
	UPROPERTY(EditAnywhere, Category=Sync, meta=(FoldProperty))
	EAnimSyncMethod Method = EAnimSyncMethod::DoNotSync;

	// If true, "Relevant anim" nodes that look for the highest weighted animation in a state will ignore this node
	UPROPERTY(EditAnywhere, Category=Relevancy, meta=(FoldProperty, PinHiddenByDefault))
	bool bIgnoreForRelevancyTest = false;

	/*UPROPERTY(EditAnywhere, Category = Settings, meta = (PinHiddenByDefault, FoldProperty))
	TArray<FGBWAnimSequenceNodeSettings> SequenceList = {};*/
	
	// The animation sequence asset to play
	UPROPERTY(EditAnywhere, Category = Settings, meta = (PinHiddenByDefault, DisallowedClasses="AnimMontage", FoldProperty))
	TObjectPtr<UAnimSequenceBase> Sequence = nullptr;

	// The Basis in which the PlayRate is expressed in. This is used to rescale PlayRate inputs.
	// For example a Basis of 100 means that the PlayRate input will be divided by 100.
	UPROPERTY(EditAnywhere, Category = Settings, meta = (PinHiddenByDefault, FoldProperty))
	float PlayRateBasis = 1.0f;

	// The play rate multiplier. Can be negative, which will cause the animation to play in reverse.
	UPROPERTY(EditAnywhere, Category = Settings, meta = (PinHiddenByDefault, FoldProperty))
	float PlayRate = 1.0f;
	
	// Additional scaling, offsetting and clamping of PlayRate input.
	// Performed after PlayRateBasis.
	UPROPERTY(EditAnywhere, Category = Settings, meta = (DisplayName="PlayRateScaleBiasClamp", FoldProperty))
	FInputScaleBiasClampConstants PlayRateScaleBiasClampConstants;

	UPROPERTY()
	FInputScaleBiasClamp PlayRateScaleBiasClamp_DEPRECATED;

	// The start position in [0, 1] to use when initializing. When looping, play will still jump back to the beginning when reaching the end.
	UPROPERTY(EditAnywhere, Category = Settings, meta = (PinHiddenByDefault, FoldProperty))
	float StartPosition = 0.0f;

	// Should the animation loop back to the start when it reaches the end?
	UPROPERTY(EditAnywhere, Category = Settings, meta = (PinHiddenByDefault, FoldProperty))
	bool bLoopAnimation = true;

	// Use pose matching to choose the start position. Requires experimental PoseSearch plugin.
	UPROPERTY(EditAnywhere, Category = PoseMatching, meta = (PinHiddenByDefault, FoldProperty))
	bool bStartFromMatchingPose = false;
#endif

public:
	// FAnimNode_SequencePlayerBase interface
	virtual bool SetSequence(UAnimSequenceBase* InSequence) override;
	virtual bool SetLoopAnimation(bool bInLoopAnimation) override;
	virtual UAnimSequenceBase* GetSequence() const override;
	virtual float GetPlayRateBasis() const override;
	virtual float GetPlayRate() const override;
	virtual const FInputScaleBiasClampConstants& GetPlayRateScaleBiasClampConstants() const override;
	virtual float GetStartPosition() const override;
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 2
	virtual bool GetLoopAnimation() const override;
#endif
	virtual bool GetStartFromMatchingPose() const override;
	virtual bool SetStartPosition(float InStartPosition) override;
	virtual bool SetPlayRate(float InPlayRate) override;

	// FAnimNode_AssetPlayerBase interface
	virtual FName GetGroupName() const override;
	virtual EAnimGroupRole::Type GetGroupRole() const override;
	virtual EAnimSyncMethod GetGroupMethod() const override;
	virtual bool GetIgnoreForRelevancyTest() const override;
	virtual bool SetGroupName(FName InGroupName) override;
	virtual bool SetGroupRole(EAnimGroupRole::Type InRole) override;
	virtual bool SetGroupMethod(EAnimSyncMethod InMethod) override;
	virtual bool SetIgnoreForRelevancyTest(bool bInIgnoreForRelevancyTest) override;
};