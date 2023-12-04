// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "GBWAnimFuncLib.h"
#include "GBWFlowMoveObjects.generated.h"

struct FFlowMoveAttributeModifier;
struct FGBWFlowMoveState;
struct FGBWFlowMoveScene;
class UGBWFlowMoveComponent;
class UGBWFlowMovePerceptron_Base;
class UGBWFlowMoveScript_Base;
class UGBWFlowMoveGetFocusActor_Base;

// BEGIN struct
UENUM(BlueprintType)
enum class EGBWFlowMoveEventType : uint8
{
	Init,
	Update,
	End
};
UENUM(BlueprintType)
enum class EGBWPerceptionResultItemType : uint8
{
	Bool,
	Float,
	Vector,
	Transform,
	String,
	GamePlayTag,
	Actor,
	Object,
	MovementMode
};
UENUM(BlueprintType)
enum EConditionRequireType
{
	RequireMet,
	RequireNotMet,
	NotRequire
};
/*关于这个枚举状态
 * 
 */
UENUM(BlueprintType)
enum EFlowMoveEventType
{
	OnActive,
	OnActiveFailed,
	OnUpdate,
	OnStop,
	OnActionStart,
	OnActionUpdate,
	OnActionEnd,
	OnNoActionCanExecute,
	OnMovementModeChange,
	OnViewModeChange,
	OnFocusActorChange
};
UENUM(BlueprintType)
enum EFlowMoveCharacterViewMode
{
	TP_FreeMode,
	TP_ForwardLockMode,
	TP_ActorLockMode
};
//关于这个结构体
/*
 *
 *
 * 
 */
USTRUCT(BlueprintType)
struct FFlowMoveEvent
{
	GENERATED_BODY()

	UPROPERTY()
	int InFrameNum = -1;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	TEnumAsByte<EFlowMoveEventType> EventType = EFlowMoveEventType::OnActionStart;
	//这个变量表示的是ActionTag 只能从OnActionStart OnActionEnd OnActionUpdate 中选择
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings",
		meta=(EditConditionHides,EditCondition="EventType == EFlowMoveEventType::OnActionStart || EventType == EFlowMoveEventType::OnActionEnd || EventType == EFlowMoveEventType::OnActionUpdate"))
	FGameplayTag ActionTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings",
		meta=(EditConditionHides,EditCondition="EventType == EFlowMoveEventType::OnMovementModeChange"))
	TEnumAsByte<EMovementMode> NewMovementMode = EMovementMode::MOVE_None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings",
		meta=(EditConditionHides,EditCondition="EventType == EFlowMoveEventType::OnViewModeChange"))
	TEnumAsByte<EFlowMoveCharacterViewMode> NewViewMode = EFlowMoveCharacterViewMode::TP_FreeMode;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings",
		meta=(EditConditionHides,EditCondition="EventType == EFlowMoveEventType::OnFocusActorChange"))
	USceneComponent* NewFocusActorComponent = nullptr;
	
	FFlowMoveEvent(){}
	FFlowMoveEvent(EFlowMoveEventType TheEventType);
	FFlowMoveEvent(EFlowMoveEventType TheEventType, FGameplayTag InActionTag);
	FFlowMoveEvent(EMovementMode TheNewMovementMode);
	FFlowMoveEvent(EFlowMoveCharacterViewMode TheNewViewMode);
	FFlowMoveEvent(USceneComponent* TheNewFocusActorComponent);
	bool EqualTo(const FFlowMoveEvent& OtherEvent) const;
	FString GetDebugString() const;
};
USTRUCT(BlueprintType)
struct FFlowMoveInputValueTrigger_PerceptionInput
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Perception",
		meta=(ClampMin=0, ForceUnits = "°"))
	float UpAngleThreshold = 10.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Perception",
		meta=(ClampMin=0, ForceUnits = "°"))
	float DownAngleThreshold = 60.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Perception",
		meta=(ClampMin=0, ForceUnits = "°"))
	float LeftAndRightAngleThreshold = 30.0f;
	
	FFlowMoveInputValueTrigger_PerceptionInput(){}
};

USTRUCT(BlueprintType)
struct FGBWFlowMoveTraceSetting
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug")
	bool isDebug = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug",
		meta=(EditConditionHides,EditCondition="isDebug"))
	float DebugTime = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug",
		meta=(EditConditionHides,EditCondition="isDebug"))
	bool DrawTraceShape = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug",
		meta=(EditConditionHides,EditCondition="isDebug"))
	bool DrawTraceLine = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	TArray<TEnumAsByte<ETraceTypeQuery>> TraceChannels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScreeningConditions")
	bool UseScreeningConditions = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScreeningConditions",
		meta=(EditConditionHides,EditCondition="UseScreeningConditions"))
	TArray<TSubclassOf<AActor>> ActorWithClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScreeningConditions",
		meta=(EditConditionHides,EditCondition="UseScreeningConditions"))
	TArray<FName> ActorWithTag;

	FGBWFlowMoveTraceSetting(){}
	bool IsMetScreeningConditions(const AActor* InActor);
};
USTRUCT(BlueprintType)
struct FGBWPerceptionResultItem
{
	GENERATED_BODY()

	UPROPERTY()
	EGBWPerceptionResultItemType Type = EGBWPerceptionResultItemType::Float;
	
	UPROPERTY()
	bool BValue = true;
	UPROPERTY()
	float FValue = 0.0f;
	UPROPERTY()
	FVector VValue = FVector::ZeroVector;
	UPROPERTY()
	FTransform TValue = FTransform();
	UPROPERTY()
	FString SValue = "";
	UPROPERTY()
	FGameplayTag GTValue = FGameplayTag::EmptyTag;
	UPROPERTY()
	AActor* AValue = nullptr;
	UPROPERTY()
	UObject* OValue = nullptr;
	UPROPERTY()
	TEnumAsByte<EMovementMode> MovementModeValue = EMovementMode::MOVE_None;
	
