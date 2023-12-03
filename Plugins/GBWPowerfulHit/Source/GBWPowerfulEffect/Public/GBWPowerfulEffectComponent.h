// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimNodes/GBWAnimNodeStruct_Effect.h"
#include "GBWPowerfulEffectComponent.generated.h"

class AGBWEffectActorBase;

UENUM(BlueprintType)
enum EGBWEffectRepelType
{
	None,
	RemoveExistingSameClass,
	BlockedByExistingSameClass,
	RemoveExistingSpecifiedClass,
	BlockedByExistingSpecifiedClass
};
USTRUCT(BlueprintType, DisplayName="SpawnRules")
struct FGBWSpawnRules
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	TEnumAsByte<EGBWEffectRepelType> RepelType = EGBWEffectRepelType::RemoveExistingSameClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	TArray<TSubclassOf<AGBWEffectActorBase>> SpecifiedClassSet = {};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	FTransform Transform = FTransform();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	bool bUseOwnerTransform = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	bool bAttachToOwner = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	TEnumAsByte<EAttachmentRule> LocationAttachmentRule = EAttachmentRule::KeepRelative;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	TEnumAsByte<EAttachmentRule> RotationAttachmentRule = EAttachmentRule::KeepRelative;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	TEnumAsByte<EAttachmentRule> ScaleAttachmentRule = EAttachmentRule::KeepRelative;

	FGBWSpawnRules(){}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBWPOWERFULEFFECT_API UGBWPowerfulEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGBWPowerfulEffectComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadOnly, Replicated, Category="GBW|Effect")
	bool bIsEffectCompActive = true;
	UPROPERTY(BlueprintReadOnly, Replicated, Category="GBW|Effect")
	TArray<AGBWEffectActorBase*> Effects;
	UPROPERTY(BlueprintReadOnly, Category="GBW|Effect")
	TArray<AGBWEffectActorBase*> Effects_Local;

	UFUNCTION(BlueprintCallable,Category = "GBW|Effect")
	void SetEffectCompActive(bool bActive = true);
	UFUNCTION(Reliable, Server, Category = "GBW|Effect")
	void SetEffectCompActive_Server(bool bActive = true);
	void SetEffectCompActive_Imp(bool bActive = true);
	
	void ExecuteEffect_WithStructParamStr(
		FGuid& EffectGuid,
		TSubclassOf<AGBWEffectActorBase> EffectClass,
		const FGBWSpawnRules& EffectSpawnSettings = FGBWSpawnRules(),
		const FString& EffectParamsStr = FString(),
		bool bIsReplicated = true,
		bool bOnlyTriggerFromServer = false);
	UFUNCTION(BlueprintCallable,Category = "GBW|Effect")
	void ExecuteEffect(
		FGuid& EffectGuid,
		TSubclassOf<AGBWEffectActorBase> EffectClass,
		FGBWSpawnRules EffectSpawnSettings = FGBWSpawnRules(),
		bool bIsReplicated = true,
		bool bOnlyTriggerFromServer = false);
	UFUNCTION(Reliable, Server, Category = "GBW|Effect")
	void ExecuteEffect_Server(
		const FGuid& EffectGuid,
		TSubclassOf<AGBWEffectActorBase> EffectClass,
		FGBWSpawnRules EffectSpawnSettings = FGBWSpawnRules(),
		const FString& EffectParamsStr = FString(),
		bool bIsReplicated = true);
	void ExecuteEffect_Imp(
		const FGuid& EffectGuid,
		TSubclassOf<AGBWEffectActorBase> EffectClass,
		const FGBWSpawnRules& EffectSpawnSettings = FGBWSpawnRules(),
		FString EffectParamsStr = FString(),
		bool bIsReplicated = true);

	void AddEffect(AGBWEffectActorBase* NewEffect, bool bIsReplicated = true);
	UFUNCTION(BlueprintCallable,Category = "GBW|Effect")
	void RemoveEffect(FGuid EffectGuid);
	UFUNCTION(Reliable, Server, Category = "GBW|Effect")
	void RemoveEffect_Server(const FGuid& EffectGuid);
	void RemoveEffect_Imp(const FGuid& EffectGuid);

	UFUNCTION(BlueprintCallable,Category = "GBW|Effect")
	void HasSpecifiedClassEffect(TSubclassOf<AGBWEffectActorBase> EffectClass, bool& bServerHas, bool& bLocalHas);
	UFUNCTION(BlueprintCallable,Category = "GBW|Effect")
	void HasSpecifiedClassesEffect(TArray<TSubclassOf<AGBWEffectActorBase>> EffectClassSet, bool& bServerHas, bool& bLocalHas);
	
	UFUNCTION(BlueprintCallable,Category = "GBW|Effect")
	void RemoveEffectByClass(TSubclassOf<AGBWEffectActorBase> EffectClass);
	UFUNCTION(Reliable, Server, Category = "GBW|Effect")
	void RemoveEffectByClass_Server(TSubclassOf<AGBWEffectActorBase> EffectClass);
	void RemoveEffectByClass_Imp(TSubclassOf<AGBWEffectActorBase> EffectClass);
	
	UFUNCTION(BlueprintCallable,Category = "GBW|Effect")
	void RemoveEffectByClassSet(TArray<TSubclassOf<AGBWEffectActorBase>> EffectClassSet);
	UFUNCTION(Reliable, Server, Category = "GBW|Effect")
	void RemoveEffectByClassSet_Server(const TArray<TSubclassOf<AGBWEffectActorBase>>& EffectClassSet);
	void RemoveEffectByClassSet_Imp(const TArray<TSubclassOf<AGBWEffectActorBase>>& EffectClassSet);


	UPROPERTY(BlueprintReadOnly, Replicated, Category="GBW|Effect|Skeleton")
	bool bIsPostureAdjust;
	UPROPERTY(BlueprintReadOnly, Replicated, Category="GBW|Effect|Skeleton")
	FGBWEffect_PostureAdjustData PostureAdjustData;
	UPROPERTY(BlueprintReadOnly, Replicated, Category="GBW|Effect|Skeleton")
	bool bIsBodyShake;
	UPROPERTY(BlueprintReadOnly, Replicated, Category="GBW|Effect|Skeleton")
	FGBWEffect_BodyShakeData BodyShakeData;
	UPROPERTY(BlueprintReadOnly, Replicated, Category="GBW|Effect|Skeleton")
	bool bIsBodyTwist;
	UPROPERTY(BlueprintReadOnly, Replicated, Category="GBW|Effect|Skeleton")
	FGBWEffect_BodyTwistData BodyTwistData;
	
	bool GetPostureAdjustData(FGBWEffect_PostureAdjustData& OutPostureAdjustData) const;
	bool GetBodyShakeData(bool ClearData, FGBWEffect_BodyShakeData& OutBodyShakeData);
	bool GetBodyTwistData(bool ClearData, FGBWEffect_BodyTwistData& OutBodyTwistData);

	UFUNCTION(BlueprintCallable,Category = "GBW|Effect|Skeleton")
	void ActivePostureAdjust(FGBWEffect_PostureAdjustData InPostureAdjustData);
	UFUNCTION(Reliable, Server, Category = "GBW|Effect|Skeleton")
	void ActivePostureAdjust_Server(const FGBWEffect_PostureAdjustData& InPostureAdjustData);
	void ActivePostureAdjust_Imp(const FGBWEffect_PostureAdjustData& InPostureAdjustData);

	UFUNCTION(BlueprintCallable,Category = "GBW|Effect|Skeleton")
	void UpdatePostureAdjustTargetPoint(FVector TargetPoint);
	UFUNCTION(Reliable, Server, Category = "GBW|Effect|Skeleton")
	void UpdatePostureAdjustTargetPoint_Server(FVector TargetPoint);
	void UpdatePostureAdjustTargetPoint_Imp(const FVector& TargetPoint);
	
	UFUNCTION(BlueprintCallable,Category = "GBW|Effect|Skeleton")
	void DeactivePostureAdjust();
	UFUNCTION(Reliable, Server, Category = "GBW|Effect|Skeleton")
	void DeactivePostureAdjust_Server();
	void DeactivePostureAdjust_Imp();

	UFUNCTION(BlueprintCallable,Category = "GBW|Effect|Skeleton")
	void ActiveBodyShake(FGBWEffect_BodyShakeData InBodyShakeData);
	UFUNCTION(Reliable, Server, Category = "GBW|Effect|Skeleton")
	void ActiveBodyShake_Server(const FGBWEffect_BodyShakeData& InBodyShakeData);
	void ActiveBodyShake_Imp(const FGBWEffect_BodyShakeData& InBodyShakeData);
	
	UFUNCTION(BlueprintCallable,Category = "GBW|Effect|Skeleton")
	void DeactiveBodyShake();
	UFUNCTION(Reliable, Server, Category = "GBW|Effect|Skeleton")
	void DeactiveBodyShake_Server();
	void DeactiveBodyShake_Imp();

	UFUNCTION(BlueprintCallable,Category = "GBW|Effect|Skeleton")
	void ActiveBodyTwist(FGBWEffect_BodyTwistData InBodyTwistData);
	UFUNCTION(Reliable, Server, Category = "GBW|Effect|Skeleton")
	void ActiveBodyTwist_Server(const FGBWEffect_BodyTwistData& InBodyTwistData);
	void ActiveBodyTwist_Imp(const FGBWEffect_BodyTwistData& InBodyTwistData);

	UFUNCTION(BlueprintCallable,Category = "GBW|Effect|Skeleton")
	void DeactiveBodyTwist();
	UFUNCTION(Reliable, Server, Category = "GBW|Effect|Skeleton")
	void DeactiveBodyTwist_Server();
	void DeactiveBodyTwist_Imp();
};