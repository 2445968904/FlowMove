// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GBWPowerfulToolsFuncLib.generated.h"

UENUM(BlueprintType)
enum class EGBWActorNetType : uint8
{
	None,
	InClient_NotPlayer,
	InClient_RemotePlayer,
	InClient_LocalPlayer,
	InServer_NotPlayer,
	InServer_RemotePlayer,
	InServer_LocalPlayer
};

UCLASS()
class GBWPOWERFULTOOLS_API UGBWPowerfulToolsFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "GBW|Tools", meta=(DefaultToSelf="InActor"))
	static UGBWToolsComponent* GetGBWtToolsComponent(AActor* InActor);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|Tools",
			CustomThunk,
			meta = (DefaultToSelf="InActor", CustomStructureParam = "StructProperty"))
	static void SetGBWProperty(
		AActor* InActor,
		FString Key,
		const int32& StructProperty){}
	DECLARE_FUNCTION(execSetGBWProperty);

	UFUNCTION(BlueprintCallable, 
		CustomThunk,
		meta = (DefaultToSelf="InActor", CustomStructureParam = "PropertyStruct"),
		Category = "GBW|Tools")
	static bool GetGBWPropertyAsStruct(AActor* InActor, FString Key, const int32& PropertyStruct) { return false; }
	DECLARE_FUNCTION(execGetGBWPropertyAsStruct);


	UFUNCTION(BlueprintCallable, Category = "GBW|Tools", meta=(DefaultToSelf="InActor"))
	static void GetPlayerCamera(
		AActor* InActor,
		bool& bIsGet,
		UCameraComponent*& Camera,
		USpringArmComponent*& SpringArm);

	UFUNCTION(BlueprintCallable, Category = "GBW|Tools", meta=(DefaultToSelf="InActor"))
	static void GetPlayerController(
		AActor* InActor,
		bool& bIsGet,
		bool& bIsLocalController,
		APlayerController*& PlayerController);

	UFUNCTION(BlueprintCallable, Category = "GBW|Tools", meta=(DefaultToSelf="InActor"))
	static EGBWActorNetType GetActorNetType(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "GBW|Tools", meta=(DefaultToSelf="InObject"))
	static void GetAllPropertiesStr(UObject* InObject, TArray<FString>& PropertyNames, TArray<FString>& PropertyValues);
	
	static FString StructToText_Imp(const void* StructPtr, FProperty* Property);
	UFUNCTION(BlueprintCallable, 
		CustomThunk,
		meta = (CustomStructureParam = "InStruct"),
		Category = "GBW|Tools")
	static FString StructToText(const int32& InStruct) { return ""; }
	DECLARE_FUNCTION(execStructToText);

	static bool TextToStruct_Imp(const FString& InText,void* StructPtr, FProperty* Property);
	UFUNCTION(BlueprintCallable, 
		CustomThunk,
		meta = (CustomStructureParam = "InStruct"),
		Category = "GBW|Tools")
	static bool TextToStruct(FString InText, const int32& InStruct) { return false; }
	DECLARE_FUNCTION(execTextToStruct);
};