	bool EqualTo(const FGBWPerceptionResultItem& OtherPerceptionResultItem) const;
	FGBWPerceptionResultItem(){}
	FGBWPerceptionResultItem(bool value){BValue=value;Type = EGBWPerceptionResultItemType::Bool;}
	FGBWPerceptionResultItem(float value){FValue=value;Type = EGBWPerceptionResultItemType::Float;}
	FGBWPerceptionResultItem(FVector value){VValue=value;Type = EGBWPerceptionResultItemType::Vector;}
	FGBWPerceptionResultItem(FTransform value){TValue=value;Type = EGBWPerceptionResultItemType::Transform;}
	FGBWPerceptionResultItem(FString value){SValue=value;Type = EGBWPerceptionResultItemType::String;}
	FGBWPerceptionResultItem(FGameplayTag value){GTValue=value;Type = EGBWPerceptionResultItemType::GamePlayTag;}
	FGBWPerceptionResultItem(AActor* value){AValue=value;Type = EGBWPerceptionResultItemType::Actor;}
	FGBWPerceptionResultItem(UObject* value){OValue=value;Type = EGBWPerceptionResultItemType::Object;}
	FGBWPerceptionResultItem(EMovementMode value){MovementModeValue=value;Type = EGBWPerceptionResultItemType::MovementMode;}
};
USTRUCT(BlueprintType)
struct FGBWPerceptionResult
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FName> Keys;
	UPROPERTY()
	TArray<FGBWPerceptionResultItem> Values;
	
	FGBWPerceptionResult(){}

	bool IsValid() const;

	bool EqualTo(FGBWPerceptionResult OtherPerceptionResult);

	void AddBool(FName Key, bool Value);
	void AddFloat(FName Key, float Value);
	void AddVector(FName Key, FVector Value);
	void AddTransform(FName Key, FTransform Value);
	void AddString(FName Key, FString Value);
	void AddGameplayTag(FName Key, FGameplayTag Value);
	void AddActor(FName Key, AActor* Value);
	void AddObject(FName Key, UObject* Value);
	void AddMovementMode(FName Key, EMovementMode Value);

	void GetBool(FName Key, bool& IsGet, bool& Value) const;
	void GetFloat(FName Key, bool& IsGet, float& Value) const;
	void GetVector(FName Key, bool& IsGet, FVector& Value) const;
	void GetTransform(FName Key, bool& IsGet, FTransform& Value) const;
	void GetString(FName Key, bool& IsGet, FString& Value) const;
	void GetGameplayTag(FName Key, bool& IsGet, FGameplayTag& Value) const;
	void GetActor(FName Key, bool& IsGet, AActor*& Value) const;
	void GetObject(FName Key, bool& IsGet, UObject*& Value) const;
	void GetMovementMode(FName Key, bool& IsGet, TEnumAsByte<EMovementMode>& Value) const;
};

UENUM(BlueprintType)
enum EFlowMoveAnimType
{
	Montage,
	BlendSpace
};
USTRUCT(BlueprintType)
struct FGBWFlowMoveAnimSetting
{
	GENERATED_BODY()

	UPROPERTY()
	bool bIsPlayed = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TEnumAsByte<EFlowMoveAnimType> AnimType = EFlowMoveAnimType::Montage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FName MeshTag = NAME_None;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TArray<FName> ExcludedMeshTag = {};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="AnimType == EFlowMoveAnimType::Montage"))
	UAnimMontage* Montage = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="AnimType == EFlowMoveAnimType::Montage"))
	bool bIsLoop = false;
	/** If the array is empty, random will be attempted in all Montage Sections */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="AnimType == EFlowMoveAnimType::Montage"))
	TArray<FName> RandomSections = {};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="AnimType == EFlowMoveAnimType::BlendSpace"))
	UBlendSpace* AnimBlendSpace = nullptr;

	bool EqualTo(const FGBWFlowMoveAnimSetting& Other) const;
	bool IsAnimValid() const;
	void Reset();
	FString GetDebugStr() const;
};

UCLASS(Blueprintable, meta = (DisplayName = "GBWFlowMoveObject_Base"))
class GBWFLOWMOVE_API UGBWFlowMoveObject_Base : public UObject
{
	GENERATED_BODY()

	virtual bool IsNameStableForNetworking() const override;
	virtual bool IsSupportedForNetworking() const override;
protected:
	struct FWorldContext* WorldContext;
public:
	UGBWFlowMoveObject_Base();

	UPROPERTY()
	FGuid Guid = FGuid();

	// Allows the Object to use BP_Functions
	UFUNCTION(BlueprintCallable, Category="WorldContext")
	void SetWorldContext(UObject* NewWorldContext);

	UFUNCTION(BlueprintCallable, Category = "WorldContext")
	UObject* GetWorldContextObject();

	//~ Begin UObject Interface
	virtual class UWorld* GetWorld() const override final;
	struct FWorldContext* GetWorldContext() const { return WorldContext; };
};
UENUM(BlueprintType)
enum class EGBWFlowMoveDirectionType : uint8
{
	MM UMETA(DisplayName="◈"),
	MU UMETA(DisplayName="↑"),
	MD UMETA(DisplayName="↓"),
	LM UMETA(DisplayName="←"),
	RM UMETA(DisplayName="→"),
	LU UMETA(DisplayName="↖"),
	RU UMETA(DisplayName="↗"),
	LD UMETA(DisplayName="↙"),
	RD UMETA(DisplayName="↘")
};
UENUM(BlueprintType)
enum EGBWFlowMoveScenePointType
{
	TargetPoint,
	ActorPoint,
	LeftSidePoint,
	RightSidePoint,
	FloorPoint,
	RoofPoint,
	FloorForwardLedgePoint,
	FloorBackwardLedgePoint,
	NearestFloorLedgePoint,
	RoofForwardLedgePoint,
	RoofBackwardLedgePoint,
	NearestRoofLedgePoint,
};
USTRUCT(BlueprintType)
struct FGBWFlowMoveSceneChangeInfo
{
	GENERATED_BODY()
	
	bool SceneType = false;
	bool ActorLocationScene = false;
	bool TargetLocationScene = false;
	bool LeftLocationScene = false;
	bool RightLocationScene = false;
	bool TargetActor = false;
	bool Slope = false;
	bool PerceptionResult = false;

