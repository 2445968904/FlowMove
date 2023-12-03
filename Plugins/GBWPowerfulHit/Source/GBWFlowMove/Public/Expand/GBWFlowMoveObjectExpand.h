// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GBWFlowMoveComponent.h"
#include "GBWFlowMoveObjects.h"
#include "Kismet/KismetMathLibrary.h"
#include "RMS/GBWRMSBPFuncLib.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/UserWidget.h"
#include "GBWFlowMoveObjectExpand.generated.h"

//BEGIN Perceptron
USTRUCT(BlueprintType)
struct FGBWFlowMoveDefaultPerceptionSetting
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception")
	FGBWFlowMoveTraceSetting TraceSetting = FGBWFlowMoveTraceSetting();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 60, ForceUnits = "cm"))
	bool bUseTraceDirection = false;;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 60, ForceUnits = "cm"))
	float PrejudgeDistance = 150.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 60, ForceUnits = "cm"))
	float TraceDetectorHeight = 80.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 100, ForceUnits = "cm"))
	float CrawlerMaxTraceDistance = 300.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float TargetPointMaxDownDistance = 500.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 30, ClampMax = 180, ForceUnits = "°"))
	float DirectiveCoverageAngle = 90.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 0.1))
	float SlopeSensitivity = 3.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 100.0))
	float FloorAndRoofLedgeTraceDistance = 200.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", DisplayName="CrawlerCount / HalfDirectiveCoverageAngle", Meta = (ClampMin = 1))
	int CrawlerCount = 2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 30.0f, ForceUnits = "cm"))
	float BeginLedgeDownForwardTraceDistanceThreshold = 90.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 32))
	int MaxCrawlerStep = 32;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResultSettings", DisplayName="SceneTag_Free")
	FName FreeTag = "Free";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResultSettings", DisplayName="SceneTag_HitWall")
	FName HitWallTag = "HitWall";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResultSettings", DisplayName="SceneTag_Ledge")
	FName LedgeTag = "Ledge";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResultSettings", DisplayName="SceneTag_Hole")
	FName HoleTag = "Hole";

	FGBWFlowMoveDefaultPerceptionSetting(){}
};
UCLASS(Blueprintable, meta = (DisplayName = "_Default Perceptron"))
class GBWFLOWMOVE_API UGBWFlowMovePerceptron_Default : public UGBWFlowMovePerceptron_Base
{
	GENERATED_BODY()
public:
	UGBWFlowMovePerceptron_Default();
	
	virtual bool GetPerceptionResult_Implementation(
		FGBWFlowMoveScene& FlowMoveScene,
		EGBWFlowMoveEventType EventType,
		ACharacter* OwnerCharacter,
		UGBWFlowMoveComponent* FlowMoveComponent) override;

	FGBWFlowMoveScene FlowMoveSceneCache = FGBWFlowMoveScene();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	FGBWFlowMoveDefaultPerceptionSetting ThePerceptionSetting = FGBWFlowMoveDefaultPerceptionSetting();

	bool FindWay(
		FGBWFlowMoveScene& FlowMoveScene,
		ACharacter* Character,
		UGBWFlowMoveComponent* FlowMoveComponent,
		FGBWFlowMoveDefaultPerceptionSetting PerceptionSetting) const;

