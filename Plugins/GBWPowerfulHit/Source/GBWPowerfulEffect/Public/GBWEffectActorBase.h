// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GBWPowerfulToolsFuncLib.h"
#include "GBWPowerfulToolsFuncLib.h"
#include "GameFramework/Actor.h"
#include "GBWEffectActorBase.generated.h"

class UGBWPowerfulEffectComponent;

UCLASS(HideDropdown)
class GBWPOWERFULEFFECT_API AGBWEffectActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGBWEffectActorBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Replicated, Category="GBW|Effect")
	FGuid EffectGuid = FGuid();
	UPROPERTY(BlueprintReadOnly, Replicated, Category="GBW|Effect")
	TEnumAsByte<EGBWActorNetType> OwnerActorNetType = EGBWActorNetType::None;
	UPROPERTY(BlueprintReadOnly, Replicated, Category="GBW|Effect")
	AActor* OwnerActor = nullptr;
	UPROPERTY(BlueprintReadOnly, Replicated, Category="GBW|Effect")
	FString EffectParamsStr = FString();

	bool bIsStarted = false;

	UFUNCTION(BlueprintCallable,Category = "GBW|Effect")
	void EndEffect();
	UFUNCTION(BlueprintPure,Category = "GBW|Effect")
	AActor* GetOwnerActor() const;

	void DestroyEffect();

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|Effect")
	void OnEffectStart(EGBWActorNetType OwnerNetType);
	virtual void OnEffectStart_Implementation(EGBWActorNetType OwnerNetType){}
	void EffectStart(EGBWActorNetType OwnerNetType)
	{
		if (!bIsStarted)
		{
			OnEffectStart(OwnerNetType);
			bIsStarted = true;
		}
	}

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|Effect")
	void OnEffectUpdate(EGBWActorNetType OwnerNetType, float DeltaTime);
	virtual void OnEffectUpdate_Implementation(EGBWActorNetType OwnerNetType, float DeltaTime){}
	void EffectUpdate(EGBWActorNetType OwnerNetType, float DeltaTime){OnEffectUpdate(OwnerNetType, DeltaTime);}

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|Effect")
	void OnEffectEnd(EGBWActorNetType OwnerNetType);
	virtual void OnEffectEnd_Implementation(EGBWActorNetType OwnerNetType){}
	void EffectEnd(EGBWActorNetType OwnerNetType);
	UFUNCTION(Reliable, Server, Category = "GBW|Effect")
	void EffectEnd_Server(EGBWActorNetType OwnerNetType);
	UFUNCTION(Reliable, NetMulticast, Category = "GBW|Effect")
	void EffectEnd_Multicast(EGBWActorNetType OwnerNetType);
	void EffectEnd_Imp(EGBWActorNetType OwnerNetType);

	UFUNCTION(BlueprintCallable, 
		CustomThunk,
		meta = (CustomStructureParam = "EffectParameters"),
		Category = "GBW|Effect")
	static bool InitEffectParameters(const int32& EffectParameters) { return false; }
	DECLARE_FUNCTION(execInitEffectParameters);
};
