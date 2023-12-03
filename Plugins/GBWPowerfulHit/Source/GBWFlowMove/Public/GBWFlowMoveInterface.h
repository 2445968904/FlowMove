// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "GBWFlowMoveInterface.generated.h"

UINTERFACE(MinimalAPI)
class UGBWFlowMoveInterface : public UInterface
{
	GENERATED_BODY()
};
class GBWFLOWMOVE_API IGBWFlowMoveInterface
{
	GENERATED_BODY()

public:
};

UINTERFACE(MinimalAPI)
class UGBWFlowMoveInteractiveActorInterface : public UInterface
{
	GENERATED_BODY()
};
class GBWFLOWMOVE_API IGBWFlowMoveInteractiveActorInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="GBW|FlowMove")
	void CanInteractive(
		ACharacter* FromCharacter,
		UGBWFlowMoveComponent* FromFlowMoveComponent,
		bool& CanInteractive);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="GBW|FlowMove")
	void GetTargetPoint(
		ACharacter* FromCharacter,
		UGBWFlowMoveComponent* FromFlowMoveComponent,
		bool& bIsGet,
		FVector& TargetPoint,
		FName& SceneTag);
};