	FGBWFlowMoveSceneChangeInfo(){}
};
USTRUCT(BlueprintType)
struct FGBWFlowMoveFloorRoofScene
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FVector Floor = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FVector FloorLedge_Forward = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FVector FloorLedge_Backward = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FVector Roof = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FVector RoofLedge_Forward = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FVector RoofLedge_Backward = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float HeightFromGround = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float HeightFromRoof = 0.0f;

	

	FGBWFlowMoveFloorRoofScene(){}
	bool EqualTo(const FGBWFlowMoveFloorRoofScene& Other) const;
};
USTRUCT(BlueprintType)
struct FGBWFlowMoveBlockWallScene
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FVector WallPoint = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FVector WallLedge_Up = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FVector WallLedge_Down = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FVector WallLedge_Left = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FVector WallLedge_Right = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float DistanceFromActor = 0.0f;

	FGBWFlowMoveBlockWallScene(){}
	bool EqualTo(const FGBWFlowMoveBlockWallScene& Other) const;
	bool IsWall() const;
	bool IsLedge_Up() const;
	bool IsLedge_Down() const;
	bool IsLedge_Left() const;
	bool IsLedge_Right() const;
};
USTRUCT(BlueprintType)
struct FGBWFlowMovePointScene
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FVector Point = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float Height = 0.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float Right = 0.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float Forward = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float Angle = 0.f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float Direction = 0.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	EGBWFlowMoveDirectionType SceneDirectionMark = EGBWFlowMoveDirectionType::MM;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMoveFloorRoofScene FloorAndRoof = FGBWFlowMoveFloorRoofScene();

	FGBWFlowMovePointScene(){}
	FGBWFlowMovePointScene(ACharacter* InCharacter, const FVector& InPoint, bool bFindLedge, float LedgeTraceDistance, FGBWFlowMoveTraceSetting TraceSetting);
	bool EqualTo(const FGBWFlowMovePointScene& Other) const;
};
USTRUCT(BlueprintType)
struct FGBWFlowMoveScene
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FName SceneType = NAME_None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMovePointScene ActorLocationScene = FGBWFlowMovePointScene();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMovePointScene TargetLocationScene = FGBWFlowMovePointScene();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMovePointScene LeftLocationScene = FGBWFlowMovePointScene();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMovePointScene RightLocationScene = FGBWFlowMovePointScene();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	AActor* TargetActor = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float Slope = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWPerceptionResult PerceptionResult = FGBWPerceptionResult();

	FGBWFlowMoveScene(){}
	FGBWFlowMoveScene(ACharacter* InCharacter,
		FName InSceneType,
		AActor* InTargetActor,
		float InSlope,
		const FVector& InTargetPoint,
		const FVector& LeftTargetPoint,
		const FVector& RightTargetPoint,
		const FGBWFlowMoveTraceSetting& TraceSetting,
		float LedgeTraceDistance);

	bool IsValid() const;
	bool EqualTo(const FGBWFlowMoveScene& OtherScene);
	bool CheckSceneChange(
		const FGBWFlowMoveScene& OtherScene,
		FGBWFlowMoveSceneChangeInfo& ChangeInfo);

	bool GetPlaneConstraintSetting(FVector& PlaneNormal, FVector& PlaneOrigin) const;
};
USTRUCT(BlueprintType)
struct FGBWSupplementaryFlowMoveScene
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGameplayTag SceneSlot = FGameplayTag();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMoveScene Scene = FGBWFlowMoveScene();
};

UENUM(BlueprintType)
enum class EGBWFlowMoveValueScopePointType : uint8
{
	Contain UMETA(DisplayName="Contain"),
	NotContain UMETA(DisplayName="NotContain"),
	Unrestricted UMETA(DisplayName="∞")
};
USTRUCT(BlueprintType)
struct FGBWFlowMoveFloatScopePoint
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	EGBWFlowMoveValueScopePointType Type = EGBWFlowMoveValueScopePointType::Unrestricted;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings", meta=(EditConditionHides, EditCondition="Type!=EGBWFlowMoveValueScopePointType::Unrestricted"))
	float Value = 0.0f;

	FGBWFlowMoveFloatScopePoint(){}
};
USTRUCT(BlueprintType)
struct FGBWFlowMoveFloatScope
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMoveFloatScopePoint Min = FGBWFlowMoveFloatScopePoint();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMoveFloatScopePoint Max = FGBWFlowMoveFloatScopePoint();

	FGBWFlowMoveFloatScope(){}
	bool InScope(const float InValue) const;
};
USTRUCT(BlueprintType)
struct FGBWFlowMoveVectorScope
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMoveFloatScope X_Scope = FGBWFlowMoveFloatScope();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMoveFloatScope Y_Scope = FGBWFlowMoveFloatScope();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMoveFloatScope Z_Scope = FGBWFlowMoveFloatScope();

	FGBWFlowMoveVectorScope(){}
	bool InScope(const FVector InValue) const;
};
USTRUCT(BlueprintType)
struct FGBWFlowMoveTransformScope
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMoveVectorScope Location_Scope = FGBWFlowMoveVectorScope();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMoveVectorScope Rotation_Scope = FGBWFlowMoveVectorScope();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMoveVectorScope Scale_Scope = FGBWFlowMoveVectorScope();

	FGBWFlowMoveTransformScope(){}
	bool InScope(const FTransform InValue) const;
};
USTRUCT(BlueprintType)
struct FGBWPerceptionResultItemScope
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Settings")
	EGBWPerceptionResultItemType Type = EGBWPerceptionResultItemType::Float;
	
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly,Category="Settings",meta=(EditConditionHides,EditCondition="Type==EGBWPerceptionResultItemType::Bool"))
	FString BValueScope = "Ture/False";
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Settings",meta=(EditConditionHides,EditCondition="Type==EGBWPerceptionResultItemType::Float"))
	FGBWFlowMoveFloatScope FValueScope = FGBWFlowMoveFloatScope();
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Settings",meta=(EditConditionHides,EditCondition="Type==EGBWPerceptionResultItemType::Vector"))
	FGBWFlowMoveVectorScope VValueScope = FGBWFlowMoveVectorScope();
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Settings",meta=(EditConditionHides,EditCondition="Type==EGBWPerceptionResultItemType::Transform"))
	FGBWFlowMoveTransformScope TValueScope = FGBWFlowMoveTransformScope();
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Settings",meta=(EditConditionHides,EditCondition="Type==EGBWPerceptionResultItemType::String"))
	TSet<FString> SValueScope;
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Settings",meta=(EditConditionHides,EditCondition="Type==EGBWPerceptionResultItemType::GameplayTag"))
	TSet<FGameplayTag> GTValueScope;
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly,Category="Settings",meta=(EditConditionHides,EditCondition="Type==EGBWPerceptionResultItemType::Actor"))
	FString AValueScope = "Currently not supported";
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly,Category="Settings",meta=(EditConditionHides,EditCondition="Type==EGBWPerceptionResultItemType::Object"))
	FString OValueScope = "Currently not supported";
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Settings",meta=(EditConditionHides,EditCondition="Type==EGBWPerceptionResultItemType::MovementMode"))
	TSet<TEnumAsByte<EMovementMode>> MovementModeValueScope = {
		EMovementMode::MOVE_None,
		EMovementMode::MOVE_Walking,
		EMovementMode::MOVE_NavWalking,
		EMovementMode::MOVE_Flying,
		EMovementMode::MOVE_Falling,
		EMovementMode::MOVE_Swimming,
		EMovementMode::MOVE_Custom
	};
	
	FGBWPerceptionResultItemScope(){}
};
USTRUCT(BlueprintType)
struct FGBWPerceptionResultScope
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Settings")
	TMap<FName,FGBWPerceptionResultItemScope> Scope;
	
	FGBWPerceptionResultScope(){}
};
USTRUCT(BlueprintType)
struct FGBWFlowMoveSceneScope
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TSet<FName> SceneTypeScope;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMoveFloatScope SceneHeightScope = FGBWFlowMoveFloatScope();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TSet<TEnumAsByte<EMovementMode>> MovementModeScope = {
		EMovementMode::MOVE_None,
		EMovementMode::MOVE_Walking,
		EMovementMode::MOVE_NavWalking,
		EMovementMode::MOVE_Flying,
		EMovementMode::MOVE_Falling,
		EMovementMode::MOVE_Swimming,
		EMovementMode::MOVE_Custom
	};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMoveFloatScope HeightFromGround = FGBWFlowMoveFloatScope();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWPerceptionResultScope PerceptionResultScope = FGBWPerceptionResultScope();

	FGBWFlowMoveSceneScope(){}
};

