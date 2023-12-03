// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraEmitter.h"
#include "Curves/CurveVector.h"
#include "Engine/Engine.h"
#include "GBWNumericGeneratorScript.generated.h"

UCLASS()
class GBWNUMERICGENERATOR_API UGBWNumericGeneratorNetWorkObject : public UObject
{
	GENERATED_BODY()

	virtual bool IsNameStableForNetworking() const override
	{
		return true;
	}

	virtual bool IsSupportedForNetworking() const override
	{
		return true;
	}

protected:
	struct FWorldContext* WorldContext;

public:

	UGBWNumericGeneratorNetWorkObject(){}

	// Allows the Object to use BP_Functions
	UFUNCTION(BlueprintCallable, Category="WorldContext")
	void SetWorldContext(UObject* NewWorldContext);

	UFUNCTION(BlueprintCallable, Category = "WorldContext")
	UObject* GetWorldContextObject();

	//~ Begin UObject Interface
	virtual class UWorld* GetWorld() const override final;
	struct FWorldContext* GetWorldContext() const { return WorldContext; }
};

UENUM(BlueprintType)
enum class EGBWNumericGeneratorMode :uint8
{
	DirectlyValue UMETA(DisplayName = "DirectlyValue"),
	GenerateValue UMETA(DisplayName = "GenerateValue")
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_Bool_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_Bool : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	bool GenerateValue(AActor* OwnerActor);
	virtual bool GenerateValue_Implementation(AActor* OwnerActor) {
		return false;
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	bool GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	bool Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_Float_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_Float : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	float GenerateValue(AActor* OwnerActor);
	virtual float GenerateValue_Implementation(AActor* OwnerActor) {
		return 0.0f;
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	float GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	float Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_Vector_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_Vector : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	FVector GenerateValue(AActor* OwnerActor);
	virtual FVector GenerateValue_Implementation(AActor* OwnerActor) {
		return FVector::ZeroVector;
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	FVector GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	FVector Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_Rotator_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_Rotator : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	FRotator GenerateValue(AActor* OwnerActor);
	virtual FRotator GenerateValue_Implementation(AActor* OwnerActor) {
		return FRotator::ZeroRotator;
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	FRotator GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	FRotator Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_Transform_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_Transform : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	FTransform GenerateValue(AActor* OwnerActor);
	virtual FTransform GenerateValue_Implementation(AActor* OwnerActor) {
		return FTransform();
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	FTransform GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	FTransform Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_GameplayTag_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_GameplayTag : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	FGameplayTag GenerateValue(AActor* OwnerActor);
	virtual FGameplayTag GenerateValue_Implementation(AActor* OwnerActor) {
		return FGameplayTag::EmptyTag;
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	FGameplayTag GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	FGameplayTag Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_Text_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_Text : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	FText GenerateValue(AActor* OwnerActor);
	virtual FText GenerateValue_Implementation(AActor* OwnerActor) {
		return FText();
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	FText GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	FText Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_String_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_String : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	FString GenerateValue(AActor* OwnerActor);
	virtual FString GenerateValue_Implementation(AActor* OwnerActor) {
		return FString();
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	FString GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	FString Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_Name_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_Name : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	FName GenerateValue(AActor* OwnerActor);
	virtual FName GenerateValue_Implementation(AActor* OwnerActor) {
		return FName();
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	FName GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	FName Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_CurveFloat_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_CurveFloat : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	UCurveFloat* GenerateValue(AActor* OwnerActor);
	virtual UCurveFloat* GenerateValue_Implementation(AActor* OwnerActor) {
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	UCurveFloat* GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	UCurveFloat* Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_CurveVector_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_CurveVector : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	UCurveVector* GenerateValue(AActor* OwnerActor);
	virtual UCurveVector* GenerateValue_Implementation(AActor* OwnerActor) {
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	UCurveVector* GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	UCurveVector* Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_Montage_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_Montage : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	UAnimMontage* GenerateValue(AActor* OwnerActor);
	virtual UAnimMontage* GenerateValue_Implementation(AActor* OwnerActor) {
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	UAnimMontage* GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	UAnimMontage* Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_AnimSequence_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_AnimSequence : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	UAnimSequence* GenerateValue(AActor* OwnerActor);
	virtual UAnimSequence* GenerateValue_Implementation(AActor* OwnerActor) {
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	UAnimSequence* GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	UAnimSequence* Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_AnimBlendSpace_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_AnimBlendSpace : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	UBlendSpace* GenerateValue(AActor* OwnerActor);
	virtual UBlendSpace* GenerateValue_Implementation(AActor* OwnerActor) {
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	UBlendSpace* GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	UBlendSpace* Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_AnimBlueprint_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_AnimBlueprint : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	UAnimBlueprint* GenerateValue(AActor* OwnerActor);
	virtual UAnimBlueprint* GenerateValue_Implementation(AActor* OwnerActor) {
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	UAnimBlueprint* GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	UAnimBlueprint* Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_SkeletalMesh_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_SkeletalMesh : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	USkeletalMesh* GenerateValue(AActor* OwnerActor);
	virtual USkeletalMesh* GenerateValue_Implementation(AActor* OwnerActor) {
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	USkeletalMesh* GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	USkeletalMesh* Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_StaticlMesh_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_StaticMesh : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	UStaticMesh* GenerateValue(AActor* OwnerActor);
	virtual UStaticMesh* GenerateValue_Implementation(AActor* OwnerActor) {
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	UStaticMesh* GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	UStaticMesh* Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_Niagara_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_Niagara : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	UNiagaraSystem* GenerateValue(AActor* OwnerActor);
	virtual UNiagaraSystem* GenerateValue_Implementation(AActor* OwnerActor) {
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	UNiagaraSystem* GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	UNiagaraSystem* Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_Sound_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_Sound : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	USoundBase* GenerateValue(AActor* OwnerActor);
	virtual USoundBase* GenerateValue_Implementation(AActor* OwnerActor) {
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	USoundBase* GetValue(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
	UFUNCTION(BlueprintPure, Category = "GBW|NG")
	USoundBase* Value(AActor* OwnerActor)
	{
		return GenerateValue(OwnerActor);
	}
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_Bool"))
struct FNG_Bool
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	bool Value = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_Bool* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_Float"))
struct FNG_Float
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	float Value = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_Float* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_Vector"))
struct FNG_Vector
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	FVector Value = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_Vector* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_Rotator"))
struct FNG_Rotator
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	FRotator Value = FRotator::ZeroRotator;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_Rotator* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_Transform"))
struct FNG_Transform
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	FTransform Value = FTransform();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_Transform* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_GameplayTag"))
struct FNG_GameplayTag
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	FGameplayTag Value = FGameplayTag();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_GameplayTag* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_Text"))
struct FNG_Text
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	FText Value = FText();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_Text* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_String"))
struct FNG_String
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	FString Value = FString();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_String* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_Name"))
struct FNG_Name
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	FName Value = FName();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_Name* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_CurveFloat"))
struct FNG_CurveFloat
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	UCurveFloat* Value = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_CurveFloat* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_CurveVector"))
struct FNG_CurveVector
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	UCurveVector* Value = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_CurveVector* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_Montage"))
struct FNG_Montage
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	UAnimMontage* Value = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_Montage* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_AnimSequence"))
struct FNG_AnimSequence
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	UAnimSequence* Value = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_AnimSequence* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_AnimBlendSpace"))
struct FNG_AnimBlendSpace
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	UBlendSpace* Value = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_AnimBlendSpace* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_AnimBlueprint"))
struct FNG_AnimBlueprint
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	UAnimBlueprint* Value = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_AnimBlueprint* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_SkeletalMesh"))
struct FNG_SkeletalMesh
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	USkeletalMesh* Value = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_SkeletalMesh* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_StaticMesh"))
struct FNG_StaticMesh
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	UStaticMesh* Value = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_StaticMesh* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_Niagara"))
struct FNG_Niagara
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	UNiagaraSystem* Value = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_Niagara* Generator;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_Sound"))
struct FNG_Sound
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNumericGeneratorMode GeneratorMode = EGBWNumericGeneratorMode::DirectlyValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::DirectlyValue"))
	USoundBase* Value = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Settings",
		meta=(EditConditionHides,
			EditCondition="GeneratorMode==EGBWNumericGeneratorMode::GenerateValue"))
	UGBWNumericGenerator_Sound* Generator;
};

UENUM(BlueprintType)
enum class EGBWNumericGeneratorValueType :uint8
{
	Bool,
	Float,
	Vector,
	Rotator,
	Transform,
	GameplayTag,
	String,
	Text,
	Name,
	CurveFloat,
	CurveVector,
	Montage,
	AnimSequence,
	AnimBlendSpace,
	AnimBlueprint,
	SkeletalMesh,
	StaticMesh,
	Niagara,
	Sound,
	Other
};
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "NG_Value_Base"))
class GBWNUMERICGENERATOR_API UGBWNumericGenerator_Value_Base : public UGBWNumericGeneratorNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	EGBWNumericGeneratorValueType GetTheValueType();
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() {
		return EGBWNumericGeneratorValueType::Float;
	}
	UFUNCTION(BlueprintCallable, Category = "GBW|NG")
	EGBWNumericGeneratorValueType GetValueType()
	{
		return GetTheValueType();
	}

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	bool GetValue_Bool(AActor* OwnerActor, bool DefaultValue);
	virtual bool GetValue_Bool_Implementation(AActor* OwnerActor, bool DefaultValue){return DefaultValue;}
	
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	float GetValue_Float(AActor* OwnerActor, float DefaultValue);
	virtual float GetValue_Float_Implementation(AActor* OwnerActor, float DefaultValue){return DefaultValue;}
	
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	FVector GetValue_Vector(AActor* OwnerActor, FVector DefaultValue);
	virtual FVector GetValue_Vector_Implementation(AActor* OwnerActor, FVector DefaultValue){return DefaultValue;}

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	FRotator GetValue_Rotator(AActor* OwnerActor, FRotator DefaultValue);
	virtual FRotator GetValue_Rotator_Implementation(AActor* OwnerActor, FRotator DefaultValue){return DefaultValue;}
	
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	FTransform GetValue_Transform(AActor* OwnerActor, FTransform DefaultValue);
	virtual FTransform GetValue_Transform_Implementation(AActor* OwnerActor, FTransform DefaultValue){return DefaultValue;}
	
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	FGameplayTag GetValue_GameplayTag(AActor* OwnerActor, FGameplayTag DefaultValue);
	virtual FGameplayTag GetValue_GameplayTag_Implementation(AActor* OwnerActor, FGameplayTag DefaultValue){return DefaultValue;}
	
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	FText GetValue_Text(AActor* OwnerActor, const FText& DefaultValue);
	virtual FText GetValue_Text_Implementation(AActor* OwnerActor, const FText& DefaultValue){return DefaultValue;}

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	FString GetValue_String(AActor* OwnerActor, const FString& DefaultValue);
	virtual FString GetValue_String_Implementation(AActor* OwnerActor, const FString& DefaultValue){return DefaultValue;}

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	FName GetValue_Name(AActor* OwnerActor, const FName& DefaultValue);
	virtual FName GetValue_Name_Implementation(AActor* OwnerActor, const FName& DefaultValue){return DefaultValue;}

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	UCurveFloat* GetValue_CurveFloat(AActor* OwnerActor, UCurveFloat* DefaultValue);
	virtual UCurveFloat* GetValue_CurveFloat_Implementation(AActor* OwnerActor, UCurveFloat* DefaultValue){return DefaultValue;}
	
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	UCurveVector* GetValue_CurveVector(AActor* OwnerActor, UCurveVector* DefaultValue);
	virtual UCurveVector* GetValue_CurveVector_Implementation(AActor* OwnerActor, UCurveVector* DefaultValue){return DefaultValue;}
	
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	UAnimMontage* GetValue_Montage(AActor* OwnerActor, UAnimMontage* DefaultValue);
	virtual UAnimMontage* GetValue_Montage_Implementation(AActor* OwnerActor, UAnimMontage* DefaultValue){return DefaultValue;}
	
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	UAnimSequence* GetValue_AnimSequence(AActor* OwnerActor, UAnimSequence* DefaultValue);
	virtual UAnimSequence* GetValue_AnimSequence_Implementation(AActor* OwnerActor, UAnimSequence* DefaultValue){return DefaultValue;}
	
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	UBlendSpace* GetValue_AnimBlendSpace(AActor* OwnerActor, UBlendSpace* DefaultValue);
	virtual UBlendSpace* GetValue_AnimBlendSpace_Implementation(AActor* OwnerActor, UBlendSpace* DefaultValue){return DefaultValue;}
	
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	UAnimBlueprint* GetValue_AnimBlueprint(AActor* OwnerActor, UAnimBlueprint* DefaultValue);
	virtual UAnimBlueprint* GetValue_AnimBlueprint_Implementation(AActor* OwnerActor, UAnimBlueprint* DefaultValue){return DefaultValue;}
	
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	USkeletalMesh* GetValue_SkeletalMesh(AActor* OwnerActor, USkeletalMesh* DefaultValue);
	virtual USkeletalMesh* GetValue_SkeletalMesh_Implementation(AActor* OwnerActor, USkeletalMesh* DefaultValue){return DefaultValue;}
	
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	UStaticMesh* GetValue_StaticMesh(AActor* OwnerActor, UStaticMesh* DefaultValue);
	virtual UStaticMesh* GetValue_StaticMesh_Implementation(AActor* OwnerActor, UStaticMesh* DefaultValue){return DefaultValue;}
	
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	UNiagaraSystem* GetValue_Niagara(AActor* OwnerActor, UNiagaraSystem* DefaultValue);
	virtual UNiagaraSystem* GetValue_Niagara_Implementation(AActor* OwnerActor, UNiagaraSystem* DefaultValue){return DefaultValue;}
	
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|NG")
	USoundBase* GetValue_Sound(AActor* OwnerActor, USoundBase* DefaultValue);
	virtual USoundBase* GetValue_Sound_Implementation(AActor* OwnerActor, USoundBase* DefaultValue){return DefaultValue;}
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "Bool"))
class GBWNUMERICGENERATOR_API UGBWNG_Bool : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_Bool Value = FNG_Bool();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::Bool;
	}
	virtual bool GetValue_Bool_Implementation(AActor* OwnerActor, bool DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "Float"))
class GBWNUMERICGENERATOR_API UGBWNG_Float : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_Float Value = FNG_Float();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::Float;
	}
	virtual float GetValue_Float_Implementation(AActor* OwnerActor, float DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "Vector"))
class GBWNUMERICGENERATOR_API UGBWNG_Vector : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_Vector Value = FNG_Vector();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::Vector;
	}
	virtual FVector GetValue_Vector_Implementation(AActor* OwnerActor, FVector DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "Rotator"))
class GBWNUMERICGENERATOR_API UGBWNG_Rotator : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_Rotator Value = FNG_Rotator();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::Vector;
	}
	virtual FRotator GetValue_Rotator_Implementation(AActor* OwnerActor, FRotator DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "Transform"))
class GBWNUMERICGENERATOR_API UGBWNG_Transform : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_Transform Value = FNG_Transform();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::Transform;
	}
	virtual FTransform GetValue_Transform_Implementation(AActor* OwnerActor, FTransform DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "GameplayTag"))
class GBWNUMERICGENERATOR_API UGBWNG_GameplayTag : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_GameplayTag Value = FNG_GameplayTag();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::GameplayTag;
	}
	virtual FGameplayTag GetValue_GameplayTag_Implementation(AActor* OwnerActor, FGameplayTag DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "Text"))
class GBWNUMERICGENERATOR_API UGBWNG_Text : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_Text Value = FNG_Text();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::Text;
	}
	virtual FText GetValue_Text_Implementation(AActor* OwnerActor, const FText& DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "String"))
class GBWNUMERICGENERATOR_API UGBWNG_String : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_String Value = FNG_String();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::String;
	}
	virtual FString GetValue_String_Implementation(AActor* OwnerActor, const FString& DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "Name"))