	static TArray<FVector> GenerateCrawlerDirection(float TraceAngle, FVector TraceVector, int InterpCount = 2);
	bool TestWay(FHitResult& TryHitResult, FHitResult& SuccessHitResult, ACharacter* Character, FVector Start, FVector End, float TraceHeight,FGBWFlowMoveDefaultPerceptionSetting PerceptionSetting) const;
	void FindBottleneck(
		ACharacter* Character,
		FVector Start,
		FVector End,
		FGBWFlowMoveDefaultPerceptionSetting PerceptionSetting,
		TArray<FHitResult>& WayHitResultDown,
		TArray<FHitResult>& WayHitResultUp,
		float& MaxHeightHitResult_High,
		FHitResult& MaxHeightHitResult_HighResult,
		float& MaxHeightHitResult_Low,
		FHitResult& MaxHeightHitResult_LowResult,
		float& MinHeightHitResult_Low,
		FHitResult& MinHeightHitResult_LowResult,
		float& WayMinHeight,
		FHitResult& MinZoneHitResult_DownResult,
		FHitResult& MinZoneHitResult_UpResult
		) const;
	static float GetSlope(const ACharacter* Character, TArray<FHitResult> HitResultArr, bool& bIsWalkable);
	bool TestBarrierFree(ACharacter* Character,const FHitResult TargetHitResultDown, FHitResult& WayHitResult, TArray<FHitResult>& WayHitResultDownArr,float TraceHeight, FGBWFlowMoveDefaultPerceptionSetting PerceptionSetting) const;
};

USTRUCT(BlueprintType)
struct FGBWFlowMoveFindTheBestFootholdPerceptionSetting
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception")
	bool SimpleDebug = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception")
	FGBWFlowMoveTraceSetting TraceSetting = FGBWFlowMoveTraceSetting();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 60, ForceUnits = "cm"))
	float PrejudgeDistance = 400.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 100, ForceUnits = "cm"))
	float FloorAndRoofLedgeTraceDistance = 200.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 100, ForceUnits = "cm"))
	float CrawlerMaxTraceDistance = 400.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float TargetPointMaxDistance = 400.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float TestVisibilityMinDistance = 40.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", DisplayName="CrawlerCount / HalfDirectiveCoverageAngle", Meta = (ClampMin = 1))
	int CrawlerCount = 16;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception")
	bool bIsOnlyGetTargetFromActorWithComponentTag = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResultSettings", DisplayName="DefaultSuccessTag")
	FName SuccessTag = "FindFoothold";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResultSettings", DisplayName="FailedTag")
	FName FailedTag = "NotFindFoothold";

	FGBWFlowMoveFindTheBestFootholdPerceptionSetting();
	static bool GetSceneTagByActor(
		AActor* InActor,
		ACharacter* FromCharacter,
		UGBWFlowMoveComponent* FromFlowMoveComponent,
		FName& SceneTag,
		FVector& TargetPoint);
};
UCLASS(Blueprintable, meta = (DisplayName = "_FindTheBestFoothold"))
class GBWFLOWMOVE_API UGBWFlowMovePerceptron_FindTheBestFoothold : public UGBWFlowMovePerceptron_Base
{
	GENERATED_BODY()
public:
	UGBWFlowMovePerceptron_FindTheBestFoothold();
	
	virtual bool GetPerceptionResult_Implementation(
		FGBWFlowMoveScene& FlowMoveScene,
		EGBWFlowMoveEventType EventType,
		ACharacter* OwnerCharacter,
		UGBWFlowMoveComponent* FlowMoveComponent) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	FGBWFlowMoveFindTheBestFootholdPerceptionSetting ThePerceptionSetting = FGBWFlowMoveFindTheBestFootholdPerceptionSetting();

	bool FindWay(
		FGBWFlowMoveScene& FlowMoveScene,
		ACharacter* Character,
		UGBWFlowMoveComponent* FlowMoveComponent,
		FGBWFlowMoveFindTheBestFootholdPerceptionSetting PerceptionSetting) const;
	bool TryGetBestFootholdSceneTargetFromActor(
		FGBWFlowMoveScene& FlowMoveScene,
		ACharacter* Character,
		FGBWFlowMoveFindTheBestFootholdPerceptionSetting PerceptionSetting,
		UGBWFlowMoveComponent* FlowMoveComponent) const;

