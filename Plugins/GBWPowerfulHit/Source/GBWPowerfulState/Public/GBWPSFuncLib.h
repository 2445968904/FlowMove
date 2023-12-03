// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GBWPowerfulStateComponent.h"
#include "GBWPSFuncLib.generated.h"

UCLASS()
class GBWPOWERFULSTATE_API UGBWPSFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Value|Make", meta=(BlueprintThreadSafe))
	static FPS_Value MakePSFloat(float Value);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Value|Make", meta=(BlueprintThreadSafe))
	static FPS_Value MakePSVector(FVector Value);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Value|Make", meta=(BlueprintThreadSafe))
	static FPS_Value MakePSTransform(FTransform Value);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Value|Make", meta=(BlueprintThreadSafe))
	static FPS_Value MakePSActor(AActor* Value);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Value|Make", meta=(BlueprintThreadSafe))
	static FPS_Value MakePSText(FString Value);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Value|Make", meta=(BlueprintThreadSafe))
	static FPS_Value MakePSGT(FGameplayTag Value);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Value|Make", meta=(BlueprintThreadSafe))
	static FPS_Value MakePSBool(bool Value);

	UFUNCTION(BlueprintCallable, Category = "GBW|PS|Value")
	static float GetFValue(const FPS_Value PSValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|Value")
	static FVector GetVValue(const FPS_Value PSValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|Value")
	static FTransform GetTValue(const FPS_Value PSValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|Value")
	static AActor* GetActorValue(const FPS_Value PSValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|Value")
	static FString GetTextValue(const FPS_Value PSValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|Value")
	static FGameplayTag GetGTValue(const FPS_Value PSValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|Value")
	static bool GetBoolValue(const FPS_Value PSValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|Value")
	static FString GetValueDebugString(const FPS_Value PSValue);

	UFUNCTION(BlueprintCallable, Category = "GBW|PS|State|Make")
	static FPS_State MakePowerfulState();
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|State|Make")
	static FPS_State AddFloatToPS(FPS_State InState, FGameplayTag Key, float Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|State|Make")
	static FPS_State AddVectorToPS(FPS_State InState, FGameplayTag Key, FVector Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|State|Make")
	static FPS_State AddTransformToPS(FPS_State InState, FGameplayTag Key, FTransform Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|State|Make")
	static FPS_State AddActorToPS(FPS_State InState, FGameplayTag Key, AActor* Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|State|Make")
	static FPS_State AddTextToPS(FPS_State InState, FGameplayTag Key, FString Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|State|Make")
	static FPS_State AddGTToPS(FPS_State InState, FGameplayTag Key, FGameplayTag Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|State|Make")
	static FPS_State AddBoolToPS(FPS_State InState, FGameplayTag Key, bool Value);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|State")
	static bool GetStateFValue(FPS_State InState, FGameplayTag Key, float& Result);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|State")
	static bool GetStateVValue(FPS_State InState, FGameplayTag Key, FVector& Result);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|State")
	static bool GetStateTValue(FPS_State InState, FGameplayTag Key, FTransform& Result);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|State")
	static bool GetStateActorValue(FPS_State InState, FGameplayTag Key, AActor*& Result);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|State")
	static bool GetStateTextValue(FPS_State InState, FGameplayTag Key, FString& Result);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|State")
	static bool GetStateGTValue(FPS_State InState, FGameplayTag Key, FGameplayTag& Result);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS|State")
	static bool GetStateBoolValue(FPS_State InState, FGameplayTag Key, bool& Result);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS")
	static UGBWPowerfulStateComponent* GetPowerfulStateComponent(AActor* InActor);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void SetPowerfulState(AActor* InActor, bool bOnlyTriggerInServer, FGameplayTag Key, FPS_Value Value);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void PS_SetPowerfulState(bool InCondition, bool ExecuteWhenNotMeetCondition, bool& OutCondition, const UAnimInstance* AnimInstance, FGameplayTag Key, FPS_Value Value);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void RemovePowerfulState(AActor* InActor, bool bOnlyTriggerInServer, FGameplayTag Key);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void PS_RemovePowerfulState(bool InCondition, bool ExecuteWhenNotMeetCondition, bool& OutCondition, const UAnimInstance* AnimInstance, FGameplayTag Key);

	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void SetPSFloat(AActor* InActor, FGameplayTag Key,float Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void SetPSVector(AActor* InActor, FGameplayTag Key,FVector Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void SetPSTransform(AActor* InActor, FGameplayTag Key,FTransform Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void SetPSActor(AActor* InActor, FGameplayTag Key,AActor* Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void SetPSText(AActor* InActor, FGameplayTag Key, const FString Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void SetPSGT(AActor* InActor, FGameplayTag Key,FGameplayTag Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void SetPSBool(AActor* InActor, FGameplayTag Key,bool Value);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void GetPSFValue(AActor* InActor, FGameplayTag Key, bool& bIsGet, float& Result, float& Duration);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void PS_GetPSFValue(const UAnimInstance* AnimInstance, FGameplayTag Key, bool& bIsGet, float& Result, float& Duration);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void GetPSVValue(AActor* InActor, FGameplayTag Key, bool& bIsGet, FVector& Result, float& Duration);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void PS_GetPSVValue(const UAnimInstance* AnimInstance, FGameplayTag Key, bool& bIsGet, FVector& Result, float& Duration);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void GetPSTValue(AActor* InActor, FGameplayTag Key, bool& bIsGet, FTransform& Result, float& Duration);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void PS_GetPSTValue(const UAnimInstance* AnimInstance, FGameplayTag Key, bool& bIsGet, FTransform& Result, float& Duration);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void GetPSActorValue(AActor* InActor, FGameplayTag Key, bool& bIsGet, AActor*& Result, float& Duration);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void PS_GetPSActorValue(const UAnimInstance* AnimInstance, FGameplayTag Key, bool& bIsGet, AActor*& Result, float& Duration);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void GetPSTextValue(AActor* InActor, FGameplayTag Key, bool& bIsGet, FString& Result, float& Duration);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void PS_GetPSTextValue(const UAnimInstance* AnimInstance, FGameplayTag Key, bool& bIsGet, FString& Result, float& Duration);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void GetPSGTValue(AActor* InActor, FGameplayTag Key, bool& bIsGet, FGameplayTag& Result, float& Duration);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void PS_GetPSGTValue(const UAnimInstance* AnimInstance, FGameplayTag Key, bool& bIsGet, FGameplayTag& Result, float& Duration);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void GetPSBoolValue(AActor* InActor, FGameplayTag Key, bool& bIsGet, bool& Result, float& Duration);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void PS_GetPSBoolValue(const UAnimInstance* AnimInstance, FGameplayTag Key, bool& bIsGet, bool& Result, float& Duration);

	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void GetPSFValue_History(AActor* InActor, FGameplayTag Key, int DistanceFromNow, bool& bIsGet, float& Result, float& Duration);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void PS_GetPSFValue_History(const UAnimInstance* AnimInstance, FGameplayTag Key, int DistanceFromNow, bool& bIsGet, float& Result, float& Duration);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void GetPSVValue_History(AActor* InActor, FGameplayTag Key, int DistanceFromNow, bool& bIsGet, FVector& Result, float& Duration);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void PS_GetPSVValue_History(const UAnimInstance* AnimInstance, FGameplayTag Key, int DistanceFromNow, bool& bIsGet, FVector& Result, float& Duration);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void GetPSTValue_History(AActor* InActor, FGameplayTag Key, int DistanceFromNow, bool& bIsGet, FTransform& Result, float& Duration);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void PS_GetPSTValue_History(const UAnimInstance* AnimInstance, FGameplayTag Key, int DistanceFromNow, bool& bIsGet, FTransform& Result, float& Duration);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void GetPSActorValue_History(AActor* InActor, FGameplayTag Key, int DistanceFromNow, bool& bIsGet, AActor*& Result, float& Duration);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void PS_GetPSActorValue_History(const UAnimInstance* AnimInstance, FGameplayTag Key, int DistanceFromNow, bool& bIsGet, AActor*& Result, float& Duration);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void GetPSTextValue_History(AActor* InActor, FGameplayTag Key, int DistanceFromNow, bool& bIsGet, FString& Result, float& Duration);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void PS_GetPSTextValue_History(const UAnimInstance* AnimInstance, FGameplayTag Key, int DistanceFromNow, bool& bIsGet, FString& Result, float& Duration);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void GetPSGTValue_History(AActor* InActor, FGameplayTag Key, int DistanceFromNow, bool& bIsGet, FGameplayTag& Result, float& Duration);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void PS_GetPSGTValue_History(const UAnimInstance* AnimInstance, FGameplayTag Key, int DistanceFromNow, bool& bIsGet, FGameplayTag& Result, float& Duration);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void GetPSBoolValue_History(AActor* InActor, FGameplayTag Key, int DistanceFromNow, bool& bIsGet, bool& Result, float& Duration);
	UFUNCTION(BlueprintPure, Category = "GBW|PS|Pure", meta=(DefaultToSelf="AnimInstance", BlueprintThreadSafe))
	static void PS_GetPSBoolValue_History(const UAnimInstance* AnimInstance, FGameplayTag Key, int DistanceFromNow, bool& bIsGet, bool& Result, float& Duration);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void ActivePSDebug(AActor* InActor, FColor InDebugTextColor = FColor::Green, bool InPrintToScreen = true, bool InPrintToLog = true);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS", meta=(DefaultToSelf="InActor"))
	static void DeactivePSDebug(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "GBW|PE")
	static bool GetEventFValue(FPE_Event InEvent, FGameplayTag Key, float& Result);
	UFUNCTION(BlueprintCallable, Category = "GBW|PE")
	static bool GetEventVValue(FPE_Event InEvent, FGameplayTag Key, FVector& Result);
	UFUNCTION(BlueprintCallable, Category = "GBW|PE")
	static bool GetEventTValue(FPE_Event InEvent, FGameplayTag Key, FTransform& Result);
	UFUNCTION(BlueprintCallable, Category = "GBW|PE")
	static bool GetEventActorValue(FPE_Event InEvent, FGameplayTag Key, AActor*& Result);
	UFUNCTION(BlueprintCallable, Category = "GBW|PE")
	static bool GetEventTextValue(FPE_Event InEvent, FGameplayTag Key, FString& Result);
	UFUNCTION(BlueprintCallable, Category = "GBW|PE")
	static bool GetEventGTValue(FPE_Event InEvent, FGameplayTag Key, FGameplayTag& Result);
	UFUNCTION(BlueprintCallable, Category = "GBW|PE")
	static bool GetEventBoolValue(FPE_Event InEvent, FGameplayTag Key, bool& Result);

	UFUNCTION(BlueprintCallable, Category = "GBW|PE")
	static FPE_Event MakePE_Event(FGameplayTag EventKey);
	UFUNCTION(BlueprintCallable, Category = "GBW|PE")
	static FPE_Event AddStateToPE(FPE_Event InEvent, FPS_State EventState);
	UFUNCTION(BlueprintCallable, Category = "GBW|PE")
	static FPE_Event AddFloatToPE(FPE_Event InEvent, FGameplayTag Key, float Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PE")
	static FPE_Event AddVectorToPE(FPE_Event InEvent, FGameplayTag Key, FVector Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PE")
	static FPE_Event AddTransformToPE(FPE_Event InEvent, FGameplayTag Key, FTransform Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PE")
	static FPE_Event AddActorToPE(FPE_Event InEvent, FGameplayTag Key, AActor* Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PE")
	static FPE_Event AddTextToPE(FPE_Event InEvent, FGameplayTag Key, FString Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PE")
	static FPE_Event AddGTToPE(FPE_Event InEvent, FGameplayTag Key, FGameplayTag Value);
	UFUNCTION(BlueprintCallable, Category = "GBW|PE")
	static FPE_Event AddBoolToPE(FPE_Event InEvent, FGameplayTag Key, bool Value);

	UFUNCTION(BlueprintCallable,Category = "GBW|PE", meta=(DefaultToSelf="InActor"))
	static void SendPowerfulEvent(AActor* InActor, FPE_Event PE_Event);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PE")
	static FString GetPEEventDebugString(FPE_Event InEvent);
	UFUNCTION(BlueprintCallable, Category = "GBW|PS")
	static FString GetPSEventDebugString(FPS_Event InEvent);
};
