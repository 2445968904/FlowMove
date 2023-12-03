// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GBWPowerfulAnimInstance.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "GBWAnimComponent.generated.h"

UENUM(BlueprintType)
enum EGBWAnimAssetType
{
	None,
	AnimSequence,
	AnimBlendSpace,
	AnimMontage
};
UENUM(BlueprintType)
enum EGBWAnimPlayStateType
{
	Active,
	Deactivated,
	BlendIn,
	BlendOut
};
USTRUCT(BlueprintType)
struct FGBWAnimPlayState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	UAnimInstance* Instance = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TEnumAsByte<EGBWAnimAssetType> AssetType = EGBWAnimAssetType::None;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	UAnimationAsset* AnimAsset = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	int32 AnimNodeIndex = -1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TEnumAsByte<EGBWAnimPlayStateType> PlayState = EGBWAnimPlayStateType::Deactivated;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool bIsReplay = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float BlendInTime = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float BlendOutTime = 0.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float AnimPlayLength = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float AnimPlayDuration = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float AnimAllPlayLength = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float AnimPlayPosition = 0.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float AnimBlendWeight = 0.0f;
	
	FGBWAnimPlayState(){}
	FGBWAnimPlayState(
		UAnimInstance* InInstance,
		EGBWAnimAssetType InAssetType,
		UAnimationAsset* InAnimAsset,
		int32 InAnimNodeIndex,
		float InBlendInTime,
		float InBlendOutTime,
		float InAnimPlayLength,
		float InAnimPlayDuration,
		float InAnimAllPlayLength,
		float InAnimPlayPosition,
		float InAnimBlendWeight
		)
	{
		Instance = InInstance;
		AssetType = InAssetType;
		AnimAsset = InAnimAsset;
		AnimNodeIndex = InAnimNodeIndex;
		BlendInTime = InBlendInTime;
		BlendOutTime = InBlendOutTime;
		AnimPlayLength = InAnimPlayLength;
		AnimPlayDuration = InAnimPlayDuration;
		AnimAllPlayLength = InAnimAllPlayLength;
		AnimPlayPosition = InAnimPlayPosition;
		AnimBlendWeight = InAnimBlendWeight;
	}
};
USTRUCT(BlueprintType)
struct FGBWAnimCurveValue
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FName CurveName = FName();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float Value = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float DeltaValue = 0.0f;

	FGBWAnimCurveValue(){}
	FGBWAnimCurveValue(FName InCurveName, float InValue, float InDeltaValue)
	{
		CurveName = InCurveName;
		Value = InValue;
		DeltaValue = InDeltaValue;
	}
};
USTRUCT(BlueprintType)
struct FGBWAnimStateMachineNodeInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	UAnimInstance* Instance = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	int32 NodeIndex = -1;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FName CurrentStateName = FName();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float CurrentStateWeight = 0.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool bIsActive = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float ElapsedTime = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FName LastStateName = FName();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float LastStateElapsedTime = 0.0f;
	
	FGBWAnimStateMachineNodeInfo(){}
	FGBWAnimStateMachineNodeInfo(
		UAnimInstance* InInstance,
		int32 InNodeIndex,
		FName InCurrentStateName,
		FName InLastStateName,
		float InCurrentStateWeight,
		bool InIsActive,
		float InElapsedTime,
		float InLastStateElapsedTime)
	{
		Instance = InInstance;
		NodeIndex = InNodeIndex;
		CurrentStateName = InCurrentStateName;
		LastStateName = InLastStateName;
		CurrentStateWeight = InCurrentStateWeight;
		bIsActive = InIsActive;
		ElapsedTime = InElapsedTime;
		LastStateElapsedTime = InLastStateElapsedTime;
	}
};
USTRUCT(BlueprintType)
struct FGBWAnimPlayedCache
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	int MaxAnimSequencePlayedCacheCount = 20;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	UAnimMontage* LastMontage = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TArray<UAnimSequence*> AnimSequencePlayed = {};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	int AnimSequencePlayedCacheIndexNow = -1;

	FGBWAnimPlayedCache(){}
	void AddAnimSequenceCache(UAnimSequence* InAnimSequence);
	bool GetLastAnimSequenceFromArr(TArray<UAnimSequence*> AnimArr, UAnimSequence*& LastAnim, int& LastAnimIndex);
	UAnimSequence* GetAnimSequenceNow();
	UAnimSequence* GetPreviousAnimSequence(int Offset = 0);
	FString GetAnimSequencePlayedCacheDebugString(int CacheCount = -1);
};
USTRUCT(BlueprintType)
struct FGBWAnimPlayStateSet
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	int FrameCount = -1;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	UAnimInstance* Instance = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TArray<UAnimInstance*> LinkedInstances = {};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWAnimPlayState MainAnimPlayState = FGBWAnimPlayState();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TArray<FGBWAnimPlayState> AnimPlayStateSet = {};
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool bIsBlendFinished = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TArray<UAnimNotifyState*> NotifyStateSet = {};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TArray<FGBWAnimCurveValue> CurveValueState = {};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TArray<FGBWAnimStateMachineNodeInfo> StateMachineState = {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWAnimPlayedCache AnimPlayedCache = FGBWAnimPlayedCache();

	FGBWAnimPlayStateSet(){}
	FGBWAnimPlayStateSet(
		UAnimInstance* InInstance,
		const TArray<UAnimInstance*>& InLinkedInstances,
		const FGBWAnimPlayState& InMainAnimPlayState,
		const TArray<FGBWAnimPlayState>& InAnimPlayStateSet,
		const TArray<FGBWAnimStateMachineNodeInfo>& InStateMachineState,
		bool InIsBlendFinished
		)
	{
		Instance = InInstance;
		LinkedInstances = InLinkedInstances;
		MainAnimPlayState = InMainAnimPlayState;
		AnimPlayStateSet = InAnimPlayStateSet;
		StateMachineState = InStateMachineState;
		bIsBlendFinished = InIsBlendFinished;
	}
	FGBWAnimPlayStateSet(
		UAnimInstance* InInstance,
		const TArray<UAnimInstance*>& InLinkedInstances,
		const FGBWAnimPlayState& InMainAnimPlayState,
		const TArray<FGBWAnimPlayState>& InAnimPlayStateSet,
		bool InIsBlendFinished,
		const TArray<UAnimNotifyState*>& InNotifyStateSet,
		const TArray<FGBWAnimCurveValue>& InCurveValueState,
		const TArray<FGBWAnimStateMachineNodeInfo>& InStateMachineState
		)
	{
		Instance = InInstance;
		LinkedInstances = InLinkedInstances;
		MainAnimPlayState = InMainAnimPlayState;
		AnimPlayStateSet = InAnimPlayStateSet;
		bIsBlendFinished = InIsBlendFinished;
		NotifyStateSet = InNotifyStateSet;
		CurveValueState = InCurveValueState;
		StateMachineState = InStateMachineState;
	}

	bool IsValid() const;
};