class GBWNUMERICGENERATOR_API UGBWNG_Name : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_Name Value = FNG_Name();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::Name;
	}
	virtual FName GetValue_Name_Implementation(AActor* OwnerActor, const FName& DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "CurveFloat"))
class GBWNUMERICGENERATOR_API UGBWNG_CurveFloat : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_CurveFloat Value = FNG_CurveFloat();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::CurveFloat;
	}
	virtual UCurveFloat* GetValue_CurveFloat_Implementation(AActor* OwnerActor, UCurveFloat* DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "CurveVector"))
class GBWNUMERICGENERATOR_API UGBWNG_CurveVector : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_CurveVector Value = FNG_CurveVector();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::CurveVector;
	}
	virtual UCurveVector* GetValue_CurveVector_Implementation(AActor* OwnerActor, UCurveVector* DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "Montage"))
class GBWNUMERICGENERATOR_API UGBWNG_Montage : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_Montage Value = FNG_Montage();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::Montage;
	}
	virtual UAnimMontage* GetValue_Montage_Implementation(AActor* OwnerActor, UAnimMontage* DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "AnimSequence"))
class GBWNUMERICGENERATOR_API UGBWNG_AnimSequence : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_AnimSequence Value = FNG_AnimSequence();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::AnimSequence;
	}
	virtual UAnimSequence* GetValue_AnimSequence_Implementation(AActor* OwnerActor, UAnimSequence* DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "AnimBlendSpace"))
