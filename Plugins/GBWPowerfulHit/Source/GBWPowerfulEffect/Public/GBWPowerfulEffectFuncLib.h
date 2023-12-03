// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GBWPowerfulEffectComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GBWPowerfulEffectFuncLib.generated.h"

UCLASS()
class GBWPOWERFULEFFECT_API UGBWPowerfulEffectFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "GBW|Effect", meta=(DefaultToSelf="InActor"))
	static UGBWPowerfulEffectComponent* GetGBWEffectComponent(AActor* InActor, bool TryAddComponent=true);

	UFUNCTION(BlueprintCallable, Category = "GBW|Effect", meta=(DefaultToSelf="InActor"))
	static void SetEffectCompActive(AActor* InActor, bool bActive = true);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|Effect",
		CustomThunk,
		meta = (DefaultToSelf="InActor", CustomStructureParam = "EffectStructParam"))
	static void ExecuteEffect_Pro(
		AActor* InActor,
		FGuid& EffectGuid,
		TSubclassOf<AGBWEffectActorBase> EffectClass,
		FGBWSpawnRules EffectSpawnSettings,
		const int32& EffectStructParam,
		bool bIsReplicated = true,
		bool bOnlyTriggerFromServer = false){}
	DECLARE_FUNCTION(execExecuteEffect_Pro);
	UFUNCTION(BlueprintCallable, Category = "GBW|Effect",
		meta = (DefaultToSelf="InActor"))
	static void ExecuteEffect(
		AActor* InActor,
		FGuid& EffectGuid,
		TSubclassOf<AGBWEffectActorBase> EffectClass,
		FGBWSpawnRules EffectSpawnSettings,
		bool bIsReplicated = true,
		bool bOnlyTriggerFromServer = false);

	UFUNCTION(BlueprintCallable, Category = "GBW|Effect|Skeleton",
		meta = (DefaultToSelf="InActor"))
	static void ActivePostureAdjust(AActor* InActor, FGBWEffect_PostureAdjustData InPostureAdjustData);
	UFUNCTION(BlueprintCallable, Category = "GBW|Effect|Skeleton",
		meta = (DefaultToSelf="InActor"))
	static void ActiveBodyShake(AActor* InActor, FGBWEffect_BodyShakeData InBodyShakeData);
	UFUNCTION(BlueprintCallable, Category = "GBW|Effect|Skeleton",
		meta = (DefaultToSelf="InActor"))
	static void ActiveBodyTwist(AActor* InActor, FGBWEffect_BodyTwistData InBodyTwistData);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|Effect|Skeleton",
		meta = (DefaultToSelf="InActor"))
	static void UpdatePostureAdjustTargetPoint(AActor* InActor, FVector TargetPoint);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|Effect|Skeleton",
		meta = (DefaultToSelf="InActor"))
	static void DeactivePostureAdjust(AActor* InActor);
	UFUNCTION(BlueprintCallable, Category = "GBW|Effect|Skeleton",
		meta = (DefaultToSelf="InActor"))
	static void DeactiveBodyShake(AActor* InActor);
	UFUNCTION(BlueprintCallable, Category = "GBW|Effect|Skeleton",
		meta = (DefaultToSelf="InActor"))
	static void DeactiveBodyTwist(AActor* InActor);
};
