// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "UObject/Interface.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GBWPowerfulStateComponent.generated.h"

UENUM(BlueprintType)
enum EPS_ValueType
{
	Float,
	Vector,
	Transform,
	Actor,
	Text,
	GameplayTag,
	Bool
};
UENUM(BlueprintType)
enum EPS_StateEventType
{
	Add,
	Change,
	Remove
};
USTRUCT(BlueprintType, meta = (DisplayName = "PS_Value"))
struct FPS_Value
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	TEnumAsByte<EPS_ValueType> ValueType = EPS_ValueType::Float;

	UPROPERTY(EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="ValueType==EPS_ValueType::Float"))
	float FValue = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="ValueType==EPS_ValueType::Vector"))
	FVector VValue = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="ValueType==EPS_ValueType::Transform"))
	FTransform TValue = FTransform();

	UPROPERTY(EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="ValueType==EPS_ValueType::Actor"))
	AActor* ActorValue = nullptr;

	UPROPERTY(EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="ValueType==EPS_ValueType::Text"))
	FString TextValue = "";

	UPROPERTY(EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="ValueType==EPS_ValueType::GameplayTag"))
	FGameplayTag GTValue = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="ValueType==EPS_ValueType::GameplayTag"))
	bool BoolValue = false;

	float GetFValue() const;
	FVector GetVValue() const;
	FTransform GetTValue() const;
	AActor* GetActorValue() const;
	FString GetTextValue() const;
	FGameplayTag GetGTValue() const;
	bool GetBoolValue() const;

	bool IsTypeMatch(EPS_ValueType InValueType) const;

	void SetFValue(const float NewValue);
	void SetVValue(const FVector& NewValue);
	void SetTValue(const FTransform& NewValue);
	void SetActorValue(AActor* NewValue);
	void SetTextValue(FString NewValue);
	void SetGTValue(const FGameplayTag NewValue);
	void SetBoolValue(const bool NewValue);

	FPS_Value(){}
	FPS_Value(const float NewValue);
	FPS_Value(const FVector& NewValue);
	FPS_Value(const FTransform& NewValue);
	FPS_Value(AActor* NewValue);
	FPS_Value(FString NewValue);
	FPS_Value(FGameplayTag NewValue);
	FPS_Value(bool NewValue);

	bool EqualTo(const FPS_Value& Other) const;
	
	FString GetString() const;
};
USTRUCT(BlueprintType, meta = (DisplayName = "PS_Element"))
struct FPS_Element
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FGameplayTag Key = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FPS_Value Value = FPS_Value();
	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	float Time = -1.0f;

	FPS_Element(){}
	FPS_Element(FGameplayTag InKey, const FPS_Value& InValue, float InTime);

	FString GetString(UWorld* InWorld, bool bIsGetOperationTime = false) const;
};
USTRUCT(BlueprintType, meta = (DisplayName = "PS_State"))
struct FPS_StateOperation
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	TEnumAsByte<EPS_StateEventType> OperationType = EPS_StateEventType::Add;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FPS_Element StateElement = FPS_Element();

	FPS_StateOperation(){}
	FPS_StateOperation(EPS_StateEventType InOperationType, FPS_Element InStateElement);
	FString GetString(UWorld* InWorld) const;
};
USTRUCT(BlueprintType, meta = (DisplayName = "PS_State"))
struct FPS_State
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	TArray<FPS_Element> State;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Settings")
	TArray<FPS_StateOperation> StateOperations;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	int MaxStateOperationNum = 2048;
	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	int StateOperationIndexNow = -1;

	void SetState(FGameplayTag Key, const FPS_Value& Value, float Time = -1.0);
	bool RemoveState(FGameplayTag Key, FPS_Value& OldValue, float Time);

	bool GetFValue(FGameplayTag Key, float& Result, float& Time);
	bool GetVValue(FGameplayTag Key, FVector& Result, float& Time);
	bool GetTValue(FGameplayTag Key, FTransform& Result, float& Time);
	bool GetActorValue(FGameplayTag Key, AActor*& Result, float& Time);
	bool GetTextValue(FGameplayTag Key, FString& Result, float& Time);
	bool GetGTValue(FGameplayTag Key, FGameplayTag& Result, float& Time);
	bool GetBoolValue(FGameplayTag Key, bool& Result, float& Time);

	bool GetFValue_History(FGameplayTag Key, int DistanceFromNow, float& Result, float& Time);
	bool GetVValue_History(FGameplayTag Key, int DistanceFromNow, FVector& Result, float& Time);
	bool GetTValue_History(FGameplayTag Key, int DistanceFromNow, FTransform& Result, float& Time);
	bool GetActorValue_History(FGameplayTag Key, int DistanceFromNow, AActor*& Result, float& Time);
	bool GetTextValue_History(FGameplayTag Key, int DistanceFromNow, FString& Result, float& Time);
	bool GetGTValue_History(FGameplayTag Key, int DistanceFromNow, FGameplayTag& Result, float& Time);
	bool GetBoolValue_History(FGameplayTag Key, int DistanceFromNow, bool& Result, float& Time);

	void AddStateOperation(const FPS_StateOperation& NewStateOperation);

	bool CheckStateChangeType(
		FGameplayTag Key,
		const FPS_Value& Value,
		EPS_StateEventType& ChangeType,
		FPS_Value& OldValue,
		FPS_Value& NewValue
	);
	
	FString GetString(UWorld* InWorld, int StateOperationNum);
};