UENUM(BlueprintType)
enum EInclusivityType
{
	Include_,
	NotInclude_
};
USTRUCT(BlueprintType)
struct FFlowMoveSceneTypeCondition
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	TEnumAsByte<EInclusivityType> ConditionType = EInclusivityType::Include_;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	TSet<FName> SceneTypeCondition;
};
USTRUCT(BlueprintType)
struct FFlowMoveMovementModeCondition
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	TEnumAsByte<EInclusivityType> ConditionType = EInclusivityType::Include_;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	TSet<TEnumAsByte<EMovementMode>> MovementMode;
};
UENUM(BlueprintType)
enum EInputValueConditionType
{
	HasValue,
	HasNoValue,
	Range
};
USTRUCT(BlueprintType)
struct FFlowMoveMoveVectorCondition
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	TEnumAsByte<EInputValueConditionType> ConditionType = EInputValueConditionType::HasValue;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings",
		meta=(EditConditionHides,EditCondition="ConditionType == EInputValueConditionType::Range"))
	TArray<FGBWFlowMoveFloatScope> MoveVectorSizeRange;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings",
		meta=(EditConditionHides,EditCondition="ConditionType == EInputValueConditionType::Range"))
	TArray<FGBWFlowMoveVectorScope> MoveVectorRange;

	bool IsMet(const FGBWFlowMoveState& FlowMoveState);
	bool IsMet(const FVector& MoveVector);
};

UENUM(BlueprintType)
enum EDirectionConditionType
{
	ValueRange,
	MarkInclude,
	MarkNotInclude
};
USTRUCT(BlueprintType)
struct FFlowMoveSceneDirectionCondition
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	TEnumAsByte<EDirectionConditionType> ConditionType = EDirectionConditionType::MarkInclude;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings",
		meta=(EditConditionHides,EditCondition="ConditionType == EDirectionConditionType::ValueRange"))
	TArray<FGBWFlowMoveFloatScope> SceneDirectionRange;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings",
		meta=(EditConditionHides,EditCondition="ConditionType == EDirectionConditionType::MarkInclude || ConditionType == EDirectionConditionType::MarkNotInclude"))
	TSet<EGBWFlowMoveDirectionType> DirectionMark;

	bool IsMet(const FGBWFlowMoveScene& FlowMoveScene) const;
};

USTRUCT(BlueprintType)
struct FGBWFlowMoveActionLockState
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	bool bHasLock = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	FGameplayTagQuery ActionToLock = FGameplayTagQuery();

	bool IsLock(FGameplayTag ForActionTag) const;
};
UENUM(BlueprintType)
enum EGBWFlowMoveActionLockStateSettingsType
{
	DoNothing,
	Set,
	ClearAll
};
USTRUCT(BlueprintType)
struct FGBWFlowMoveActionLockStateSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	FGameplayTagQuery ActionToLock = FGameplayTagQuery();

	FGBWFlowMoveActionLockStateSettings(){}
};

