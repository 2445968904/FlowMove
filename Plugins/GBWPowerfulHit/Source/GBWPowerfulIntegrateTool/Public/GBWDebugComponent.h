// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GBWDebugComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBWPOWERFULINTEGRATETOOL_API UGBWDebugComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGBWDebugComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="GBW|Debug")
	bool bIsDebug = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="GBW|Debug")
	FColor DebugTextColor = FColor::Green;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="GBW|Debug")
	bool bPrintToScreen = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="GBW|Debug")
	bool bPrintToLog = true;
	
	UPROPERTY()
	TMap<FString, int> CounterPrintCache;

	UFUNCTION(BlueprintCallable, Category="GBW|Debug")
	static UGBWDebugComponent* GetGbwDebugComponent(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category="GBW|Debug")
	static void GBWDebug_CounterPrint(AActor* InActor, FString Key);

	UFUNCTION(BlueprintCallable, Category="GBW|Debug|DebugComponent")
	void CounterPrint(FString Key);

	void DebugTick();
};