USTRUCT(BlueprintType, meta = (DisplayName = "PE_Event"))
struct FPE_Event
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FGameplayTag Key = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, Category = "Settings")
	FPS_State EventState = FPS_State();

	FString GetString(UWorld* InWorld = nullptr);
};

USTRUCT(BlueprintType, meta = (DisplayName = "PS_Event"))
struct FPS_Event
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	TEnumAsByte<EPS_StateEventType> StateEventType = EPS_StateEventType::Add;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FGameplayTag StateKey = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FPS_Value OldValue = FPS_Value();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FPS_Value NewValue = FPS_Value();

	FPS_Event(){}
	FPS_Event(EPS_StateEventType InStateEventType,FGameplayTag InStateKey,FPS_Value InOldValue,FPS_Value InNewValue);

	FString GetString();
};

UINTERFACE(MinimalAPI)
class UGBWPSBPI : public UInterface
{
	GENERATED_BODY()
};
class GBWPOWERFULSTATE_API IGBWPSBPI
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="GBW|PE")
	void OnPowerfulEvent(FGameplayTag EventKey, FPE_Event PE_Event);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="GBW|PS")
	void OnPowerfulState(FGameplayTag StateKey, FPS_Event PS_Event);
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBWPOWERFULSTATE_API UGBWPowerfulStateComponent : public UActorComponent
{
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGBWPEDlg, FGameplayTag, EventKey, FPE_Event, PE_Event);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGBWPSDlg, FGameplayTag, StateKey, FPS_Event, PS_Event);
	GENERATED_BODY()

public:	
	UGBWPowerfulStateComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "GBW|PE")
	FGBWPEDlg OnPowerfulEvent;
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "GBW|PS")
	FGBWPSDlg OnPowerfulState;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="GBW|PowerfulState|Settings")
	int MaxStateOperationNum = 2048;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="GBW|PowerfulState|Debug")
	bool bIsDebug;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="GBW|PowerfulState|Debug")
	int StateOperationNum = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="GBW|PowerfulState|Debug")
	FColor DebugTextColor = FColor::Green;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="GBW|PowerfulState|Debug")
	FColor DebugTextColor2 = FColor::Yellow;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="GBW|PowerfulState|Debug")
	bool bPrintToScreen = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="GBW|PowerfulState|Debug")
	bool bPrintToLog = true;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated, Category="GBW|PowerfulState")
	FGuid PowerfulStateGuid = FGuid::NewGuid();
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated, Category="GBW|PowerfulState")
	FPS_State PowerfulState;
	