USTRUCT(BlueprintType)
struct FGBWFlowMoveAnimCurveState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FName Name = NAME_None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float Value = 0.0f;
	
	FGBWFlowMoveAnimCurveState(){}
	FGBWFlowMoveAnimCurveState(FName InName, float InValue)
	{
		Name = InName;
        Value = InValue;
	}
};
USTRUCT(BlueprintType)
struct FGBWFlowMoveAnimPlayRateAdjustSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool bUseSpeedAdjustAnimPlayRate = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float AnimPlayRateAdjustReferenceSpeed = 1500.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float AdjustAnimPlayRateScale = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float Min_AnimPlayRate = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float Max_AnimPlayRate = 3.0f;
	
	FGBWFlowMoveAnimPlayRateAdjustSettings(){}
};
UENUM(BlueprintType)
enum EFlowMoveDirectionType
{
	ActorForward,
	Velocity,
	Controller,
	CurrentMoveToDirection,
	TargetMoveToDirection,
	ActorToTarget,
	StartPointToTarget
};
USTRUCT(BlueprintType)
struct FGBWFlowMoveControlParam
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool bIsActive = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Action")
	FGuid ActionGuid = FGuid();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Action")
	FGameplayTag ActionTag = FGameplayTag();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Action")
	FGameplayTag ActionTargetScene = FGameplayTag();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Action")
	TEnumAsByte<EGBWFlowMoveScenePointType> ActionTargetPoint = EGBWFlowMoveScenePointType::TargetPoint;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWAnimPlayStateSet AnimPlayStateSet = FGBWAnimPlayStateSet();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMoveAnimPlayRateAdjustSettings AnimPlayRateAdjust = FGBWFlowMoveAnimPlayRateAdjustSettings();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float AnimPlayRateNow = 1.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float ToTargetPointRate = 0.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float TargetGravity = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float GravitySmoothSpeed = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float Gravity = 0.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float RotationLockToMoveDirectionYaw = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TEnumAsByte<EFlowMoveDirectionType> RotationLockTo = EFlowMoveDirectionType::CurrentMoveToDirection;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float RotationLockToSmoothSpeed = 12.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float TargetMoveSpeed = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float MoveSpeedSmoothSpeed = 6.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float MoveSpeed = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float MaxMoveSpeed = 5000.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float ConstrainMoveTime = 0.3f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float InertiaTime = 0.3f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float CompensateLostSpeedToCurrentSpeedDirection = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float MoveToDirectionSmoothSpeed = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FVector PathOffset = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FRotator RotationOffset = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	FVector MoveToDirection = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	float PostureAdjustAmplitude = 30.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	float PostureAdjustReferenceSpeed = 1000.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float DeltaToTargetPointRate = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float DeltaGravity = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float DeltaRotationLockToMoveDirectionYaw = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float DeltaMoveSpeed = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	float DeltaMoveToDirectionSmoothSpeed = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FVector DeltaPathOffset = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FRotator DeltaRotationOffset = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool bIsLock_Gravity = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool bIsLock_RotationLockToMoveDirectionYaw = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool bIsLock_MoveSpeed = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool bIsLock_MoveToDirectionSmoothSpeed = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool bIsLock_MaxSpeed = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool bIsLock_ConstrainMoveToTargetPlane = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool bIsLock_UseInertia = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool bIsLock_UsePostureAdjust = false;
	
	FGBWFlowMoveControlParam(){}

	bool HasActiveAnimCurveState(FName InName) const;
};
USTRUCT(BlueprintType)
struct FGBWFlowMoveControlSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Settings|Action")
	FGuid ActionGuid = FGuid();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Action")
	FGameplayTag ActionTag = FGameplayTag();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Action")
	FGameplayTag ActionTargetScene = FGameplayTag();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Action")
	TEnumAsByte<EGBWFlowMoveScenePointType> ActionTargetPoint = EGBWFlowMoveScenePointType::TargetPoint;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl", meta=(InlineEditConditionToggle))
	bool bIsMaxSpeed = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl", meta=(InlineEditConditionToggle))
	bool bIsConstrainMoveToTargetPlane = false;
	//Inertia will be supported in future versions
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl", meta=(InlineEditConditionToggle))
	bool bIsUseInertia = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl", meta=(InlineEditConditionToggle))
	bool bIsGravity = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl", meta=(InlineEditConditionToggle))
	bool bIsRotationLockToMoveDirectionYaw = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl", meta=(InlineEditConditionToggle))
	bool bIsMoveSpeed = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl", meta=(InlineEditConditionToggle))
	bool bIsMoveToDirectionSmoothSpeed = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl", meta=(InlineEditConditionToggle))
	bool bIsUseSpeedAdjustAnimPlayRate = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl", meta=(InlineEditConditionToggle))
	bool bIsUsePostureAdjust = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl",
		meta=(EditCondition="bIsMaxSpeed"))
	float MaxSpeed = 5000.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl",
		meta=(EditCondition="bIsConstrainMoveToTargetPlane"))
	float ConstrainMoveTime = 0.3f;
	//Inertia will be supported in future versions
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl",
		meta=(EditCondition="bIsUseInertia"))
	float InertiaTime = 0.3f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl",
		meta=(EditCondition="bIsGravity"))
	float Gravity = -4.9f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl",
		meta=(EditCondition="bIsGravity"))
	float GravitySmoothSpeed = 6.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl",
		meta=(EditCondition="bIsRotationLockToMoveDirectionYaw"))
	float RotationLockToMoveDirectionYaw = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl",
		meta=(EditCondition="bIsRotationLockToMoveDirectionYaw"))
	TEnumAsByte<EFlowMoveDirectionType> RotationLockTo = EFlowMoveDirectionType::CurrentMoveToDirection;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl",
		meta=(EditCondition="bIsRotationLockToMoveDirectionYaw"))
	float RotationLockToSmoothSpeed = 12.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl",
		meta=(EditCondition="bIsMoveSpeed"))
	float MoveSpeed = 200.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl",
		meta=(EditCondition="bIsMoveSpeed"))
	float MoveSpeedSmoothSpeed = 6.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl",
		meta=(EditCondition="bIsMoveSpeed"))
	bool CompensateLostSpeedToCurrentSpeedDirection = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl",
		meta=(EditCondition="bIsMoveToDirectionSmoothSpeed"))
	float MoveToDirectionSmoothSpeed = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl",
		meta=(EditCondition="bIsUseSpeedAdjustAnimPlayRate"))
	FGBWFlowMoveAnimPlayRateAdjustSettings AnimPlayRateAdjust = FGBWFlowMoveAnimPlayRateAdjustSettings();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl",
		meta=(EditCondition="bIsUsePostureAdjust"))
	float PostureAdjustAmplitude = 30.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|MoveControl",
		meta=(EditCondition="bIsUsePostureAdjust"))
	float PostureAdjustReferenceSpeed = 1000.0f;
	
	FGBWFlowMoveControlSettings(){}
};
USTRUCT(BlueprintType)
struct FGBWFlowMoveState
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	FGuid FlowMoveComponentGuid = FGuid();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	UGBWFlowMoveComponent* FlowMoveComponent = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	ACharacter* OwnerCharacter = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	bool bIsActive = false;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	bool bIsStopping = false;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	float Timer = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	float FrameDeltaTime = 0.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	TEnumAsByte<EMovementMode> MovementMode = MOVE_None;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	TEnumAsByte<EFlowMoveCharacterViewMode> ViewMode = EFlowMoveCharacterViewMode::TP_FreeMode;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	USceneComponent* FocusActorComponent = nullptr;

	UPROPERTY()
	FGameplayTagContainer PerceptronReady = FGameplayTagContainer();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FGameplayTagContainer PerceptronNow = FGameplayTagContainer();
	UPROPERTY()
	FGameplayTagContainer PerceptronToEnd = FGameplayTagContainer();
	UPROPERTY()
	FGameplayTagContainer PerceptronInvalid = FGameplayTagContainer();

	UPROPERTY()
	FGameplayTagContainer ScriptReady = FGameplayTagContainer();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FGameplayTagContainer ScriptNow = FGameplayTagContainer();
	UPROPERTY()
	FGameplayTagContainer ScriptToEnd = FGameplayTagContainer();
	UPROPERTY()
	FGameplayTagContainer ScriptInvalid = FGameplayTagContainer();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	TArray<FGBWSupplementaryFlowMoveScene> SupplementaryFlowMoveScene;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FVector MoveVector = FVector::ZeroVector;
	UPROPERTY()
	FVector LastMoveVector = FVector::ZeroVector;
	UPROPERTY()
	float LastMoveVectorTime = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FVector ControlVector = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FVector ForwardVector = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FVector PerceptionVector = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FGBWFlowMoveActionLockState ActionLockState = FGBWFlowMoveActionLockState();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FGameplayTag LastFlowMoveActionTag = FGameplayTag();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FGameplayTag CurrentFlowMoveActionTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	float CurrentFlowMoveExecutedTime = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FGBWFlowMoveScene CurrentActionTargetScene = FGBWFlowMoveScene();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FGBWFlowMoveControlParam MoveControlParam = FGBWFlowMoveControlParam();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FTransform RMSParamNow = FTransform();
	FTransform RMSParamNow_Local = FTransform();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	float LostSpeed = 0.0f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FTransform LastActorTransform = FTransform();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FVector RealVelocity = FVector();

	TArray<FFlowMoveEvent> FlowMoveEventNow;

	FGBWFlowMoveState(){}

	void MakeVelocity();
	
	void ClearCurrentFlowMoveAction();
	void ClearLastFlowMoveAction();
	
	void SetSupplementaryScene(FGameplayTag SceneSlot, FGBWFlowMoveScene NewScene);
	void RemoveSupplementaryScene(FGameplayTag SceneSlot);
	bool GetSupplementaryScene(FGameplayTag SceneSlot, FGBWFlowMoveScene& Result);
	int GetSupplementarySceneIndex(FGameplayTag SceneSlot);

	void ClearFlowMoveEvent();
	void AddFlowMoveEvent(const FFlowMoveEvent& NewEvent);
	bool IsHasFlowMoveEvent(const FFlowMoveEvent& FlowMoveEvent);

	void SetMoveVector(const FVector& NewMoveVector);

	void AddPerceptronTag(FGameplayTag PerceptronTag);
	void ReadyPerceptronTag(FGameplayTag PerceptronTag);
	void RemovePerceptronTag(FGameplayTag PerceptronTag);
	void FinishPerceptronTag(FGameplayTag PerceptronTag);
	void CheckPerceptronTag();

	void AddScriptTag(FGameplayTag ScriptTag);
	void ReadyScriptTag(FGameplayTag ScriptTag);
	void RemoveScriptTag(FGameplayTag ScriptTag);
	void FinishScriptTag(FGameplayTag ScriptTag);
	void CheckScriptTag();
};
USTRUCT(BlueprintType)
struct FGBWFlowMoveRMSControllerInitInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool bIsValid = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGameplayTag ActionTargetScene = FGameplayTag();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FGBWFlowMoveState BeginFlowMoveState = FGBWFlowMoveState();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FTransform BeginTransform = FTransform();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FVector BeginVelocity = FVector();
	
	FGBWFlowMoveRMSControllerInitInfo(){}
	FGBWFlowMoveRMSControllerInitInfo(const FGBWFlowMoveState& FlowMoveState);
};