USTRUCT(BlueprintType, meta=(DisplayName="GBWMontage"))
struct FGBWGBWMontage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	UAnimMontage* MontageToPlay = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float PlayRate=1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float StartingPosition=0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FName StartingSection=NAME_None;
};
USTRUCT(BlueprintType, meta=(DisplayName="GBWConditionMontage"))
struct FGBWConditionMontage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool Condition = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWGBWMontage MontageSetting = FGBWGBWMontage();

	bool CanPlay() const;
};

USTRUCT(BlueprintType, meta=(DisplayName="GBWConditionMontage"))
struct FGBWAnimStateGetType
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool IncludeMainAnimInstance = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool IncludeLinkedAnimInstance = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings",
		meta=(EditCondition="IncludeLinkedAnimInstance"))
	TArray<TSubclassOf<UAnimInstance>> IncludeLinkedAnimInstanceClassSet = {UGBWPowerfulAnimInstance::StaticClass()};
	/*UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings",
		meta=(EditCondition="IncludeMainAnimInstance"))
	TSet<FName> IncludeLinkedAnimInstanceTagSet = {};*/

	FGBWAnimStateGetType(){}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBWANIM_API UGBWAnimComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGBWAnimComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
private:
	int TickCount = 0;
	
public:
	UPROPERTY(Replicated)
	ACharacter* CharacterOwner = nullptr;
	UPROPERTY(BlueprintReadOnly, Replicated, Category="AnimState")
	FGBWAnimPlayStateSet AnimPlayState = FGBWAnimPlayStateSet();
	UPROPERTY(BlueprintReadOnly, Category="AnimState")
	FGBWAnimPlayStateSet AnimPlayState_Local = FGBWAnimPlayStateSet();
	UPROPERTY(BlueprintReadOnly, Replicated, Category="AnimState")
	FGBWAnimStateGetType AnimStateGetType = FGBWAnimStateGetType();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateAnimPlayState(float DeltaTime);

	ACharacter* GetCharacterOwner();
	bool IsLocalOwn() const;
	bool IsInServer();

	UFUNCTION(BlueprintCallable, Category = "GBW|Anim")
	bool IsReadyToUse();

	UFUNCTION(Reliable, Server, Category = "GBW|Anim")
	void SetAnimPlayState_Server(FGBWAnimPlayStateSet NewAnimPlayState);
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|Anim")
	void SetAnimPlayState_Multicast(FGBWAnimPlayStateSet NewAnimPlayState);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|Anim")
	bool GetAnimPlayState(FGBWAnimPlayStateSet& AnimPlayStateSet);
	UFUNCTION(BlueprintCallable, Category = "GBW|Anim")
	bool GetAnimPlayState_Local(FGBWAnimPlayStateSet& AnimPlayStateSet);

	UFUNCTION(BlueprintCallable,Category = "GBW|Anim")
	void SetIncludeMainInstance(bool bInclude = false);
	UFUNCTION(Reliable, Server, Category = "GBW|Anim")
	void SetIncludeMainInstance_Server(bool bInclude = false);

	UFUNCTION(BlueprintCallable,Category = "GBW|Anim")
	void SetIncludeLinkedInstance(bool bInclude = false);
	UFUNCTION(Reliable, Server, Category = "GBW|Anim")
	void SetIncludeLinkedInstance_Server(bool bInclude = false);

	UFUNCTION(BlueprintCallable,Category = "GBW|Anim")
	void AddIncludeLinkedInstanceClass(TArray<TSubclassOf<UAnimInstance>> IncludeLinkedAnimInstanceClassSet);
	UFUNCTION(Reliable, Server, Category = "GBW|Anim")
	void AddIncludeLinkedInstanceClass_Server(const TArray<TSubclassOf<UAnimInstance>>& IncludeLinkedAnimInstanceClassSet);

	UFUNCTION(BlueprintCallable,Category = "GBW|Anim")
	void PlayMontage(USkeletalMeshComponent* SKMComponent,
		UAnimMontage* MontageToPlay,
		float PlayRate=1.0f,
		float StartingPosition=0.0f,
		FName StartingSection=NAME_None);
	UFUNCTION(Reliable, Server, Category = "GBW|Anim")
	void PlayMontage_Server(USkeletalMeshComponent* SKMComponent,
		UAnimMontage* MontageToPlay,
		float PlayRate=1.0f,
		float StartingPosition=0.0f,
		FName StartingSection=NAME_None);
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|Anim")
	void PlayMontage_Multicast(USkeletalMeshComponent* SKMComponent,
		UAnimMontage* MontageToPlay,
		float PlayRate=1.0f,
		float StartingPosition=0.0f,
		FName StartingSection=NAME_None);
	void PlayMontage_Imp(const USkeletalMeshComponent* SKMComponent,
	                            UAnimMontage* MontageToPlay,
	                            float PlayRate=1.0f,
	                            float StartingPosition=0.0f,
	                            FName StartingSection=NAME_None);

	UFUNCTION(BlueprintCallable,Category = "GBW|Anim")
	void StopMontage(USkeletalMeshComponent* SKMComponent);
	UFUNCTION(Reliable, Server, Category = "GBW|Anim")
	void StopMontage_Server(USkeletalMeshComponent* SKMComponent);
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|Anim")
	void StopMontage_Multicast(USkeletalMeshComponent* SKMComponent);
	void StopMontage_Imp(const USkeletalMeshComponent* SKMComponent);

	UFUNCTION(BlueprintCallable,Category = "GBW|Anim")
	UAnimMontage* GetNextMontageFromList(
		TArray<UAnimMontage*> MontageList);

	UFUNCTION(BlueprintCallable,Category = "GBW|Anim")
	UAnimSequence* GetNextAnimSequenceFromList(
		bool bHoldNowWhenAnimActive,
		bool bHoldNowWhenAnimIsPreAnim,
		TArray<UAnimSequence*> AnimSequenceList);

	EGBWAnimPlayStateType GetAnimPlayStateByNodeIndex(int32 NodeIndex, bool& bIsReplay);

	bool IsAnimStateIsConsistentWithServer() const;
	bool GetStateMachineInfoByNodeIndex(int32 NodeIndex, FGBWAnimStateMachineNodeInfo& SMInfo) const;
};