class GBWNUMERICGENERATOR_API UGBWNG_AnimBlendSpace : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_AnimBlendSpace Value = FNG_AnimBlendSpace();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::AnimBlendSpace;
	}
	virtual UBlendSpace* GetValue_AnimBlendSpace_Implementation(AActor* OwnerActor, UBlendSpace* DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "AnimBlueprint"))
class GBWNUMERICGENERATOR_API UGBWNG_AnimBlueprint : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_AnimBlueprint Value = FNG_AnimBlueprint();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::AnimBlueprint;
	}
	virtual UAnimBlueprint* GetValue_AnimBlueprint_Implementation(AActor* OwnerActor, UAnimBlueprint* DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "SkeletalMesh"))
class GBWNUMERICGENERATOR_API UGBWNG_SkeletalMesh : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_SkeletalMesh Value = FNG_SkeletalMesh();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::SkeletalMesh;
	}
	virtual USkeletalMesh* GetValue_SkeletalMesh_Implementation(AActor* OwnerActor, USkeletalMesh* DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "StaticMesh"))
class GBWNUMERICGENERATOR_API UGBWNG_StaticMesh : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_StaticMesh Value = FNG_StaticMesh();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::StaticMesh;
	}
	virtual UStaticMesh* GetValue_StaticMesh_Implementation(AActor* OwnerActor, UStaticMesh* DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "Niagara"))