	static TArray<FVector> GenerateCrawlerDirection(FVector BeginTraceVector, int InterpCount = 16);
	bool TestWay(FHitResult& TryHitResult,
		ACharacter* Character,
		FVector Start,
		FVector End,
		FGBWFlowMoveFindTheBestFootholdPerceptionSetting PerceptionSetting) const;
	void FindBottleneck(
		ACharacter* Character,
		FVector Start,
		FVector End,
		FGBWFlowMoveFindTheBestFootholdPerceptionSetting PerceptionSetting,
		TArray<FHitResult>& WayHitResultDown,
		TArray<FHitResult>& WayHitResultUp,
		float& MaxHeightHitResult_High,
		FHitResult& MaxHeightHitResult_HighResult,
		float& MaxHeightHitResult_Low,
		FHitResult& MaxHeightHitResult_LowResult,
		float& MinHeightHitResult_Low,
		FHitResult& MinHeightHitResult_LowResult,
		float& WayMinHeight,
		FHitResult& MinZoneHitResult_DownResult,
		FHitResult& MinZoneHitResult_UpResult
		) const;
	bool TryGetBestFootholdTargetFromActor(
		AActor* InActor,
		ACharacter* FromCharacter,
		UGBWFlowMoveComponent* FromFlowMoveComponent,
		FName& SceneTag,
		FVector& TargetPoint) const;
	static bool TestVisibility(
		AActor* StartActor,
		AActor* EndActor,
		float MinDistanceForTest,
		FGBWFlowMoveTraceSetting TraceSetting,
		FVector& TouchPoint);
};

USTRUCT(BlueprintType)
struct FGBWFlowMoveGetTargetPointByProjectilePerceptionSetting
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception")
	TSubclassOf<USceneComponent> LaunchFromComponentClass = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception")
	FName LaunchFromComponentTag = NAME_None;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception")
	FName LaunchFromComponentSocketName = NAME_None;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception")
	FTransform LaunchFromComponentOffset = FTransform();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 0, ForceUnits = "s"))
	float DelayLaunch = 0.35f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float ProjectileRadius = 15.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float ProjectileSpeed = 4500.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float ProjectileMaxFlyDistance = 2000.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception")
	TSubclassOf<AActor> ProjectileActorClass = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception")
	bool GetLastProjectileLocationAsTargetWhenNotHit = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 0, ForceUnits = "°"))
	float TraceAngleThreshold = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	FGBWFlowMoveTraceSetting TraceSetting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResultSettings")
	FName FailedTag = "NoTarget";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResultSettings")
	FName SuccessTag = "GetTarget";

	FGBWFlowMoveGetTargetPointByProjectilePerceptionSetting(){}
};
UCLASS(Blueprintable, meta = (DisplayName = "_Get Target By Projectile"))
class GBWFLOWMOVE_API UGBWFlowMovePerceptron_GetTargetPointByProjectile : public UGBWFlowMovePerceptron_Base
{
	GENERATED_BODY()
public:
	UGBWFlowMovePerceptron_GetTargetPointByProjectile();
	
	virtual bool GetPerceptionResult_Implementation(
		FGBWFlowMoveScene& FlowMoveScene,
		EGBWFlowMoveEventType EventType,
		ACharacter* OwnerCharacter,
		UGBWFlowMoveComponent* FlowMoveComponent) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	FGBWFlowMoveGetTargetPointByProjectilePerceptionSetting ThePerceptionSetting = FGBWFlowMoveGetTargetPointByProjectilePerceptionSetting();
	
	UFUNCTION(BlueprintCallable, Category="GBW|FlowMove")
	bool GetFlowScene(
		FGBWFlowMoveScene& FlowMoveScene,
		EGBWFlowMoveEventType EventType,
		ACharacter* Character = nullptr,
		FGBWFlowMoveGetTargetPointByProjectilePerceptionSetting PerceptionSetting = FGBWFlowMoveGetTargetPointByProjectilePerceptionSetting(),
		FVector InputVector = FVector::ZeroVector,
		FVector ControlVector = FVector::ZeroVector,
		UGBWFlowMoveComponent* FlowMoveComponent = nullptr);

private:
	bool bIsActive = false;
	bool bIsFinished = false;
	FVector LastProjectileLocation = FVector::ZeroVector;
	FVector FlyDirection = FVector::ZeroVector;
	float Timer = 0.0f;
	float FlyDistance = 0.0f;
	FGuid ProjectileActorGUID = FGuid();
	bool bIsProjectileActorInited = false;
	