UENUM(BlueprintType)
enum EFlowMoveNetworkLocationType
{
	Server,
	Client
};
//END struct

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, HideDropdown, meta = (DisplayName = "GBWFlowMovePerceptron_Base"))
class GBWFLOWMOVE_API UGBWFlowMovePerceptron_Base : public UGBWFlowMoveObject_Base
{
	GENERATED_BODY()
public:
	UGBWFlowMovePerceptron_Base()
	{}

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category="PerceptronSettings")
	bool bIsDefaultActive = false;
	
	bool bIsActive = false;

	FGBWFlowMoveScene FlowMoveSceneCache = FGBWFlowMoveScene();
	bool bHasCache = false;

	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Perception")
	void SetCache(FGBWFlowMoveScene TheFlowMoveSceneCache);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Perception")
	bool GetCache(FGBWFlowMoveScene& Result, const bool bConsume = true);

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|FlowMove|Perception")
	bool GetPerceptionResult(
			FGBWFlowMoveScene& FlowMoveScene,
			EGBWFlowMoveEventType EventType,
			ACharacter* OwnerCharacter,
			UGBWFlowMoveComponent* FlowMoveComponent);
	virtual bool GetPerceptionResult_Implementation(
			FGBWFlowMoveScene& FlowMoveScene,
			EGBWFlowMoveEventType EventType,
			ACharacter* OwnerCharacter,
			UGBWFlowMoveComponent* FlowMoveComponent);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Perception")
			bool GetThePerceptionResult(
			FGBWFlowMoveScene& FlowMoveScene,
			const EGBWFlowMoveEventType EventType,
			ACharacter* OwnerCharacter,
			UGBWFlowMoveComponent* FlowMoveComponent);

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|FlowMove|Perception")
	void OnUpdate(
		FGameplayTag PerceptionKey,
		EGBWFlowMoveEventType EventType,
		UGBWFlowMoveComponent* FlowMoveComponent);
	virtual void OnUpdate_Implementation(
		FGameplayTag PerceptionKey,
		EGBWFlowMoveEventType EventType,
		UGBWFlowMoveComponent* FlowMoveComponent){}
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Perception")
	void Update(FGameplayTag PerceptionKey, EGBWFlowMoveEventType EventType,
		UGBWFlowMoveComponent* FlowMoveComponent){OnUpdate(PerceptionKey, EventType, FlowMoveComponent);}

	UPROPERTY()
	TMap<UObject*,UGBWFlowMovePerceptron_Base*> CopyCache;
	void CheckCopyCache();
	UGBWFlowMovePerceptron_Base* GetCopy(UObject* Outer);
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, HideDropdown, meta = (DisplayName = "GBWFlowMoveScript_Base"))
class GBWFLOWMOVE_API UGBWFlowMoveScript_Base : public UGBWFlowMoveObject_Base
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category="ScriptSettings")
	bool bIsDefaultActive = false;
	
	UGBWFlowMoveScript_Base()
	{}
	
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|FlowMove")
	void OnUpdate(
		EGBWFlowMoveEventType EventType,
		EFlowMoveNetworkLocationType NetworkLocation,
		bool bIsLocalOwn,
		UGBWFlowMoveComponent* FlowMoveComponent);
	virtual void OnUpdate_Implementation(
		EGBWFlowMoveEventType EventType,
		EFlowMoveNetworkLocationType NetworkLocation,
		bool bIsLocalOwn,
		UGBWFlowMoveComponent* FlowMoveComponent){}
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove")
	void Update(
		EGBWFlowMoveEventType EventType,
		EFlowMoveNetworkLocationType NetworkLocation,
		bool bIsLocalOwn,
		UGBWFlowMoveComponent* FlowMoveComponent);

	UPROPERTY()
	TMap<UObject*,UGBWFlowMoveScript_Base*> CopyCache;
	void CheckCopyCache();
	UGBWFlowMoveScript_Base* GetCopy(UObject* Outer);
};
//这里有个类叫做FlowMoveGetFocusActor
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, HideDropdown, meta = (DisplayName = "GBWFlowMoveGetFocusActor_Base"))
class GBWFLOWMOVE_API UGBWFlowMoveGetFocusActor_Base : public UGBWFlowMoveObject_Base
{
	GENERATED_BODY()
public:
	UGBWFlowMoveGetFocusActor_Base()
	{}
	//定义为BlueprintNativeEvent 就可以在蓝图中进行覆写 _Implementation
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|FlowMove")
	USceneComponent* OnGetFocusActor(
		UGBWFlowMoveComponent* FlowMoveComponent);
	virtual USceneComponent* OnGetFocusActor_Implementation(
		UGBWFlowMoveComponent* FlowMoveComponent);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove")
	USceneComponent* GetFocusActor(
		UGBWFlowMoveComponent* FlowMoveComponent);