class GBWNUMERICGENERATOR_API UGBWNG_Niagara : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_Niagara Value = FNG_Niagara();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::Niagara;
	}
	virtual UNiagaraSystem* GetValue_Niagara_Implementation(AActor* OwnerActor, UNiagaraSystem* DefaultValue) override;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "Sound"))
class GBWNUMERICGENERATOR_API UGBWNG_Sound : public UGBWNumericGenerator_Value_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	FNG_Sound Value = FNG_Sound();
	
	virtual EGBWNumericGeneratorValueType GetTheValueType_Implementation() override
	{
		return EGBWNumericGeneratorValueType::Sound;
	}
	virtual USoundBase* GetValue_Sound_Implementation(AActor* OwnerActor, USoundBase* DefaultValue) override;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_ValueGroup"))
struct FNG_ValueGroup
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(MultiLine=true))
	FText Description = FText::FromString("No ValueGroup Description");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced, Category = "Settings", meta=(ForceInlineRow, TitleProperty="Description"))
	TMap<FGameplayTag, UGBWNumericGenerator_Value_Base*> Values;
};

USTRUCT(BlueprintType, meta = (DisplayName = "NG_Set"))
struct FNG_Set
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FString Description = "No Description";
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(ForceInlineRow, TitleProperty="Description"))
	TMap<FGameplayTag,FNG_ValueGroup> ValueGroups;
};