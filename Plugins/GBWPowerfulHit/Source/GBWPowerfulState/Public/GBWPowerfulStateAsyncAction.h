// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GBWPowerfulStateComponent.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Tickable.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GBWPowerfulStateAsyncAction.generated.h"

UCLASS(meta = (HideThen = true))
class GBWPOWERFULSTATE_API UGBWPSAsync_ListenEvent : public UBlueprintAsyncActionBase, public FTickableGameObject
{
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FGBWPSTaskDlg,
		UGBWPSAsync_ListenEvent*, Task,
		float, Time,
		float, DeltaTime,
		FPE_Event, PE_Event,
		FPS_Event, PS_Event);
	
	GENERATED_BODY()
public:

	UGBWPSAsync_ListenEvent();
	~UGBWPSAsync_ListenEvent();

	// FTickableGameObject Begin
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableWhenPaused() const;
	virtual bool IsTickableInEditor() const;
	// FTickableGameObject End

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true",
		WorldContext = "WorldContextObject",
		DisplayName = "PS_ListenEvent",
		ScriptName = "PS_ListenEvent",
		DefaultToSelf="InActor"),
		Category = "GBW|PS|Task")
	static UGBWPSAsync_ListenEvent* PS_ListenEvent(
		AActor* InActor,
		FGameplayTagQuery EventToListen = FGameplayTagQuery(),
		bool bStopWhenEvent = true,
		FGameplayTagQuery StateToListen = FGameplayTagQuery(),
		bool bStopWhenState = true,
		float Timeout = -1.0f);

	UPROPERTY(BlueprintAssignable)
		FGBWPSTaskDlg Then;
	UPROPERTY(BlueprintAssignable)
		FGBWPSTaskDlg Update;
	UPROPERTY(BlueprintAssignable)
		FGBWPSTaskDlg OnEvent;
	UPROPERTY(BlueprintAssignable)
		FGBWPSTaskDlg OnState;
	UPROPERTY(BlueprintAssignable)
		FGBWPSTaskDlg Completed;

	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Task")
		void Stop();

private:

	virtual void Activate() override;
	UFUNCTION()
		void CompletedEvent();
	UFUNCTION()
		void OnPEEvent(FGameplayTag EventKey, FPE_Event PE_Event);
	UFUNCTION()
		void OnPSEvent(FGameplayTag StateKey, FPS_Event PS_Event);
	FTimerHandle SetTimerDelegate(FTimerDynamicDelegate Delegate, float Time);
	
	void PreGarbageCollect();
	// The last frame number we were ticked.
	// We don't want to tick multiple times per frame 
	uint32 LastFrameNumberWeTicked = INDEX_NONE;

	UPROPERTY()
	UWorld* World;
	UPROPERTY()
	UObject* WorldContextObject;
	FTimerHandle TimerHandle;
	float Timer = 0.0f;
	float FrameDeltaTime = 0.0f;
	bool bIsCompleted = false;
	
	UPROPERTY()
	UGBWPowerfulStateComponent* PSC = nullptr;
	FGameplayTagQuery TheEventToListen = FGameplayTagQuery::EmptyQuery;
	FGameplayTagQuery TheStateToListen = FGameplayTagQuery::EmptyQuery;
	bool bStopWhenEvent = true;
	bool bStopWhenState = true;
};