	void ResetState(UGBWFlowMoveComponent* FlowMoveComponent);

	FVector GetLaunchLocation(const ACharacter* Character) const;

	void UpdateProjectileActor(float DeltaTime, UGBWFlowMoveComponent* FlowMoveComponent);
};

USTRUCT(BlueprintType)
struct FGBWFlowMoveFindTargetPointByActorPerceptionSetting
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Perception")
	TSubclassOf<UUserWidget> SightWidgetClass_NoTarget;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Perception")
	TSubclassOf<UUserWidget> SightWidgetClass_GetTarget;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Perception")
	TSubclassOf<UUserWidget> TargetWidgetClass;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Perception")
	USoundBase* Sound_GetTarget = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float PrejudgeDistance = 2000.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 0, ForceUnits = "°"))
	float TraceAngleThreshold = 180.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Perception", Meta = (ClampMin = 0, ForceUnits = "°"))
	float FuzzyQueryAngleRestrictions = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	FGBWFlowMoveTraceSetting TraceSetting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResultSettings")
	FName FailedTag = "NoTarget";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResultSettings")
	FName SuccessTag = "GetTarget";

	FGBWFlowMoveFindTargetPointByActorPerceptionSetting(){}
};
UCLASS(Blueprintable, meta = (DisplayName = "_Find Target By Actor"))
class GBWFLOWMOVE_API UGBWFlowMovePerceptron_FindTargetPointByActor : public UGBWFlowMovePerceptron_Base
{
	GENERATED_BODY()
public:
	UGBWFlowMovePerceptron_FindTargetPointByActor();
	
	virtual bool GetPerceptionResult_Implementation(
		FGBWFlowMoveScene& FlowMoveScene,
		EGBWFlowMoveEventType EventType,
		ACharacter* OwnerCharacter,
		UGBWFlowMoveComponent* FlowMoveComponent) override;
	virtual void OnUpdate_Implementation(FGameplayTag PerceptionKey, EGBWFlowMoveEventType EventType, UGBWFlowMoveComponent* FlowMoveComponent) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	FGBWFlowMoveFindTargetPointByActorPerceptionSetting ThePerceptionSetting = FGBWFlowMoveFindTargetPointByActorPerceptionSetting();
	
	UFUNCTION(BlueprintCallable, Category="GBW|FlowMove")
	bool GetFlowScene(
		FGBWFlowMoveScene& FlowMoveScene,
		ACharacter* Character,
		FGBWFlowMoveFindTargetPointByActorPerceptionSetting PerceptionSetting,
		UGBWFlowMoveComponent* FlowMoveComponent);

	UFUNCTION(BlueprintCallable, Category="GBW|FlowMove")
	bool TestVisibility(
		AActor* StartActor,
		AActor* EndActor,
		FGBWFlowMoveTraceSetting TraceSetting,
		FVector& TouchPoint);

	UPROPERTY()
	UUserWidget* SightWidget_NoTarget = nullptr;
	UPROPERTY()
	UUserWidget* SightWidget_GetTarget = nullptr;
	UPROPERTY()
	UUserWidget* TargetWidget = nullptr;
	void CheckWidget(FGameplayTag PerceptionKey, EGBWFlowMoveEventType EventType, UGBWFlowMoveComponent* FlowMoveComponent);
	UPROPERTY()
	AActor* TargetActorCache = nullptr;
};
//END Perceptron

