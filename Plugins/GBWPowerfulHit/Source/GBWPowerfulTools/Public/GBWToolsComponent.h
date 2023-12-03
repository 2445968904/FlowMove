// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GBWToolsComponent.generated.h"


USTRUCT(BlueprintType, DisplayName="SpawnRules")
struct FGBWStructItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	FString Key = FString("");
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	FString StructStr = FString("");

	FGBWStructItem(){}
	FGBWStructItem(FString InKey, FString InStructStr)
	{
		Key = InKey;
		StructStr = InStructStr;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBWPOWERFULTOOLS_API UGBWToolsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGBWToolsComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(Replicated)
	TArray<FGBWStructItem> StructItems;
	
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void AddProperty(
		FString Key,
		FString Value);
	UFUNCTION(Reliable, Server, Category = "GBW|Effect")
	void AddProperty_Server(
		const FString& Key,
		const FString& Value);
	void AddProperty_Imp(
		FString Key,
		FString Value);

	bool GetProperty(const FString& Key, FString& Value);
};