protected:
	virtual void BeginPlay() override;
	
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
//State BEGIN
	bool HasNetAuth() const;
	void SetState_Imp(FGameplayTag Key, const FPS_Value& Value);

	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	void SetMaxStateOperationNum(int InMaxStateOperationNum);
	UFUNCTION(Reliable,Server,Category = "GBW|PS|Component")
	void SetMaxStateOperationNum_Server(int InMaxStateOperationNum);
	
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	void SetState(FGameplayTag Key, FPS_Value Value);
	UFUNCTION(Reliable,Server,Category = "GBW|PS|Component")
	void SetState_Server(FGameplayTag Key, FPS_Value Value);

	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	void RemoveState(FGameplayTag Key);
	UFUNCTION(Reliable,Server,Category = "GBW|PS|Component")
	void RemoveState_Server(FGameplayTag Key);

	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	void SetStateFloat(FGameplayTag Key,float Value);
	UFUNCTION(Reliable,Server,Category = "GBW|PS|Component")
	void SetStateFloat_Server(FGameplayTag Key,float Value);
	
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	void SetStateVector(FGameplayTag Key,FVector Value);
	UFUNCTION(Reliable,Server,Category = "GBW|PS|Component")
	void SetStateVector_Server(FGameplayTag Key,FVector Value);
	
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	void SetStateTransform(FGameplayTag Key,FTransform Value);
	UFUNCTION(Reliable,Server,Category = "GBW|PS|Component")
	void SetStateTransform_Server(FGameplayTag Key,FTransform Value);
	
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	void SetStateActor(FGameplayTag Key,AActor* Value);
	UFUNCTION(Reliable,Server,Category = "GBW|PS|Component")
	void SetStateActor_Server(FGameplayTag Key,AActor* Value);
	
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	void SetStateText(FGameplayTag Key, const FString Value);
	UFUNCTION(Reliable,Server,Category = "GBW|PS|Component")
	void SetStateText_Server(FGameplayTag Key, const FString& Value);
	
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	void SetStateGT(FGameplayTag Key,FGameplayTag Value);
	UFUNCTION(Reliable,Server,Category = "GBW|PS|Component")
	void SetStateGT_Server(FGameplayTag Key,FGameplayTag Value);

	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	void SetStateBool(FGameplayTag Key,bool Value);
	UFUNCTION(Reliable,Server,Category = "GBW|PS|Component")
	void SetStateBool_Server(FGameplayTag Key,bool Value);
	
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	bool GetFValue(FGameplayTag Key, float& Result, float& Duration);
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	bool GetVValue(FGameplayTag Key, FVector& Result, float& Duration);
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	bool GetTValue(FGameplayTag Key, FTransform& Result, float& Duration);
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	bool GetActorValue(FGameplayTag Key, AActor*& Result, float& Duration);
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	bool GetTextValue(FGameplayTag Key, FString& Result, float& Duration);
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	bool GetGTValue(FGameplayTag Key, FGameplayTag& Result, float& Duration);
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	bool GetBoolValue(FGameplayTag Key, bool& Result, float& Duration);

	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	bool GetFValue_History(FGameplayTag Key, int DistanceFromNow, float& Result, float& Duration);
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	bool GetVValue_History(FGameplayTag Key, int DistanceFromNow, FVector& Result, float& Duration);
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	bool GetTValue_History(FGameplayTag Key, int DistanceFromNow, FTransform& Result, float& Duration);
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	bool GetActorValue_History(FGameplayTag Key, int DistanceFromNow, AActor*& Result, float& Duration);
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	bool GetTextValue_History(FGameplayTag Key, int DistanceFromNow, FString& Result, float& Duration);
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	bool GetGTValue_History(FGameplayTag Key, int DistanceFromNow, FGameplayTag& Result, float& Duration);
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component")
	bool GetBoolValue_History(FGameplayTag Key, int DistanceFromNow, bool& Result, float& Duration);
//State END

//Event BEGIN
	UFUNCTION(BlueprintCallable,Category = "GBW|PE|Component")
	void SendEvent(FPE_Event PE_Event);
	UFUNCTION(Reliable,Server,Category = "GBW|PE|Component")
	void SendEvent_Server(FPE_Event PE_Event);
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|PE|Component")
	void SendEvent_Multicast(FPE_Event PE_Event);
	void SendEvent_Imp(const FPE_Event& PE_Event) const;
//Event END

//Debug Begin
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component|Debug")
	void ActivePSDebug(FColor InDebugTextColor = FColor::Green, bool InPrintToScreen = true, bool InPrintToLog = true);
	UFUNCTION(Reliable,Server,Category = "GBW|PS|Component|Debug")
	void ActivePSDebug_Server(FColor InDebugTextColor = FColor::Green, bool InPrintToScreen = true, bool InPrintToLog = true);
	UFUNCTION(BlueprintCallable,Category = "GBW|PS|Component|Debug")
	void DeactivePSDebug();
	UFUNCTION(Reliable,Server,Category = "GBW|PS|Component|Debug")
	void DeactivePSDebug_Server();
	
	void PSDebug();
	void PEDebug(const FPE_Event& PE_Event) const;
//Debug END
};