// BEGIN GetFocusActor
UCLASS(Blueprintable, meta = (DisplayName = "_GetFocusActor_Default"))
class GBWFLOWMOVE_API UGBWFlowMoveGetFocusActor_GetFocusActor_Default : public UGBWFlowMoveGetFocusActor_Base
{
	GENERATED_BODY()
public:
	UGBWFlowMoveGetFocusActor_GetFocusActor_Default(){};
	
	virtual USceneComponent* OnGetFocusActor_Implementation(
		UGBWFlowMoveComponent* FlowMoveComponent) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	FGBWFlowMoveTraceSetting PerceptionSetting = FGBWFlowMoveTraceSetting();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	float PrejudgeDistance = 2000.0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	TArray<FName> ComponentWithTags;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	float FuzzyQueryAngleRestrictions = 45.0f;
};
//END GetFocusActor

// BEGIN Script
UCLASS(Blueprintable, meta = (DisplayName = "_Debug"))
class GBWFLOWMOVE_API UGBWFlowMoveScript_Debug : public UGBWFlowMoveScript_Base
{
	GENERATED_BODY()
public:
	virtual void OnUpdate_Implementation(
		EGBWFlowMoveEventType EventType,
		EFlowMoveNetworkLocationType NetworkLocation,
		bool bIsLocalOwn,
		UGBWFlowMoveComponent* FlowMoveComponent) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	bool bIsDebug = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	bool FlowMoveState = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	bool FlowMovePerceptron = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	bool FlowMoveScript = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	bool FlowMoveAction = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	bool FlowMoveScene = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings",
		meta=(EditCondition="FlowMoveScene"))
	TArray<FGameplayTag> FlowMoveSceneTags = {};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings",
		meta=(EditCondition="FlowMoveScene"))
	FGameplayTag DrawGraphFlowMoveSceneTag = FGameplayTag();
	
	void FlowMoveDebug(FGBWFlowMoveState TaskState) const;
};

UENUM(BlueprintType)
enum EFlowMoveAdjustCapsuleType
{
	DirectAdjustment,
	AdjustToValidSpace
};
UCLASS(Blueprintable, meta = (DisplayName = "_AdjustCapsule"))
class GBWFLOWMOVE_API UGBWFlowMoveScript_AdjustCapsule : public UGBWFlowMoveScript_Base
{
	GENERATED_BODY()
public:
	virtual void OnUpdate_Implementation(
		EGBWFlowMoveEventType EventType,
		EFlowMoveNetworkLocationType NetworkLocation,
		bool bIsLocalOwn,
		UGBWFlowMoveComponent* FlowMoveComponent) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings",
		meta=(ClampMin = 10.0f, ForceUnits = "cm"))
	float MinHeightCanAdjustTo = 30.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	float InterpSpeed = 12.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	TEnumAsByte<EFlowMoveAdjustCapsuleType> Type = EFlowMoveAdjustCapsuleType::AdjustToValidSpace;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings",
		meta=(EditConditionHides,EditCondition="Type == EFlowMoveAdjustCapsuleType::DirectAdjustment",ClampMin = 10.0f, ForceUnits = "cm"))
	float AdjustTo = 60.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings",
		meta=(EditConditionHides,EditCondition="Type == EFlowMoveAdjustCapsuleType::AdjustToValidSpace", ClampMin = 0, ClampMax = 100, ForceUnits = "%"))
	float AdjustToValidSpaceFraction = 80.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings",
		meta=(EditConditionHides,EditCondition="Type == EFlowMoveAdjustCapsuleType::AdjustToValidSpace"))
	bool ByCurrentLocation = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings",
		meta=(EditConditionHides,EditCondition="Type == EFlowMoveAdjustCapsuleType::AdjustToValidSpace"))
	bool ByTargetLocation = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings",
		meta=(EditConditionHides,EditCondition="Type == EFlowMoveAdjustCapsuleType::AdjustToValidSpace"))
	FGameplayTag DefaultTargetSceneSlot = FGameplayTag();

	float LastTargetHeight = 0.0f;
};
// END Script