	UFUNCTION(BlueprintCallable, Category="GBW|FlowMove")
	bool TestVisibility(
		AActor* StartActor,
		AActor* EndActor,
		FGBWFlowMoveTraceSetting TraceSetting,
		FVector& TouchPoint);
};

//这是ActionTree 并且有一个ActionTreeeNodeType这个枚举
UENUM(BlueprintType)
enum EFlowMoveActionTreeNodeType
{
	TransitNode,
	ActionNode
};
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "ActionTreeNode"))
class GBWFLOWMOVE_API UGBWFlowMoveActionTree : public UGBWFlowMoveObject_Base
{
	GENERATED_BODY()
public:
	UGBWFlowMoveActionTree()
	{}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("Some Description For This Action Tree");
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	TEnumAsByte<EFlowMoveActionTreeNodeType> TreeNodeType = EFlowMoveActionTreeNodeType::ActionNode;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings", meta=(EditConditionHides, EditCondition="TreeNodeType == EFlowMoveActionTreeNodeType::ActionNode"))
	FGameplayTag ActionTag = FGameplayTag();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings", meta=(EditConditionHides, EditCondition="TreeNodeType == EFlowMoveActionTreeNodeType::ActionNode"))
	FGameplayTag ActionTargetScene = FGameplayTag();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings", meta=(EditConditionHides, EditCondition="TreeNodeType == EFlowMoveActionTreeNodeType::ActionNode"))
	FGBWFlowMoveAnimSetting ActionAnim = FGBWFlowMoveAnimSetting();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category="Settings", meta=(EditConditionHides, EditCondition="TreeNodeType == EFlowMoveActionTreeNodeType::TransitNode"))
	TMap<FName, UGBWFlowMoveActionTree*> ActionTrees;
	
	bool IsActionValid() const;
};

USTRUCT(BlueprintType)
struct FFlowMoveActionNodeCache
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	FString ActionName = "";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	FGameplayTag ActionTag = FGameplayTag();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	UGBWFlowMoveActionTree* ActionNode = nullptr;
	
	FFlowMoveActionNodeCache(){}
	FFlowMoveActionNodeCache(
		FString InActionName,
		FGameplayTag InActionTag,
		UGBWFlowMoveActionTree* InActionNode)
	{
		ActionName = InActionName;
		ActionTag = InActionTag;
		ActionNode = InActionNode;
	}
};
USTRUCT(BlueprintType)
struct FFlowMoveAnimCurveNameSetting
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FName ToTargetPointRate = FName("FM.ToTargetRate");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FName Gravity = FName("FM.Gravity");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FName RotationLockToMoveDirectionYaw = FName("FM.RotLock");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FName MoveSpeed = FName("FM.MoveSpeed");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FName MoveToDirectionSmoothSpeed = FName("FM.MoveToDirectionSmoothSpeed");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FName PathOffset_Forward = FName("FM.F");
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FName PathOffset_Right = FName("FM.R");
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FName PathOffset_Up = FName("FM.U");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FName RotationOffset_Pitch = FName("FM.Pitch");
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FName RotationOffset_Yaw = FName("FM.Yaw");
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FName RotationOffset_Roll = FName("FM.Roll");
};


UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "GBWFlowMoveBrain_Base"))
class GBWFLOWMOVE_API UGBWFlowMoveBrain_Base : public UGBWFlowMoveObject_Base
{
	//Brain是MoveObject_Base的一个子类
	GENERATED_BODY()
public:
	UGBWFlowMoveBrain_Base(){}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="FlowMoveSettings|AnimState")
	FGBWAnimStateGetType AnimStateGetType = FGBWAnimStateGetType();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="FlowMoveSettings|View")
	TEnumAsByte<EFlowMoveCharacterViewMode> DefaultViewMode = EFlowMoveCharacterViewMode::TP_ForwardLockMode;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="FlowMoveSettings|Input")
	float MoveVectorZeroFaultToleranceDuration = 0.1f;//移动向量零容错时长 Tolerance
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="FlowMoveSettings|Input"
		,meta=(InlineEditConditionToggle))
	bool bUseDefaultMoveVectorWhenZero = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="FlowMoveSettings|Input"
		,meta=(EditCondition="bUseDefaultMoveVectorWhenZero"))
	FVector DefaultMoveVector = FVector(1,0,0);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="FlowMoveSettings|Input")
	FFlowMoveInputValueTrigger_PerceptionInput PerceptionInputSettings = FFlowMoveInputValueTrigger_PerceptionInput();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FlowMoveSettings|State")
	FGameplayTag ActionStateKey = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FlowMoveSettings|State")
	FGameplayTag ActionEventKey_Start = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FlowMoveSettings|State")
	FGameplayTag ActionEventKey_End = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FlowMoveSettings|State")
	FGameplayTag ActionEventValueKey = FGameplayTag();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FlowMoveSettings|Movement")
	FFlowMoveAnimCurveNameSetting AnimCurveNameSetting = FFlowMoveAnimCurveNameSetting();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced, Category = "FlowMoveSettings|ResourcePool",meta=(ForceInlineRow))
	TMap<FGameplayTag, UGBWFlowMovePerceptron_Base*> FlowMovePerceptronSet;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced, Category = "FlowMoveSettings|ResourcePool",meta=(ForceInlineRow))
	TMap<FGameplayTag, UGBWFlowMoveScript_Base*> FlowMoveScriptSet;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced, Category = "FlowMoveSettings|ResourcePool",meta=(ForceInlineRow))
	TMap<FGameplayTag, UGBWFlowMoveGetFocusActor_Base*> FlowMoveGetTargetFunctionSet;

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|FlowMove|Event")
	void OnFlowMoveEvent(
		ACharacter* OwnerCharacter,
		UGBWFlowMoveComponent* FlowMoveComponent,
		FGBWFlowMoveState FlowMoveState,
		const FFlowMoveEvent& FlowMoveEvent);
	virtual void OnFlowMoveEvent_Implementation(
		ACharacter* OwnerCharacter,
		UGBWFlowMoveComponent* FlowMoveComponent,
		FGBWFlowMoveState FlowMoveState,
		const FFlowMoveEvent& FlowMoveEvent);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Event")
	void OnFMEvent(
		ACharacter* OwnerCharacter,
		UGBWFlowMoveComponent* FlowMoveComponent,
		FGBWFlowMoveState FlowMoveState,
		const FFlowMoveEvent& FlowMoveEvent);

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|FlowMove|State")
	void GetFlowMoveIsActive(
		ACharacter* OwnerCharacter,
		UGBWFlowMoveComponent* FlowMoveComponent,
		float DeltaTime,
		FGBWFlowMoveState FlowMoveState,
		bool& bIsActive,
		bool& WaitForCurrentActionFinished);
	virtual void GetFlowMoveIsActive_Implementation(
		ACharacter* OwnerCharacter,
		UGBWFlowMoveComponent* FlowMoveComponent,
		float DeltaTime,
		FGBWFlowMoveState FlowMoveState,
		bool& bIsActive,
		bool& WaitForCurrentActionFinished);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|State")
	void GetFMIsActive(
		ACharacter* OwnerCharacter,
		UGBWFlowMoveComponent* FlowMoveComponent,
		float DeltaTime,
		FGBWFlowMoveState FlowMoveState,
		bool& bIsActive,
		bool& WaitForCurrentActionFinished);

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|FlowMove|Input")
	void GetMoveVector(
		ACharacter* OwnerCharacter,
		UGBWFlowMoveComponent* FlowMoveComponent,
		float DeltaTime,
		FGBWFlowMoveState FlowMoveState,
		bool& bIsGet,
		FVector& MoveVector);
	virtual void GetMoveVector_Implementation(
		ACharacter* OwnerCharacter,
		UGBWFlowMoveComponent* FlowMoveComponent,
		float DeltaTime,
		FGBWFlowMoveState FlowMoveState,
		bool& bIsGet,
		FVector& MoveVector);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Input")
	void GetFMMoveVector(
		ACharacter* OwnerCharacter,
		UGBWFlowMoveComponent* FlowMoveComponent,
		float DeltaTime,
		FGBWFlowMoveState FlowMoveState,
		bool& bIsGet,
		FVector& MoveVector);

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|FlowMove|Input")
	void GetControlVector(
		ACharacter* OwnerCharacter,
		UGBWFlowMoveComponent* FlowMoveComponent,
		float DeltaTime,
		FGBWFlowMoveState FlowMoveState,
		bool& bIsGet,
		FVector& ControlVector);
	virtual void GetControlVector_Implementation(
		ACharacter* OwnerCharacter,
		UGBWFlowMoveComponent* FlowMoveComponent,
		float DeltaTime,
		FGBWFlowMoveState FlowMoveState,
		bool& bIsGet,
		FVector& ControlVector);
	UFUNCTION(BlueprintCallable, Category = "GBW|FlowMove|Input")
	void GetFMControlVector(
		ACharacter* OwnerCharacter,
		UGBWFlowMoveComponent* FlowMoveComponent,
		float DeltaTime,
		FGBWFlowMoveState FlowMoveState,
		bool& bIsGet,
		FVector& ControlVector);
};